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

#ifndef _SFTP_HANDLE_H_
#define _SFTP_HANDLE_H_

#include "get_config.h"

#ifdef HAVE_LIBSSH

#include <string>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "ssh/sftp_session.h"
#include "ssh/ssh_utilities.h"
#include "logger.h"

class SFTPHandle
{
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
            SFTP_EXCEPTION(sftp_session, "libssh2_sftp_open() failed");
    }

    inline ~SFTPHandle()
    {
        if (m_handle != nullptr)
            libssh2_sftp_close(m_handle);
    }

    inline LIBSSH2_SFTP_HANDLE *handle() noexcept
    {
        return m_handle;
    }

    inline auto read(char *buffer, std::size_t len) noexcept
    {
        return libssh2_sftp_read(m_handle, buffer, len);
    }

    inline auto write(const char *buffer, std::size_t len) noexcept
    {
        return libssh2_sftp_write(m_handle, buffer, len);
    }

private:
    LIBSSH2_SFTP_HANDLE *m_handle;
};

#endif

#endif /* _SFTP_HANDLE_H_ */
