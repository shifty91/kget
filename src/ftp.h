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

#include <cstdint>

#include "method.h"
#include "tcp_connection.h"

class FTPMethod : public Method
{
public:
    FTPMethod() :
        Method()
    {}

    virtual ~FTPMethod()
    {}

    virtual void get(const Request& req) const override;

private:
    int ftp_ret_code(const std::string& response) const;
    void check_response(int expected_response, int real_response) const;
    std::size_t ftp_size(const std::string& line) const;
    std::uint16_t ftp_pasv_port(const std::string& line) const;
    std::uint16_t ftp_epsv_port(const std::string& line) const;
    std::string read_response(const TCPConnection& tcp) const;
    bool is_reponse(const std::string& line) const;

    template<typename... Args>
    [[nodiscard]]
    auto command_ret(TCPConnection& tcp, Args&&... args) const
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
    void command_check(TCPConnection& tcp, int expected_response, Args&&... args) const
    {
        auto response = command_ret_code(tcp, std::forward<Args>(args)...);
        check_response(expected_response, response);
    }

    template<typename... Args>
    [[nodiscard]]
    auto command_ret_code(TCPConnection& tcp, Args&&... args) const
    {
        auto line = command_ret(tcp, std::forward<Args>(args)...);
        return ftp_ret_code(line);
    }
};

#endif /* _FTP_H_ */
