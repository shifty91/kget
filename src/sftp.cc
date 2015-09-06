#include <iomanip>
#include <array>
#include <iostream>

#include "tcp_connection.h"
#include "logger.h"
#include "utils.h"
#include "progress_bar.h"

#include "sftp.h"

KeyPairVector SFTPMethod::find_user_keys() const
{
    KeyPairVector keys;
    keys.reserve(2);

    // search for ~/.ssh/id_rsa or ~/.ssh/id_dsa
    std::string home = Utils::get_home();
    if (home == "") {
        log_dbg("Home of user not found.");
        return { };
    }

    std::array<std::string, 2> publickeys;
    publickeys[0] = home;
    publickeys[0] += "/";
    publickeys[0] += ".ssh/id_rsa.pub";
    publickeys[1] = home;
    publickeys[1] += "/";
    publickeys[1] += ".ssh/id_dsa.pub";

    std::array<std::string, 2> privatekeys;
    privatekeys[0] = home;
    privatekeys[0] += "/";
    privatekeys[0] += ".ssh/id_rsa";
    privatekeys[1] = home;
    privatekeys[1] += "/";
    privatekeys[1] += ".ssh/id_dsa";

    auto len = publickeys.size();
    for (decltype(len) i = 0; i < len; ++i) {
        if (Utils::file_exists(publickeys[i]) &&
            Utils::file_exists(privatekeys[i]))
            keys.emplace_back(publickeys[i], privatekeys[i]);
    }

    return keys;
}

void SFTPMethod::print_fingerprint(const std::string& fingerprint) const
{
    std::stringstream ss;
    ss << "Fingerprint: ";
    ss << std::hex << std::showbase << std::internal << std::setfill('0');
    for (int i = 0; i < 20; ++i)
        ss << std::setw(2) << static_cast<int>(static_cast<unsigned char>(fingerprint[i])) << " ";
    log_info(ss.str());
}

void SFTPMethod::publickey_auth(LIBSSH2_SESSION *session, const std::string& user) const
{
    auto keys = find_user_keys();
    auto len = keys.size();
    bool authenticated = false;

    // FIXME: Use SSH-Agent, if key is protected with passphrase
    for (decltype(len) i = 0; i < len; ++i) {
        int rc = libssh2_userauth_publickey_fromfile(session, user.c_str(), keys[i].first.c_str(),
                                                     keys[i].second.c_str(), "");
        if (rc == 0) {
            authenticated = true;
            break;
        } else if (rc == LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED) {
            // we need a passphrase
            std::string passphrase = Utils::user_input_pw("Enter passphrase");
            int rc = libssh2_userauth_publickey_fromfile(session, user.c_str(), keys[i].first.c_str(),
                                                         keys[i].second.c_str(), passphrase.c_str());
            if (rc != 0)
                EXCEPTION("Wrong passphrase!");
            authenticated = true;
        }
    }
    if (!authenticated)
        EXCEPTION("Public key authentication failed.");
}

void SFTPMethod::get(const std::string& fileToSave, const std::string& user,
                     const std::string& pw) const
{
    TCPConnection tcp;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    LIBSSH2_SFTP_ATTRIBUTES file_attr;
    std::string fingerprint;
    std::string userauthlist;
    std::string object(m_object);
    int sock;

    tcp.connect(m_host, "ssh");
    sock = tcp.socket();

    // FIXME: This code is not exception safe
    if (libssh2_init(0))
        EXCEPTION("libssh2_init() failed.");

    session = libssh2_session_init();
    if (!session)
        EXCEPTION("libssh2_session_init() failed.");

    libssh2_session_set_blocking(session, 1);

    if (libssh2_session_handshake(session, sock))
        EXCEPTION("libssh2_session_handshake() failed.");

    fingerprint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
    print_fingerprint(fingerprint);

    userauthlist = libssh2_userauth_list(session, user.c_str(), user.size());

    // password authentication
    if (pw != "") {
        std::size_t found = userauthlist.find("password");
        if (found == std::string::npos)
            EXCEPTION("Username and password specified, but password authenticate is not supported.");
        if (libssh2_userauth_password(session, user.c_str(), pw.c_str()))
            EXCEPTION("Authentication failed!");
    } else { // public key
        auto found = userauthlist.find("publickey");
        if (found == std::string::npos)
            EXCEPTION("Publickey authentication is not supported.");
        publickey_auth(session, user);
    }

    // start sftp
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session)
        EXCEPTION("libssh2_sftp_init() failed.");

    if (m_object[0] != '/') {
        std::stringstream ss;
        ss << "/" << m_object;
        object = ss.str();
    }

    // stat file
    if (libssh2_sftp_stat(sftp_session, object.c_str(), &file_attr))
        EXCEPTION("libssh2_sftp_stat() failed.");
    auto len = file_attr.filesize;

    sftp_handle = libssh2_sftp_open(sftp_session, object.c_str(), LIBSSH2_FXF_READ, 0);
    if (!sftp_handle)
        EXCEPTION("libssh2_sftp_open() failed.");

    // get and save file
    ProgressBar pg(len);
    std::ofstream ofs;
    ofs.open(fileToSave);
    if (ofs.fail())
        EXCEPTION("Failed to open file " << fileToSave);

    while (42) {
        char buffer[4096];
        int read = libssh2_sftp_read(sftp_handle, buffer, sizeof(buffer));
        if (read < 0)
            EXCEPTION("libssh2_sftp_read() failed.");
        if (read == 0)
            break;
        ofs.write(buffer, read);
        pg.update(read);
    }

    // shutdown libssh2
    libssh2_sftp_close(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

    libssh2_session_disconnect(session, "Shutdown");
    libssh2_session_free(session);

    libssh2_exit();
}
