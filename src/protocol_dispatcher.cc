/*
 * Copyright (C) 2015-2019 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * This file is part of Get.
 *
 * Get is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Get is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Get.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <experimental/filesystem>

#include "get_config.h"
#include "tcp_connection.h"
#include "tcp_ssl_connection.h"
#include "url_parser.h"
#include "logger.h"
#include "http.h"
#include "ftp.h"
#include "sftp.h"
#include "redirect_exception.h"
#include "auth_exception.h"
#include "utils.h"
#include "config.h"

#include "protocol_dispatcher.h"

ProtocolDispatcher::ProtoMap ProtocolDispatcher::protoMap;
bool ProtocolDispatcher::initialized = false;

void ProtocolDispatcher::init()
{
    protoMap.emplace("http",  std::make_unique<HTTPMethod<> >());
#ifdef HAVE_OPENSSL
    protoMap.emplace("https", std::make_unique<HTTPMethod<TCPSSLConnection> >());
#endif
    protoMap.emplace("ftp",   std::make_unique<FTPMethod<> >());
#ifdef HAVE_OPENSSL
    protoMap.emplace("ftps",  std::make_unique<FTPMethod<TCPSSLConnection> >());
#endif
#ifdef HAVE_LIBSSH
    protoMap.emplace("sftp",  std::make_unique<SFTPMethod>());
#endif
    initialized = true;
}

Request ProtocolDispatcher::build_request() const
{
    namespace fs = std::experimental::filesystem;

    URLParser parser(m_url);
    std::string name;
    std::size_t start_offset = 0;

    parser.parse();

    if (m_output == "") {
        name = fs::path(parser.object()).filename();
        if (name == "")
            EXCEPTION("URL does not have a valid object.");
    } else
        name = m_output;

    if (Config::instance()->continue_download() && Utils::file_exists(name))
        start_offset = Utils::file_size(name);

    return { parser.method(), parser.host(), parser.object(), name,
             parser.user(), parser.pw(), start_offset };
}

void ProtocolDispatcher::dispatch()
{
    Config *config = Config::instance();
    std::string user, pw;

    if (!initialized)
        init();

    while (42) {
        auto req = build_request();

        // user/pw might be overriden by user input if auth fails
        if (!user.empty())
            req.user() = user;
        if (!pw.empty())
            req.pw() = pw;

        // here: catch only redirect|auth exceptions, everything else is just forwarded
        try {
            auto it = protoMap.find(req.method());
            if (it == protoMap.end())
                EXCEPTION("The method ", req.method()," is not supported right now.");

            it->second->get(req);
        } catch (const RedirectException& ex) {
            if (config->follow_redirects()) {
                const auto& url = ex.url();
                log_info("HTTP redirect detected. Going to URL: ", url);
                m_url = url;
                continue;
            }
            log_info("HTTP redirect detected. Following redirects disabled.");
            break;
        } catch (const AuthException&) {
            log_info("HTTP Authorization detected. Please provide your credentials: ");
            user = Utils::user_input("Username");
            pw   = Utils::user_input_pw("Password");
            continue;
        }

        log_info("File saved to ", req.out_file_name());

        break;
    }
}
