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

#ifndef _FTP_H_
#define _FTP_H_

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
    void check_response(const TCPConnection& tcp, int expected_response,
                        const std::string& file = "", int line = 0) const;
    void check_response(const std::string& line, int expected_response,
                        const std::string& file = "", int src_line = 0) const;
    std::size_t ftp_size(const std::string& line) const;
    int ftp_pasv_port(const std::string& line) const;
    int ftp_epsv_port(const std::string& line) const;
    std::string read_response(const TCPConnection& tcp) const;
    bool is_reponse(const std::string& line) const;
};

#endif /* _FTP_H_ */
