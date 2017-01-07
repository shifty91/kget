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

#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <string>
#include <sstream>
#include <fstream>

#include "logger.h"

#define TCP_WRITE(tcp, msg)                     \
    do {                                        \
        std::stringstream ss;                   \
        ss << msg;                              \
        (tcp).write(ss.str());                  \
    } while (0)

class Connection
{
public:
    Connection() :
        m_sock{-1}, m_connected{false}
    {}

    virtual ~Connection()
    {}

    Connection(const Connection& other) = delete;
    Connection(const Connection&& other) = delete;

    Connection& operator=(const Connection& other) = delete;
    Connection& operator=(const Connection&& other) = delete;

    inline int socket() const
    {
        if (!m_connected)
            EXCEPTION("Not connected!");
        return m_sock;
    }

    virtual void connect(const std::string& host, const std::string& service) = 0;

    virtual void connect(const std::string& host, int port) = 0;

    virtual void close() = 0;

    virtual void write(const std::string& toWrite) const = 0;

    virtual std::string read(std::size_t numBytes) const = 0;

    virtual std::string read_until_eof(std::size_t fileSize = 0) const = 0;

    virtual std::string read_until_eof_with_pg(std::size_t fileSize) const = 0;

    virtual void read_until_eof_to_fstream(std::ofstream& ofs) const = 0;

    virtual void read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t fileSize) const = 0;

    virtual std::string read_ln() const = 0;

protected:
    // Buffer size for read(2) used in TCP connections
    static const std::size_t BUFFER_SIZE = 4096;

    int m_sock;
    bool m_connected;
};

#endif /* _CONNECTION_H_ */
