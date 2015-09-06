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
protected:
     // Buffer size for read(2) used in TCP connections
    static const int BUFFER_SIZE = 4096;
    int m_sock;
    bool m_connected;

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
};

#endif /* _CONNECTION_H_ */
