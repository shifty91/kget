/*
 * Copyright (C) 2015-2021 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include "tcp_ssl_connection.h"

#include "logger.h"
#include "progress_bar.h"
#include "config.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#include <openssl/x509v3.h>

SSLInit TCPSSLConnection::m_ssl_init;

void TCPSSLConnection::init_ssl(const std::string& host)
{
    m_ssl_ctx.context_new(SSLv23_client_method());
    if (!Config::instance()->use_sslv2())
        m_ssl_ctx.set_options(SSL_OP_NO_SSLv2);
    if (!Config::instance()->use_sslv3())
        m_ssl_ctx.set_options(SSL_OP_NO_SSLv3);

    m_ssl_ctx.set_cipher_list("HIGH:MEDIUM:!RC4:!SRP:!PSK:!MD5:!aNULL@STRENGTH");
    m_ssl_ctx.set_default_verify_paths();

    m_ssl.ssl_new(m_ssl_ctx);
    m_ssl.set_fd(m_sock);

    // force verification of server's certificate
    if (Config::instance()->verify_peer()) {
        X509_VERIFY_PARAM *param;
        param = SSL_get0_param(m_ssl.handle());

        X509_VERIFY_PARAM_set_hostflags(param, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
        X509_VERIFY_PARAM_set1_host(param, host.c_str(), 0);
        m_ssl.set_verify(SSL_VERIFY_PEER, nullptr);
    }
    m_ssl.connect();

    auto verified = m_ssl.get_verify_result();
    if (verified == X509_V_OK)
        log_dbg("Server's certificate verified.");
    else
        log_dbg("Server's certificate not verfified (result=", verified, ").");

    log_dbg("SSL connection uses '", m_ssl.get_cipher(), "' cipher.");
}

void TCPSSLConnection::connect(const std::string& host, int port)
{
    connect(host, std::to_string(port));
}

void TCPSSLConnection::connect(const std::string& host, const std::string& service)
{
    close();
    tcp_connect(host, service);
    init_ssl(host);
    m_connected = true;
}

void TCPSSLConnection::write(const std::string& to_write) const
{
    int written = 0;
    auto len = to_write.size();
    auto start = to_write.c_str();

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(len)>(written) < len) {
        auto tmp = m_ssl.write(start + written, len - written);
        if (tmp <= 0)
            EXCEPTION("SSL_write() failed: ", m_ssl.str_error(tmp));
        written += tmp;
    }
}

std::string TCPSSLConnection::read(std::size_t num_bytes) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(num_bytes)>(read) < num_bytes) {
        auto len = std::min(sizeof(buffer), num_bytes - static_cast<int>(read));
        auto tmp = m_ssl.read(buffer, len);
        if (tmp <= 0)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPSSLConnection::read_until_eof(std::size_t file_size) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (file_size > 0)
        result.reserve(file_size);
    while (42) {
        auto tmp = m_ssl.read(buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPSSLConnection::read_until_eof_with_pg(std::size_t file_size) const
{
    ProgressBar pg(file_size);
    std::string result;
    char buffer[BUFFER_SIZE];
    int read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (file_size > 0)
        result.reserve(file_size);
    while (42) {
        auto tmp = m_ssl.read(buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
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
        auto tmp = m_ssl.read(buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
    }
}

void TCPSSLConnection::read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t start_offset, std::size_t file_size) const
{
    ProgressBar pg(start_offset, file_size);
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        auto tmp = m_ssl.read(buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
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
        auto tmp = m_ssl.read(buffer, sizeof(buffer));
        if (tmp < 0)
            EXCEPTION("SSL_read() failed: ", m_ssl.str_error(tmp));
        if (tmp == 0)
            EXCEPTION("SSL_read() in read_ln() encountered EOF");
        result.insert(read, buffer, tmp);
        read += tmp;

        if (buffer[0] == '\n')
            break;
    }

    return result;
}

#endif
