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

#ifndef _FTP_H_
#define _FTP_H_

#include <cctype>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <type_traits>

#include "logger.h"
#include "utils.h"
#include "config.h"
#include "method.h"
#include "tcp_connection.h"
#include "tcp_ssl_connection.h"

template<typename CONNECTION = TCPConnection>
class FTPMethod : public Method
{
public:
    FTPMethod() :
        Method()
    {
        static_assert(std::is_same_v<CONNECTION, TCPConnection>
#ifdef HAVE_OPENSSL
                      || std::is_same_v<CONNECTION, TCPSSLConnection>
#endif
                      , "FTPMethod may only be used in combination with "
                      "TCPConnection or TCPSSLConnection");
    }

    virtual ~FTPMethod()
    {}

    virtual void get(const Request& req) const override
    {
        using namespace std::string_literals;

        CONNECTION tcp, tcp_pasv;
        std::uint16_t pasv_port;
        std::size_t len = 0;
        std::ios_base::openmode mode = std::ios_base::out;
        Config *config = Config::instance();

        tcp.connect(req.host(), get_port());
        auto line = read_response(tcp);
        log_dbg("RESPONSE: ", line);
        auto response = ftp_ret_code(line);
        check_response(220, response);

        // user/pass
        auto user_name = req.user() == "" ? "anonymous"s : req.user();
        auto pass = req.pw() == "" ? "asdf"s : req.pw();

        // login
        response = command_ret_code(tcp, "USER ", user_name, "\r\n");
        if (response == 230)
            goto logged_in;

        check_response(331, response);
        command_check(tcp, 230, "PASS ", pass, "\r\n");

    logged_in:
        log_dbg("Logged into FTP server at ", req.host());

        // configure to use encrypted data transfer as well
        if constexpr (!std::is_same_v<CONNECTION, TCPConnection>) {
            command_check(tcp, 200, "PBSZ 0\r\n");
            command_check(tcp, 200, "PROT P\r\n");
        }

        // change to binary mode
        command_check(tcp, 200, "TYPE I\r\n");

        // get size
        line = command_ret(tcp, "SIZE ", req.object(), "\r\n");
        response = ftp_ret_code(line);
        if (response == 213) {
            len = ftp_size(line);
            log_dbg("File has a size of ", len, " bytes.");
        }

        // PASV/EPSV
        line = command_ret(tcp, "PASV\r\n");
        response = ftp_ret_code(line);

        if (response == 227) {
            pasv_port = ftp_pasv_port(line);
        } else if (response == 501) {
            // hmz, PASV might not be supported -> trying EPSV
            line = command_ret(tcp, "EPSV\r\n");
            response = ftp_ret_code(line);
            check_response(229, response);
            pasv_port = ftp_epsv_port(line);
        } else {
            EXCEPTION("FTP server doesn't support PASV nor EPSV. Giving up.");
        }

        log_dbg("PASV p0rt is ", pasv_port);

        // set start offset
        if (req.start_offset() > 0) {
            log_dbg("Continuing file download @ ", req.start_offset(), " bytes");
            command_check(tcp, 350, "REST ", req.start_offset(), "\r\n");
            mode |= std::ios_base::app;
        }

        // issue get file command
        log_dbg("COMMAND: ", "RETR ", req.object(), "\r\n");
        tcp << "RETR " << req.object() << "\r\n";

        // connect to ftp data
        tcp_pasv.connect(req.host(), pasv_port);

        // check RETR response
        line = read_response(tcp);
        log_dbg("RESPONSE: ", line);
        response = ftp_ret_code(line);
        check_response({ 150, 125 }, response);

        // fetch it and save to file
        std::ofstream ofs(req.out_file_name(), mode);
        if (ofs.fail())
            EXCEPTION("Failed to open file: ", req.out_file_name());
        if (config->show_pg())
            tcp_pasv.read_until_eof_with_pg_to_fstream(ofs, req.start_offset(), len);
        else
            tcp_pasv.read_until_eof_to_fstream(ofs);
        tcp_pasv.close();

        // done
        line = read_response(tcp);
        log_dbg("RESPONSE: ", line);
        response = ftp_ret_code(line);
        check_response(226, response);
        command_check(tcp, 221, "QUIT\r\n");
    }

private:
    constexpr auto get_port() const noexcept
    {
        if constexpr (std::is_same_v<CONNECTION, TCPConnection>)
            return "ftp";
        else
            return "ftps";
    }

