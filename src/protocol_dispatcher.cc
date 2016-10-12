/*
 * Copyright (C) 2015-2016 Kurt Kanzenbach <kurt@kmk-computers.de>
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

void ProtocolDispatcher::dispatch()
{
    Config *config = Config::instance();

    while (42) {
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

        // here: catch only redirect|auth exceptions, everything else is just forwarded
        try {
            if (parser.method() == "http") {
                HTTPMethod<> http(parser.host(), parser.object());
                http.get(name, m_user, m_pw);
            } else if (parser.method() == "https") {
                HTTPMethod<TCPSSLConnection> https(parser.host(), parser.object());
                https.get(name, m_user, m_pw);
            } else if (parser.method() == "ftp") {
                FTPMethod ftp(parser.host(), parser.object());
                ftp.get(name, m_user, m_pw);
            } else if (parser.method() == "sftp") {
                SFTPMethod sftp(parser.host(), parser.object());
                sftp.get(name, m_user, m_pw);
            } else {
                EXCEPTION("The method " << parser.method() <<
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

        log_info("File saved to " << name);

        break;
    }
}
