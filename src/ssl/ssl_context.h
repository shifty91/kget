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

#ifndef _SSL_CONTEXT_H_
#define _SSL_CONTEXT_H_

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include <openssl/ssl.h>

#include "ssl/ssl_utilities.h"

/**
 * RAII wrapper for SSL_CTX.
 */
class SSLContext
{
public:
    inline SSLContext() :
        m_ssl_context{nullptr}
    {}

    inline explicit SSLContext(const SSL_METHOD *method) :
        m_ssl_context{nullptr}
    {
        context_new(method);
    }

    inline void context_new(const SSL_METHOD *method)
    {
        m_ssl_context = SSL_CTX_new(method);
        if (m_ssl_context == nullptr)
            GET_SSL_EXCEPTION("SSL_CTX_new() failed.");
    }

    inline ~SSLContext() noexcept
    {
        if (m_ssl_context != nullptr)
            SSL_CTX_free(m_ssl_context);
    }

    SSLContext(const SSLContext& other) = delete;
    SSLContext(SSLContext&& other) = delete;

    SSLContext& operator=(const SSLContext& other) = delete;
    SSLContext& operator=(SSLContext&& other) = delete;

    inline SSL_CTX *context() const noexcept
    {
        return m_ssl_context;
    }

    inline void set_options(long options) noexcept
    {
        (void)SSL_CTX_set_options(m_ssl_context, options);
    }

    inline void set_cipher_list(const std::string& cipher_list)
    {
        if (SSL_CTX_set_cipher_list(m_ssl_context, cipher_list.c_str()) != 1)
            EXCEPTION("SSL_CTX_set_cipher_list() failed.");
    }

    inline void set_default_verify_paths() noexcept
    {
        SSL_CTX_set_default_verify_paths(m_ssl_context);
    }

private:
    SSL_CTX *m_ssl_context;
};

#endif

#endif /* _SSL_CONTEXT_H_ */
