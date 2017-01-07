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

#ifndef _SSL_HANDLE_H_
#define _SSL_HANDLE_H_

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>

#include "ssl/ssl_context.h"
#include "ssl/ssl_utilities.h"

/**
 * RAII wrapper for SSL.
 */
class SSLHandle
{
public:
    inline SSLHandle() :
        m_ssl_handle{nullptr}, m_connected{false}
    {}

    inline explicit SSLHandle(const SSLContext& context) :
        m_connected{false}
    {
        ssl_new(context);
    }

    inline void ssl_new(const SSLContext& context)
    {
        m_ssl_handle = SSL_new(context.context());
        if (m_ssl_handle == nullptr)
            GET_SSL_EXCEPTION("SSL_new() failed.");
    }

    inline ~SSLHandle()
    {
        if (m_connected && m_ssl_handle != nullptr)
            SSL_shutdown(m_ssl_handle);
        if (m_ssl_handle != nullptr)
            SSL_free(m_ssl_handle);
    }

    SSLHandle(const SSLHandle& other) = delete;
    SSLHandle(const SSLHandle&& other) = delete;
    SSLHandle& operator=(const SSLHandle& other) = delete;
    SSLHandle& operator=(const SSLHandle&& other) = delete;

    inline void connect()
    {
        if (SSL_connect(m_ssl_handle) != 1)
            GET_SSL_EXCEPTION("SSL_connect() failed.");
        m_connected = true;
    }

    inline void set_fd(int socket) const
    {
        if (!SSL_set_fd(m_ssl_handle, socket))
            GET_SSL_EXCEPTION("SSL_set_fd() failed.");
    }

    inline void
    set_verify(int mode, int (*verify_callback)(int, X509_STORE_CTX *)) const noexcept
    {
        SSL_set_verify(m_ssl_handle, mode, verify_callback);
    }

    inline long get_verify_result() const noexcept
    {
        return SSL_get_verify_result(m_ssl_handle);
    }

    inline SSL *handle() const noexcept
    {
        return m_ssl_handle;
    }

    inline int read(void *buffer, int size) const noexcept
    {
        return SSL_read(m_ssl_handle, buffer, size);
    }

    inline int write(const void *buffer, int size) const noexcept
    {
        return SSL_write(m_ssl_handle, buffer, size);
    }

    inline auto get_error(int ret) const noexcept
    {
        return SSL_get_error(m_ssl_handle, ret);
    }

    inline std::string str_error(int ret) const
    {
        auto code = get_error(ret);
#define _(code) case code: return #code
        switch (code) {
            _(SSL_ERROR_NONE);
            _(SSL_ERROR_ZERO_RETURN);
            _(SSL_ERROR_WANT_READ);
            _(SSL_ERROR_WANT_WRITE);
            _(SSL_ERROR_WANT_CONNECT);
            _(SSL_ERROR_WANT_ACCEPT);
            _(SSL_ERROR_WANT_X509_LOOKUP);
            _(SSL_ERROR_SYSCALL);
            _(SSL_ERROR_SSL);
        }
#undef _
        return "Unknown SSL error ocurred";
    }

private:
    SSL *m_ssl_handle;
    bool m_connected;
};

#endif

#endif /* _SSL_HANDLE_H_ */
