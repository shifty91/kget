#ifndef _HTTP_H_
#define _HTTP_H_

#include <iostream>
#include <sstream>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <string>
#include <vector>

#include "logger.h"
#include "method.h"
#include "tcp_connection.h"
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

    template<>
    class Service<TCPSSLConnection>
    {
    public:
        static const int PORT = 443;
    };

}

template<typename CONNECTION = TCPConnection>
class HTTPMethod : public Method
{
private:
    std::string build_http_request(const std::string& user, const std::string& pw) const
    {
        std::stringstream request;
        std::string object{m_object};

        if (m_object[0] != '/') {
            std::stringstream ss;
            ss << "/" << m_object;
            object = ss.str();
        }

        request << "GET "   << object << " HTTP/1.1\r\n"
                << "Host: " << m_host << "\r\n"
                << "User-Agent: Kurts Get Program\r\n"
                << "Connection: Close\r\n";
        if (user != "") {
            std::stringstream auth;
            auth << user << ":" << pw;
            Base64 base64(auth.str());
            request << "Authorization: Basic "
                    << base64.encode() << "\r\n";
        }
        request << "\r\n";

        return request.str();
    }

    void check_response_code(const std::vector<std::string>& header) const
    {
        int code;
        const auto& firstLine = header[0];
        std::regex pattern("HTTP/(\\d+\\.\\d+)\\s*(\\d+).*\\r\\n");
        std::smatch match;
        std::string codeStr;

        std::regex_match(firstLine, match, pattern);

        if (match.size() != 3)
            EXCEPTION("Received malformed HTTP Header!");

        codeStr = match[2];
        code = std::atoi(codeStr.c_str());
        if (code == 404)
            EXCEPTION("The requested object cannot be found on the server!");

        if (code == 301 || code == 302) {
            auto url = http_get_redirect_url(header);
            throw RedirectException(url);
        }

        if (code == 401)
            throw AuthException();

        if (code != 200)
            EXCEPTION("Received unexpected response code from server: " << code);
    }

    std::vector<std::string> read_http_header(const CONNECTION& tcp) const
    {
        std::vector<std::string> result;
        std::string line, lastLine;

        result.reserve(10);

        while (line != "\r\n" && lastLine != "\r\n") {
            lastLine = line;
            line = tcp.read_ln();
            result.push_back(line);
        }

        return result;
    }

    std::size_t get_content_length(const std::vector<std::string>& header) const
    {
        for (const auto& line : header) {
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

public:
    HTTPMethod(const std::string& host, const std::string& object) :
        Method(host, object)
    {}

    virtual void get(const std::string& fileToSave, const std::string& user = "",
                     const std::string& pw = "") const override
    {
        CONNECTION tcp;
        std::string request;
        std::vector<std::string> header;
        Config *config = Config::instance();

        tcp.connect(m_host, HTTPHelpers::Service<CONNECTION>::PORT);
        request = build_http_request(user, pw);

        tcp.write(request);

        header = read_http_header(tcp);
        check_response_code(header);

        auto length = get_content_length(header);

        // save
        std::ofstream ofs(fileToSave);
        if (ofs.fail())
            EXCEPTION("Failed to open file: " << fileToSave);
        if (length > 0 && config->show_pg())
            tcp.read_until_eof_with_pg_to_fstream(ofs, length);
        else
            tcp.read_until_eof_to_fstream(ofs);
    }
};

#endif /* _HTTP_H_ */
