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

#include <cctype>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <fstream>
#include <cstring>

#include "ftp.h"
#include "logger.h"
#include "config.h"

#define CHECK_RESPONSE(tcp, response)                       \
    do {                                                    \
        check_response(tcp, response, __FILE__, __LINE__);  \
    } while (0)

void FTPMethod::get(const Request& req) const
{
    TCPConnection tcp, tcp_pasv;
    int response, pasv_port;
    std::size_t len = 0;
    Config *config = Config::instance();

    tcp.connect(req.host(), "ftp");
    CHECK_RESPONSE(tcp, 220);

    if (req.user() == "")
        tcp << "USER anonymous\r\n";
    else
        tcp << "USER " << req.user() << "\r\n";
    CHECK_RESPONSE(tcp, 331);
    if (req.pw() == "")
        tcp << "PASS asdf\r\n";
    else
        tcp << "PASS " << req.pw() << "\r\n";
    CHECK_RESPONSE(tcp, 230);

    log_dbg("Logged into FTP server at " << req.host());

    // change to binary mode
    tcp << "TYPE I\r\n";
    CHECK_RESPONSE(tcp, 200);

    // get size
    tcp << "SIZE " << req.object() << "\r\n";
    auto line = tcp.read_ln();
    response = ftp_ret_code(line);
    if (response == 213) {
        len = ftp_size(line);
        log_dbg("File has a size of " << len << " bytes.");
    }

    // PASV/EPSV
    tcp << "PASV\r\n";
    line = tcp.read_ln();
    response = ftp_ret_code(line);

    if (response == 227) {
        pasv_port = ftp_pasv_port(line);
    } else if (response == 501) {
        // hmz, PASV might not be supported -> trying EPSV
        tcp << "EPSV\r\n";
        line = tcp.read_ln();
        CHECK_RESPONSE(line, 229);
        pasv_port = ftp_epsv_port(line);
    } else {
        EXCEPTION("FTP server doesn't support PASV nor EPSV. Giving up.");
    }

    if (pasv_port < 0)
        EXCEPTION("Failed to parse PASV port " << pasv_port);

    log_dbg("PASV p0rt is " << pasv_port);

    // connect to ftp data
    tcp_pasv.connect(req.host(), pasv_port);

    // get file
    tcp << "RETR " << req.object() << "\r\n";
    CHECK_RESPONSE(tcp, 150);

    // fetch it and save to file
    std::ofstream ofs(req.out_file_name());
    if (ofs.fail())
        EXCEPTION("Failed to open file: " << req.out_file_name());
    if (config->show_pg())
        tcp_pasv.read_until_eof_with_pg_to_fstream(ofs, len);
    else
        tcp_pasv.read_until_eof_to_fstream(ofs);
    tcp_pasv.close();

    // done
    CHECK_RESPONSE(tcp, 226);
    tcp << "QUIT\r\n";
    CHECK_RESPONSE(tcp, 221);
}

std::size_t FTPMethod::ftp_size(const std::string& line) const
{
    std::regex pattern("\\d+\\s*(\\d+)\\r\\n");
    std::smatch match;

    if (std::regex_match(line, match, pattern)) {
        std::string size(match[1]);
        return std::atoll(size.c_str());
    }

    EXCEPTION("Failed to parse size of requested file.");
}

int FTPMethod::ftp_pasv_port(const std::string& line) const
{
    std::regex pattern("\\d+[\\w ]+\\(\\d+,\\d+,\\d+,\\d+,(\\d+),(\\d+)\\).*\\r\\n");
    std::smatch match;

    if (std::regex_match(line, match, pattern)) {
        std::string p1(match[1]), p2(match[2]);
        return std::atoi(p1.c_str()) * 256 + std::atoi(p2.c_str());
    }

    EXCEPTION("Failed to parse PASV port.");
}

int FTPMethod::ftp_epsv_port(const std::string& line) const
{
    // grr, stupid c++ regex, using c style parsing instead
    for (decltype(line.size()) i = 0; i < line.size(); ++i)
        if (!strncmp(line.c_str() + i, "|||", 3))
            return std::atoi(line.c_str() + i + 3);

    EXCEPTION("Failed to parse EPSV port.");
}

int FTPMethod::ftp_ret_code(const std::string& response) const
{
    char a, b, c;

    if (response.size() < 3)
        EXCEPTION("Received garbage from FTP server.");

    a = response[0];
    b = response[1];
    c = response[2];

    if (!std::isdigit(a) || !std::isdigit(b) || !std::isdigit(c))
        EXCEPTION("Received garbage from FTP server.");

    return (a-'0')*100 + (b-'0')*10 + (c-'0');
}

void FTPMethod::check_response(const TCPConnection& tcp, int expected_response,
                               const std::string& file, int line) const
{
    check_response(tcp.read_ln(), expected_response, file, line);
}

void FTPMethod::check_response(const std::string& line, int expected_response,
                               const std::string& file, int src_line) const
{
    auto response = ftp_ret_code(line);
    auto config   = Config::instance();

    if (response == expected_response)
        return;

    if (unlikely(config->debug()))
        EXCEPTION("Received unexpected response code from FTP server " << response
                  << " while " << expected_response << " was expected.\n"
                  << "  Origin of failure: " << file << ":" << src_line);
    else
        EXCEPTION("Received unexpected response code from FTP server " << response
                  << " while " << expected_response << " was expected.");
}
