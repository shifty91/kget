#ifndef _SSL_HANDLE_H_
#define _SSL_HANDLE_H_

#include <openssl/ssl.h>

#include "ssl/ssl_context.h"
#include "ssl/ssl_utilities.h"

/**
 * RAII wrapper for SSL.
 */
class SSLHandle
{
private:
    SSL *m_ssl_handle;
    bool m_connected;

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

    inline void set_fd(int socket)
    {
        if (!SSL_set_fd(m_ssl_handle, socket))
            GET_SSL_EXCEPTION("SSL_set_fd() failed.");
    }

    inline void set_verify(int mode, int (*verify_callback)(int, X509_STORE_CTX *))
    {
        SSL_set_verify(m_ssl_handle, mode, verify_callback);
    }

    inline int get_verify_result()
    {
        return SSL_get_verify_result(m_ssl_handle);
    }

    inline SSL *handle() const
    {
        return m_ssl_handle;
    }
};

#endif /* _SSL_HANDLE_H_ */