    std::size_t ftp_size(const std::string& line) const
    {
        std::regex pattern("\\d+\\s*(\\d+)\\r\\n");
        std::smatch match;

        if (std::regex_match(line, match, pattern))
            return Utils::str2to<std::size_t>(match.str(1));

        EXCEPTION("Failed to parse size of requested file.");
    }

    std::uint16_t ftp_pasv_port(const std::string& line) const
    {
        std::regex pattern("\\d+[\\w ]+\\(\\d+,\\d+,\\d+,\\d+,(\\d+),(\\d+)\\).*\\r\\n");
        std::smatch match;

        if (std::regex_match(line, match, pattern))
            return Utils::str2to<std::uint16_t>(match.str(1)) * 256 +
                Utils::str2to<std::uint16_t>(match.str(2));

        EXCEPTION("Failed to parse PASV port.");
    }

    std::uint16_t ftp_epsv_port(const std::string& line) const
    {
        std::regex pattern(".*\\(\\|\\|\\|(\\d+)\\|\\).*\\r\\n");
        std::smatch match;

        if (std::regex_match(line, match, pattern))
            return Utils::str2to<std::uint16_t>(match.str(1));

        EXCEPTION("Failed to parse EPSV port.");
    }

    int ftp_ret_code(const std::string& response) const
    {
        std::regex pattern("(\\d{3})\\s+(.*)\\r\\n");
        std::smatch match;

        if (std::regex_match(response, match, pattern))
            return Utils::str2to<int>(match.str(1));

        EXCEPTION("Received garbage from FTP server.");
    }

    void check_response(int expected_response, int real_response) const
    {
        if (real_response == expected_response)
            return;

        EXCEPTION("Received unexpected response code from FTP server ", real_response,
                  " while ", expected_response, " was expected.");
    }

    void check_response(const std::vector<int>& expected_responses,
                        int real_response) const
    {
        for (auto&& response: expected_responses)
            if (response == real_response)
                return;

        std::stringstream ss;
        ss << "{ ";
        auto i = 0u;
        for (auto&& response: expected_responses) {
            ss << response;
            if (i++ != (expected_responses.size() - 1))
                ss << ", ";
        }
        ss << " }";

        EXCEPTION("Received unexpected response code from FTP server ", real_response,
                  " while ", ss.str(), " was expected.");
    }

    bool is_reponse(const std::string& line) const
    {
        std::regex pattern("\\d{3}\\s+(.*)\\r\\n");
        std::smatch match;

        return std::regex_match(line, match, pattern);
    }

    std::string read_response(const CONNECTION& tcp) const
    {
        while (42) {
            auto line = tcp.read_ln();
            if (is_reponse(line))
                return line;
        }
    }

    template<typename... Args>
    [[nodiscard]]
    auto command_ret(CONNECTION& tcp, Args&&... args) const
    {
        std::stringstream ss;

        (ss << ... << std::forward<Args>(args));
        log_dbg("COMMAND: ", ss.str());
        tcp << ss.str();
        auto line = read_response(tcp);
        log_dbg("RESPONSE: ", line);

        return line;
    }

    template<typename... Args>
    void command_check(CONNECTION& tcp, int expected_response, Args&&... args) const
    {
        auto response = command_ret_code(tcp, std::forward<Args>(args)...);
        check_response(expected_response, response);
    }

    template<typename... Args>
    [[nodiscard]]
    auto command_ret_code(CONNECTION& tcp, Args&&... args) const
    {
        auto line = command_ret(tcp, std::forward<Args>(args)...);
        return ftp_ret_code(line);
    }
};

#endif /* _FTP_H_ */
