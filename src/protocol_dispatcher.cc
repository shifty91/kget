/*
 * Copyright (C) 2015-2017 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#include <libgen.h>

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

Request ProtocolDispatcher::build_request() const
{
    URLParser parser(m_url);
    std::string name;

    parser.parse();
    log_dbg("Parsing results...");
    log_dbg("  Method: " << parser.method());
    log_dbg("  Host: "   << parser.host()  );
    log_dbg("  Object: " << parser.object());

    if (m_output == "") {
        name = GET_BASENAME(parser.object().c_str());
        if (name == "")
            EXCEPTION("URL does not have a valid object.");
    } else
        name = m_output;

    return { parser.method(), parser.host(), parser.object(), name, m_user, m_pw };
}

void ProtocolDispatcher::dispatch()
{
    Config *config = Config::instance();

    while (42) {
        auto req = build_request();

        // here: catch only redirect|auth exceptions, everything else is just forwarded
        try {
            if (req.method() == "http") {
                HTTPMethod<>().get(req);
#ifdef HAVE_OPENSSL
            } else if (req.method() == "https") {
                HTTPMethod<TCPSSLConnection>().get(req);
#endif
            } else if (req.method() == "ftp") {
                FTPMethod().get(req);
#ifdef HAVE_LIBSSH
            } else if (req.method() == "sftp") {
                SFTPMethod().get(req);
#endif
            } else {
                EXCEPTION("The method " << req.method() <<
                          " is not supported right now.");
            }
        } catch (const RedirectException& ex) {
            if (config->follow_redirects()) {
                const auto& url = ex.url();
                log_info("HTTP redirect detected. Going to URL: " << url);
                m_url = url;
                continue;
            }
            log_info("HTTP redirect detected. Following redirects disabled.");
            break;
        } catch (const AuthException&) {
            log_info("HTTP Authorization detected. Please provide your credentials: ");
            m_user = Utils::user_input("Username");
            m_pw   = Utils::user_input_pw("Password");
            continue;
        }

        log_info("File saved to " << req.out_file_name());

        break;
    }
}
