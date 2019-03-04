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

#include <cctype>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <fstream>
#include <cstring>

#include "ftp.h"
#include "logger.h"
#include "config.h"

void FTPMethod::get(const Request& req) const
{
    TCPConnection tcp, tcp_pasv;
    std::string user_name, pass;
    int pasv_port;
    std::size_t len = 0;
    std::ios_base::openmode mode = std::ios_base::out;
    Config *config = Config::instance();

    tcp.connect(req.host(), "ftp");
    auto line = read_response(tcp);
    log_dbg("RESPONSE: ", line);
    auto response = ftp_ret_code(line);
    check_response(220, response);

    // user
    if (req.user() == "")
        user_name = "anonymous";
    else
        user_name = req.user();

    response = command_ret_code(tcp, "USER ", user_name, "\r\n");
    if (response == 230)
        goto logged_in;

    // password
    check_response(331, response);
    if (req.pw() == "")
        pass = "asdf";
    else
        pass = req.pw();
    command_check(tcp, 230, "PASS ", pass, "\r\n");

logged_in:
    log_dbg("Logged into FTP server at ", req.host());

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

    if (pasv_port < 0)
        EXCEPTION("Failed to parse PASV port ", pasv_port);

    log_dbg("PASV p0rt is ", pasv_port);

    // connect to ftp data
    tcp_pasv.connect(req.host(), pasv_port);

    // set start offset
    if (req.start_offset() > 0) {
        log_dbg("Continuing file download @ ", req.start_offset(), " bytes");
        response = command_ret_code(tcp, "REST ", req.start_offset(), "\r\n");
        check_response(350, response);
        mode |= std::ios_base::app;
    }

    // get file
    command_check(tcp, 150, "RETR ", req.object(), "\r\n");

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
    std::regex pattern(".*\\(\\|\\|\\|(\\d+)\\|\\).*\\r\\n");
    std::smatch match;

    if (std::regex_match(line, match, pattern)) {
        std::string p(match[1]);
        return std::atoi(p.c_str());
    }

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

void FTPMethod::check_response(int expected_response, int real_response) const
{
    if (real_response == expected_response)
        return;

    EXCEPTION("Received unexpected response code from FTP server ", real_response,
              " while ", expected_response, " was expected.");
}

bool FTPMethod::is_reponse(const std::string& line) const
{
    std::regex pattern("\\d{3}\\s+(.*)\\r\\n");
    std::smatch match;

    return std::regex_match(line, match, pattern);
}

std::string FTPMethod::read_response(const TCPConnection& tcp) const
{
    while (42) {
        auto line = tcp.read_ln();
        if (is_reponse(line))
            return line;
    }
}
