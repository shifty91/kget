#ifndef _SSH_SESSION_H_
#define _SSH_SESSION_H_

#include <string>
#include <libssh2.h>

#include "logger.h"

class SSHSession
{
private:
    LIBSSH2_SESSION *m_session;
    bool m_connected;

public:
    inline SSHSession() :
        m_connected{false}
    {
        m_session = libssh2_session_init();
        if (m_session == nullptr)
            EXCEPTION("libssh2_session_init() failed.");
    }

    inline ~SSHSession()
    {
        if (m_connected && m_session != nullptr)
            libssh2_session_disconnect(m_session, "Shutdown");
        if (m_session != nullptr)
            libssh2_session_free(m_session);
    }

    SSHSession(const SSHSession& other) = delete;
    SSHSession(const SSHSession&& other) = delete;
    SSHSession& operator=(const SSHSession& other) = delete;
    SSHSession& operator=(const SSHSession&& other) = delete;

    inline LIBSSH2_SESSION *session()
    {
        return m_session;
    }

    inline void set_blocking(bool value)
    {
        libssh2_session_set_blocking(m_session, value);
    }

    inline void handshake(int socket)
    {
        if (libssh2_session_handshake(m_session, socket))
            EXCEPTION("libssh2_session_handshake() failed.");
        m_connected = true;
    }

    inline std::string hostkey(int hash_type)
    {
        return libssh2_hostkey_hash(m_session, hash_type);
    }

    inline std::string auth_list(const std::string& user)
    {
        return libssh2_userauth_list(m_session, user.c_str(), user.size());
    }

    inline bool auth_pw(const std::string& user, const std::string& pw)
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
                         const std::string& privatekey, const std::string& passphrase)
    {
        return libssh2_userauth_publickey_fromfile(
            m_session, user.c_str(), publickey.c_str(), privatekey.c_str(), passphrase.c_str());
    }
};

#endif /* _SSH_SESSION_H_ */
