#ifndef _SSL_CONTEXT_H_
#define _SSL_CONTEXT_H_

#include <openssl/ssl.h>

#include "ssl/ssl_utilities.h"

/**
 * RAII wrapper for SSL_CTX.
 */
class SSLContext
{
private:
    SSL_CTX *m_ssl_context;

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

    inline ~SSLContext()
    {
        if (m_ssl_context != nullptr)
            SSL_CTX_free(m_ssl_context);
    }

    SSLContext(const SSLContext& other) = delete;
    SSLContext(const SSLContext&& other) = delete;

    SSLContext& operator=(const SSLContext& other) = delete;
    SSLContext& operator=(const SSLContext&& other) = delete;

    inline SSL_CTX *context() const
    {
        return m_ssl_context;
    }

    inline void set_options(long options)
    {
        (void)SSL_CTX_set_options(m_ssl_context, options);
    }
};

#endif /* _SSL_CONTEXT_H_ */
