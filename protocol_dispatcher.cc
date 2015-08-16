#include "tcp_connection.h"
#include "tcp_ssl_connection.h"
#include "url_parser.h"
#include "logger.h"
#include "http.h"
#include "ftp.h"
#include "redirect_exception.h"
#include "auth_exception.h"
#include "utils.h"
#include "config.h"

#include "protocol_dispatcher.h"

void ProtocolDispatcher::dispatch()
{
    Config *config = Config::instance();

    while (42) {
        URLParser parser(m_url);

        parser.parse();
        log_dbg("Parsing results...");
        log_dbg("  Method: " << parser.method());
        log_dbg("  Host: "   << parser.host()  );
        log_dbg("  Object: " << parser.object());

        std::string name = basename(const_cast<char *>(parser.object().c_str()));
        if (name == "")
            EXCEPTION("URL does not have a valid object.");

        // here: catch only redirect|auth exceptions, everything else is just forwarded
        try {
            if (parser.method() == "http") {
                HTTPMethod<> http(parser.host(), parser.object());
                http.get(name, m_user, m_pw);
            } else if (parser.method() == "https") {
                HTTPMethod<TCPSSLConnection> https(parser.host(), parser.object());
                https.get(name, m_user, m_pw);
            } else if (parser.method() == "ftp") {
                FTPMethod ftp(parser.host(), parser.object());
                ftp.get(name, m_user, m_pw);
            } else {
                EXCEPTION("The method " << parser.method() <<
                          " is not supported right now.");
            }
        } catch (const RedirectException& ex) {
            if (config->follow_redirects()) {
                const auto& url = ex.url();
                log_info("HTTP redirect detected. Going to URL: " << url);
                m_url = url;
                continue;
            }
            log_info("HTTP redirect detected. Following redirects disabled.");
            break;
        } catch (const AuthException& ex) {
            log_info("HTTP Authorization detected. Please provide your credentials: ");
            std::cout << "Username: ";
            std::cin >> m_user;
            Utils::hideStdinKeystrokes();
            std::cout << "Password: ";
            std::cin >> m_pw;
            Utils::showStdinKeystrokes();
            continue;
        }

        log_info("File saved to " << name);

        break;
    }
}
