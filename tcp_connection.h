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
private:
    int m_sock;
    bool m_connected;

public:
    TCPConnection() :
        Connection()
    {}

    ~TCPConnection()
    {
        close();
    }

    TCPConnection(const TCPConnection& other) = delete;
    TCPConnection(const TCPConnection&& other) = delete;

    TCPConnection& operator=(const TCPConnection& other) = delete;
    TCPConnection& operator=(const TCPConnection&& other) = delete;

    virtual void connect(const std::string& host, const std::string& service) override;

    virtual void connect(const std::string& host, int port) override;

    inline virtual void close() override
    {
        if (m_connected)
            ::close(m_sock);
        m_connected = false;
    }

    virtual void write(const std::string& toWrite) const override;

    virtual std::string read(std::size_t numBytes) const override;

    virtual std::string read_until_eof(std::size_t fileSize = 0) const override;

    virtual std::string read_until_eof_with_pg(std::size_t fileSize) const override;

    virtual void read_until_eof_to_fstream(std::ofstream& ofs) const override;

    virtual void read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t fileSize) const override;

    virtual std::string read_ln() const override;
};

#endif /* _TCP_CONNECTION_H_ */
