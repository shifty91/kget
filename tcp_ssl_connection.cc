#include "tcp_ssl_connection.h"

#include "logger.h"
#include "progress_bar.h"

#include <cstring>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

void TCPSSLConnection::init_ssl()
{
    SSL_load_error_strings();
    SSL_library_init();

    m_ssl_context.context_new(SSLv23_client_method());
    m_ssl_context.set_options(SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    m_ssl_handle.ssl_new(m_ssl_context);

    m_ssl_handle.set_fd(m_sock);
    m_ssl_handle.connect();
}

void TCPSSLConnection::connect(const std::string& host, int port)
{
    connect(host, std::to_string(port));
}

void TCPSSLConnection::connect(const std::string& host, const std::string& service)
{
    close();
    tcp_connect(host, service);
    init_ssl();
    m_connected = true;
}

void TCPSSLConnection::tcp_connect(const std::string& host, const std::string& service)
{
    int res;
    int sock;
    struct addrinfo *sa_head, *sa, hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    /* use IPv6 or IPv4 */
    hints.ai_family   = PF_UNSPEC;
    hints.ai_flags    = AI_ADDRCONFIG;

    res = getaddrinfo(host.c_str(), service.c_str(), &hints, &sa_head);
    if (res)
        EXCEPTION("getaddrinfo() for host " << host << " failed: " << gai_strerror(res));

    // try to connect to some record...
    for (sa = sa_head; sa != NULL; sa = sa->ai_next) {
        sock = ::socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
        if (sock < 0) {
            log_err("socket() failed: " << strerror(errno));
            goto out;
        }

        if (!::connect(sock, sa->ai_addr, sa->ai_addrlen))
            break;

        ::close(sock);
    }

    if (!sa)
        EXCEPTION("connect() for host " << host << " on service " << service <<
                  " failed: " << strerror(errno));

out:
    freeaddrinfo(sa_head);

    m_sock = sock;
}

void TCPSSLConnection::write(const std::string& toWrite) const
{
    int written = 0;
    auto len = toWrite.size();
    auto start = toWrite.c_str();

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(len)>(written) < len) {
        int tmp = SSL_write(m_ssl_handle.handle(), start + written, len - written);
        if (tmp <= 0)
            EXCEPTION("SSL_write() failed: " << strerror(errno));
        written += tmp;
    }
}

std::string TCPSSLConnection::read(std::size_t numBytes) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(numBytes)>(read) < numBytes) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, sizeof(buffer));
        if (tmp <= 0)
            EXCEPTION("SSL_read() failed");
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPSSLConnection::read_until_eof(std::size_t fileSize) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (fileSize > 0)
        result.reserve(fileSize);
    while (42) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed.");
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPSSLConnection::read_until_eof_with_pg(std::size_t fileSize) const
{
    ProgressBar pg(fileSize);
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (fileSize > 0)
        result.reserve(fileSize);
    while (42) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("SSL_read() failed.");
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
        pg.update(tmp);
    }

    return result;
}

void TCPSSLConnection::read_until_eof_to_fstream(std::ofstream& ofs) const
{
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed.");
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
    }
}

void TCPSSLConnection::read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t fileSize) const
{
    ProgressBar pg(fileSize);
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed.");
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
        pg.update(tmp);
    }
}

std::string TCPSSLConnection::read_ln() const
{
    std::string result;
    char buffer[1];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    // FIXME: This is kind of slow
    while (42) {
        int tmp = SSL_read(m_ssl_handle.handle(), buffer, 1);
        if (tmp == -1)
            EXCEPTION("SSL_read() failed.");
        result.insert(read, buffer, tmp);
        read += tmp;

        if (buffer[0] == '\n')
            break;
    }

    return result;
}
