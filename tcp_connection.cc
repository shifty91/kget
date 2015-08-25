#include "tcp_connection.h"

#include "logger.h"
#include "progress_bar.h"

#include <cstring>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

void TCPConnection::connect(const std::string& host, int port)
{
    connect(host, std::to_string(port));
}

void TCPConnection::connect(const std::string& host, const std::string& service)
{
    int res;
    int sock;
    struct addrinfo *sa_head, *sa, hints;

    close();

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
        sock = socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
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
    m_connected = true;
}

void TCPConnection::write(const std::string& toWrite) const
{
    ssize_t written = 0;
    auto len = toWrite.size();
    auto start = toWrite.c_str();

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(len)>(written) < len) {
        auto tmp = ::write(m_sock, start + written, len - written);
        if (tmp == -1)
            EXCEPTION("write() to socket failed: " << strerror(errno));
        written += tmp;
    }
}

std::string TCPConnection::read(std::size_t numBytes) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(numBytes)>(read) < numBytes) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPConnection::read_until_eof(std::size_t fileSize) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (fileSize > 0)
        result.reserve(fileSize);
    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPConnection::read_until_eof_with_pg(std::size_t fileSize) const
{
    ProgressBar pg(fileSize);
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (fileSize > 0)
        result.reserve(fileSize);
    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
        pg.update(tmp);
    }

    return result;
}

void TCPConnection::read_until_eof_to_fstream(std::ofstream& ofs) const
{
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
    }
}

void TCPConnection::read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t fileSize) const
{
    ProgressBar pg(fileSize);
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
        pg.update(tmp);
    }
}

std::string TCPConnection::read_ln() const
{
    std::string result;
    char buffer[1];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    // FIXME: This is kind of slow...
    while (42) {
        auto tmp = ::read(m_sock, buffer, 1);
        if (tmp == -1)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        result.insert(read, buffer, tmp);
        read += tmp;

        if (buffer[0] == '\n')
            break;
    }

    return result;
}
