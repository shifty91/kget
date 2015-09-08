#include <iomanip>
#include <array>
#include <iostream>

#include <libssh2.h>
#include <libssh2_sftp.h>

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

void SFTPMethod::publickey_auth(SSHSession& session, const std::string& user) const
{
    auto keys = find_user_keys();
    auto len = keys.size();
    bool authenticated = false;

    // FIXME: Use SSH-Agent, if key is protected with passphrase
    for (decltype(len) i = 0; i < len; ++i) {
        auto rc = session.auth_key(user, keys[i].first, keys[i].second, "");
        if (rc == 0) {
            authenticated = true;
            break;
        } else if (rc == LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED) {
            log_dbg("Publickey is unverified. Trying to get a passphrase.");
            // we need a passphrase
            std::string passphrase = Utils::user_input_pw("Enter passphrase");
            auto rc = session.auth_key(user, keys[i].first, keys[i].second, passphrase);
            if (rc != 0)
                EXCEPTION("Wrong passphrase!");
            authenticated = true;
        }
    }

    if (!authenticated)
        EXCEPTION("Publickey authentication failed.");
}

void SFTPMethod::get(const std::string& fileToSave, const std::string& user,
                     const std::string& pw) const
{
    TCPConnection tcp;
    SSHSession session;
    SFTPSession sftp_session;
    std::string fingerprint;
    std::string userauthlist;
    std::string object(m_object);
    int sock;

    tcp.connect(m_host, "ssh");
    sock = tcp.socket();

    if (libssh2_init(0))
        EXCEPTION("libssh2_init() failed.");

    session.set_blocking(true);
    session.handshake(sock);

    fingerprint = session.hostkey(LIBSSH2_HOSTKEY_HASH_SHA1);
    print_fingerprint(fingerprint);

    userauthlist = session.auth_list(user);

    // password authentication
    if (pw != "") {
        std::size_t found = userauthlist.find("password");
        if (found == std::string::npos)
            EXCEPTION("Username and password specified, but password authenticate is not supported.");
        if (!session.auth_pw(user, pw))
            EXCEPTION("Authentication failed!");
    } else { // public key
        auto found = userauthlist.find("publickey");
        if (found == std::string::npos)
            EXCEPTION("Publickey authentication is not supported.");
        publickey_auth(session, user);
    }

    // start sftp
    sftp_session.new_session(session);

    if (m_object[0] != '/') {
        std::stringstream ss;
        ss << "/" << m_object;
        object = ss.str();
    }

    // stat file
    auto file_attr = sftp_session.stat(object);
    auto len = file_attr.filesize;

    // open file
    auto sftp_handle = sftp_session.open(object, LIBSSH2_FXF_READ, 0);

    // get and save file
    ProgressBar pg(len);
    std::ofstream ofs;
    ofs.open(fileToSave);
    if (ofs.fail())
        EXCEPTION("Failed to open file " << fileToSave);

    while (42) {
        char buffer[4096];
        auto read = sftp_handle.read(buffer, sizeof(buffer));
        if (read < 0)
            EXCEPTION("libssh2_sftp_read() failed.");
        if (read == 0)
            break;
        ofs.write(buffer, read);
        pg.update(read);
    }

    // shutdown libssh2
    libssh2_exit();
}
