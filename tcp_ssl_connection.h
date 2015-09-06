#ifndef _TCP_SSL_CONNECTION_H_
#define _TCP_SSL_CONNECTION_H_

#include <string>
#include <sstream>

#include <unistd.h>

#include "connection.h"
#include "ssl/ssl_wrapper.h"

/**
 * This class represents a TCP connection using SSL over TLS.
 * Errorhandling is done via exceptions. Uses the BSD/Linux socket API.
 * For Windows a another instance of this class should be implemented
 * using WinSock.
 */
class TCPSSLConnection : public Connection
{
private:
    SSLHandle m_ssl_handle;
    SSLContext m_ssl_context;

    void init_ssl();
    void tcp_connect(const std::string& host, const std::string& service);

public:
    TCPSSLConnection() :
        Connection()
    {}

    ~TCPSSLConnection()
    {
        close();
    }

    TCPSSLConnection(const TCPSSLConnection& other) = delete;
    TCPSSLConnection(const TCPSSLConnection&& other) = delete;

    TCPSSLConnection& operator=(const TCPSSLConnection& other) = delete;
    TCPSSLConnection& operator=(const TCPSSLConnection&& other) = delete;

    virtual void connect(const std::string& host, const std::string& service) override;

    virtual void connect(const std::string& host, int port) override;

    virtual inline void close() override
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

#endif /* _TCP_SSL_CONNECTION_H_ */
