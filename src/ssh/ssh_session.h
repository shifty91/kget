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

#ifndef _SSH_SESSION_H_
#define _SSH_SESSION_H_

#include "get_config.h"

#ifdef HAVE_LIBSSH

#include <string>
#include <libssh2.h>

#include "ssh/ssh_utilities.h"
#include "logger.h"

class SSHSession
{
public:
    inline SSHSession() :
        m_connected{false}
    {
        m_session = libssh2_session_init();
        if (m_session == nullptr)
            SSH_EXCEPTION(m_session, "libssh2_session_init() failed");
    }

    inline ~SSHSession()
    {
        if (m_connected && m_session != nullptr)
            libssh2_session_disconnect(m_session, "Shutdown");
        if (m_session != nullptr)
            libssh2_session_free(m_session);
    }

    SSHSession(const SSHSession& other) = delete;
    SSHSession(SSHSession&& other) = delete;
    SSHSession& operator=(const SSHSession& other) = delete;
    SSHSession& operator=(SSHSession&& other) = delete;

    inline LIBSSH2_SESSION *session() noexcept
    {
        return m_session;
    }

    inline void set_blocking(bool value) noexcept
    {
        libssh2_session_set_blocking(m_session, value);
    }

    inline void handshake(int socket)
    {
        if (libssh2_session_handshake(m_session, socket))
            SSH_EXCEPTION(m_session, "libssh2_session_handshake() failed");
        m_connected = true;
    }

    inline std::string hostkey(int hash_type) noexcept
    {
        return libssh2_hostkey_hash(m_session, hash_type);
    }

    inline std::string auth_list(const std::string& user) noexcept
    {
        return libssh2_userauth_list(m_session, user.c_str(), user.size());
    }

    inline bool auth_pw(const std::string& user, const std::string& pw) noexcept
    {
        auto rc = libssh2_userauth_password(m_session, user.c_str(), pw.c_str());

        switch (rc) {
        case 0:
            return true;
        case LIBSSH2_ERROR_PASSWORD_EXPIRED:
            log_dbg("Password expired.");
            break;
        case LIBSSH2_ERROR_AUTHENTICATION_FAILED:
            log_dbg("Password authentication failed.");
            break;
        }
        return false;
    }

    inline auto auth_key(const std::string& user, const std::string& publickey,
                         const std::string& privatekey, const std::string& passphrase) noexcept
    {
        return libssh2_userauth_publickey_fromfile(
            m_session, user.c_str(), publickey.c_str(), privatekey.c_str(), passphrase.c_str());
    }

private:
    LIBSSH2_SESSION *m_session;
    bool m_connected;
};

#endif

#endif /* _SSH_SESSION_H_ */
