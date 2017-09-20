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

#ifndef _HTTP_H_
#define _HTTP_H_

#include <iostream>
#include <sstream>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>

#include "get_config.h"
#include "logger.h"
#include "method.h"
#include "tcp_connection.h"
#include "tcp_ssl_connection.h"
#include "redirect_exception.h"
#include "auth_exception.h"
#include "base64.h"
#include "config.h"

namespace HTTPHelpers {

    template<typename CONNECTION>
    class Service
    {};

    template<>
    class Service<TCPConnection>
    {
    public:
        static const int PORT = 80;
    };

#ifdef HAVE_OPENSSL
    template<>
    class Service<TCPSSLConnection>
    {
    public:
        static const int PORT = 443;
    };
#endif
}

template<typename CONNECTION = TCPConnection>
class HTTPMethod : public Method
{
public:
    HTTPMethod() :
        Method()
    {}

    virtual void get(const Request& req) const override
    {
        CONNECTION tcp;
        std::string request;
        std::vector<std::string> header;
        std::ios_base::openmode mode = std::ios_base::out;
        Config *config = Config::instance();
        int response;

        tcp.connect(req.host(), HTTPHelpers::Service<CONNECTION>::PORT);
        request = build_http_request(req);

        tcp << request;

        header = read_http_header(tcp);
        response = check_response_code(header);

        auto length = get_content_length(header);
        log_dbg("File has a size of " << (length + req.start_offset()) << " bytes.");

        // save
        if (response == 206)
            mode |= std::ios_base::app;
        std::ofstream ofs(req.out_file_name(), mode);
        if (ofs.fail())
            EXCEPTION("Failed to open file: " << req.out_file_name());
        if (length > 0 && config->show_pg())
            tcp.read_until_eof_with_pg_to_fstream(
                ofs, req.start_offset(), length + req.start_offset());
        else
            tcp.read_until_eof_to_fstream(ofs);
    }

private:
    std::string build_http_request(const Request& req) const
    {
        std::stringstream request;
        std::string slashed_object{req.object()};

        if (req.object()[0] != '/') {
            std::stringstream ss;
            ss << "/" << req.object();
            slashed_object = ss.str();
        }

        request << "GET "   << slashed_object << " HTTP/1.1\r\n"
                << "Host: " << req.host() << "\r\n"
                << "User-Agent: Kurts Get Program\r\n"
                << "Connection: Close\r\n";
        if (req.user() != "") {
#ifdef HAVE_OPENSSL
            std::stringstream auth;
            auth << req.user() << ":" << req.pw();
            Base64 base64(auth.str());
            request << "Authorization: Basic "
                    << base64.encode() << "\r\n";
#else
            EXCEPTION("OpenSSL is needed for HTTP Basic Auth.");
#endif
        }
        if (req.start_offset() > 0) {
            log_dbg("Trying to continue file download @ " << req.start_offset() << " bytes");
            request << "Range: bytes=" << req.start_offset() << "-\r\n";
        }
        request << "\r\n";

        return request.str();
    }

    int check_response_code(const std::vector<std::string>& header) const
    {
        int code;
        auto&& first_line = header[0];
        std::regex pattern("HTTP/(\\d+\\.\\d+)\\s*(\\d+).*\\r\\n");
        std::smatch match;
        std::string code_str;

        std::regex_match(first_line, match, pattern);

        if (match.size() != 3)
            EXCEPTION("Received malformed HTTP Header!");

        code_str = match[2];
        code = std::atoi(code_str.c_str());
        if (code == 404)
            EXCEPTION("The requested object cannot be found on the server!");

        if (code == 301 || code == 302) {
            auto url = http_get_redirect_url(header);
            throw RedirectException(url);
        }

        if (code == 401)
            throw AuthException();

        if (code != 200 && code != 206)
            EXCEPTION("Received unexpected response code from server: " << code);

        return code;
    }

    std::vector<std::string> read_http_header(const CONNECTION& tcp) const
    {
        std::vector<std::string> result;
        std::string line, last_line;

        result.reserve(10);

        while (line != "\r\n" && last_line != "\r\n") {
            last_line = line;
            line = tcp.read_ln();
            result.push_back(line);
        }

        return result;
    }

    std::size_t get_content_length(const std::vector<std::string>& header) const
    {
        for (auto&& line : header) {
            std::regex pattern("Content-Length:\\s*(\\d+)\\s*\\r\\n");
            std::smatch match;

            if (std::regex_match(line, match, pattern)) {
                std::string str = match[1];
                return std::atoll(str.c_str());
            }
        }

        log_dbg("Cannot find content length in HTTP response header.");

        return 0;
    }

    std::string http_get_redirect_url(const std::vector<std::string>& header) const
    {
        std::string url = "";

        for (auto&& line : header) {
            std::regex pattern("Location:\\s*(.*)\\r\\n");
            std::smatch match;

            if (std::regex_match(line, match, pattern)) {
                url = match[1];
                break;
            }
        }

        if (url == "")
            EXCEPTION("Failed to parse 301 HTTP response header!");

        return url;
    }

    void http_check_auth(const std::vector<std::string>& header) const
    {
        // supported right now: Basic AUTH
        std::string auth, realm;

        for (auto&& line : header) {
            std::regex pattern("WWW-Authenticate:\\s*(\\w+)\\s*realm=(\\w+)\\r\\n");
            std::smatch match;

            if (std::regex_match(line, match, pattern)) {
                auth  = match[1];
                realm = match[2];
                break;
            }
        }

        if (auth == "")
            EXCEPTION("Failed to parse 401 HTTP response header!");
        if (auth != "Basic")
            EXCEPTION("Unsupported HTTP auth: " << auth);

        log_info("HTTP Basic Authentication: " << realm);
    }
};

#endif /* _HTTP_H_ */
