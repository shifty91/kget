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

#ifndef _SFTP_SESSION_H_
#define _SFTP_SESSION_H_

#include "get_config.h"

#ifdef HAVE_LIBSSH

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "ssh/ssh_session.h"
#include "ssh/sftp_handle.h"
#include "ssh/ssh_utilities.h"

#include "logger.h"

class SFTPSession
{
public:
    inline SFTPSession() :
        m_session{nullptr}
    {}

    inline explicit SFTPSession(SSHSession& ssh)
    {
        new_session(ssh);
    }

    inline ~SFTPSession()
    {
        if (m_session != nullptr)
            libssh2_sftp_shutdown(m_session);
    }

    SFTPSession(const SFTPSession& other) = delete;
    SFTPSession(const SFTPSession&& other) = delete;
    SFTPSession& operator=(const SFTPSession& other) = delete;
    SFTPSession& operator=(const SFTPSession&& other) = delete;

    inline LIBSSH2_SFTP *session() noexcept
    {
        return m_session;
    }

    inline void new_session(SSHSession& ssh)
    {
        m_session = libssh2_sftp_init(ssh.session());
        if (m_session == nullptr)
            SFTP_EXCEPTION(m_session, "libssh2_sftp_init() failed");
    }

    inline auto stat(const std::string& object)
    {
        LIBSSH2_SFTP_ATTRIBUTES attrs;
        if (libssh2_sftp_stat(m_session, object.c_str(), &attrs))
            SFTP_EXCEPTION(m_session, "libssh2_sftp_stat() failed");
        return attrs;
    }

    inline SFTPHandle open(const std::string& object,
                           unsigned long flags = LIBSSH2_FXF_READ,
                           long mode = 0) noexcept
    {
        return SFTPHandle(m_session, object, flags, mode);
    }

private:
    LIBSSH2_SFTP *m_session;
};

#endif

#endif /* _SFTP_SESSION_H_ */
