/*
 * Copyright (C) 2015-2020 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <string>
#include <sstream>

#include <unistd.h>

#include "connection.h"

/**
 * This class represents a TCP connection. Errorhandling is done via exceptions.
 * Uses the BSD/Linux socket API. For Windows a another instance of this class
 * should be implemented using WinSock.
 */
class TCPConnection : public Connection
{
public:
    TCPConnection() :
        Connection()
    {}

    ~TCPConnection()
    {
        close();
    }

    TCPConnection(const TCPConnection& other) = delete;
    TCPConnection(TCPConnection&& other) = delete;

    TCPConnection& operator=(const TCPConnection& other) = delete;
    TCPConnection& operator=(TCPConnection&& other) = delete;

    inline int socket() const
    {
        if (!m_connected)
            EXCEPTION("Not connected!");
        return m_sock;
    }

    virtual void connect(const std::string& host, const std::string& service) override;

    virtual void connect(const std::string& host, int port) override;

    inline virtual void close() override
    {
        if (m_connected)
            ::close(m_sock);
        m_connected = false;
    }

    virtual void write(const std::string& to_write) const override;

    virtual std::string read(std::size_t num_bytes) const override;

    virtual std::string read_until_eof(std::size_t file_size = 0) const override;

    virtual std::string read_until_eof_with_pg(std::size_t file_size) const override;

    virtual void read_until_eof_to_fstream(std::ofstream& ofs) const override;

    virtual void read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t start_offset, std::size_t file_size) const override;

    virtual std::string read_ln() const override;
};

#endif /* _TCP_CONNECTION_H_ */
