#include "tcp_connection.h"

#include "logger.h"
#include "progress_bar.h"
#include "net_utils.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

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
    close();
    m_sock = NetUtils::tcp_connect(host.c_str(), service.c_str());
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
        auto len = std::min(sizeof(buffer), numBytes - static_cast<std::size_t>(read));
        auto tmp = ::read(m_sock, buffer, len);
        if (tmp < 0)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            EXCEPTION("read() encountered EOF");
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
        if (tmp < 0)
            EXCEPTION("read() to socket failed: " << strerror(errno));
        if (tmp == 0)
            EXCEPTION("read() in read_ln() encountered EOF");
        result.insert(read, buffer, tmp);
        read += tmp;

        if (buffer[0] == '\n')
            break;
    }

    return result;
}
