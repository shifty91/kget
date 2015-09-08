#ifndef _SFTP_HANDLE_H_
#define _SFTP_HANDLE_H_

#include <string>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "ssh/sftp_session.h"
#include "logger.h"

class SFTPHandle
{
private:
    LIBSSH2_SFTP_HANDLE *m_handle;

public:
    inline SFTPHandle() :
        m_handle{nullptr}
    {}

    inline
    SFTPHandle(LIBSSH2_SFTP *sftp_session, const std::string& object,
               unsigned long flags = LIBSSH2_FXF_READ, long mode = 0)
    {
        m_handle = libssh2_sftp_open(sftp_session, object.c_str(), flags, mode);
        if (m_handle == nullptr)
            EXCEPTION("libssh2_sftp_open() failed.");
    }

    inline ~SFTPHandle()
    {
        if (m_handle != nullptr)
            libssh2_sftp_close(m_handle);
    }

    inline LIBSSH2_SFTP_HANDLE *handle()
    {
        return m_handle;
    }

    inline auto read(char *buffer, std::size_t len)
    {
        return libssh2_sftp_read(m_handle, buffer, len);
    }

    inline auto write(const char *buffer, std::size_t len)
    {
        return libssh2_sftp_write(m_handle, buffer, len);
    }
};

#endif /* _SFTP_HANDLE_H_ */
