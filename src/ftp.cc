#include <cctype>
#include <stdexcept>
#include <sstream>
#include <regex>
#include <fstream>
#include <cstring>

#include "ftp.h"
#include "logger.h"
#include "config.h"

#define CHECK_RESPONSE(tcp, response)                       \
    do {                                                    \
        check_response(tcp, response, __FILE__, __LINE__);  \
    } while (0)

void FTPMethod::get(const std::string& fileToSave, const std::string& user,
                    const std::string& pw) const
{
    TCPConnection tcp, tcp_pasv;
    int response, pasv_port;
    std::size_t len = 0;
    Config *config = Config::instance();

    tcp.connect(m_host, "ftp");
    CHECK_RESPONSE(tcp, 220);

    if (user == "")
        tcp.write("USER anonymous\r\n");
    else
        TCP_WRITE(tcp, "USER " << user << "\r\n");
    CHECK_RESPONSE(tcp, 331);
    if (pw == "")
        tcp.write("PASS asdf\r\n");
    else
        TCP_WRITE(tcp, "PASS " << pw << "\r\n");
    CHECK_RESPONSE(tcp, 230);

    log_dbg("Logged into FTP server at " << m_host);

    // change to binary mode
    tcp.write("TYPE I\r\n");
    CHECK_RESPONSE(tcp, 200);

    // get size
    TCP_WRITE(tcp, "SIZE " << m_object << "\r\n");
    auto line = tcp.read_ln();
    response = ftp_ret_code(line);
    if (response == 213) {
        len = ftp_size(line);
        log_dbg("File has a size of " << len << " bytes.");
    }

    // PASV/EPSV
    tcp.write("PASV\r\n");
    line = tcp.read_ln();
    response = ftp_ret_code(line);

    if (response == 227) {
        pasv_port = ftp_pasv_port(line);
    } else if (response == 501) {
        // hmz, PASV might not be supported -> trying EPSV
        tcp.write("EPSV\r\n");
        line = tcp.read_ln();
        CHECK_RESPONSE(line, 229);
        pasv_port = ftp_epsv_port(line);
    } else {
        EXCEPTION("FTP server doesn't support PASV nor EPSV. Giving up.");
    }

    if (pasv_port < 0)
        EXCEPTION("Failed to parse PASV port " << pasv_port);

    log_dbg("PASV p0rt is " << pasv_port);

    // connect to ftp data
    tcp_pasv.connect(m_host, pasv_port);

    // get file
    TCP_WRITE(tcp, "RETR " << m_object << "\r\n");
    CHECK_RESPONSE(tcp, 150);

    // fetch it and save to file
    std::ofstream ofs(fileToSave);
    if (ofs.fail())
        EXCEPTION("Failed to open file: " << fileToSave);
    if (config->show_pg())
        tcp_pasv.read_until_eof_with_pg_to_fstream(ofs, len);
    else
        tcp_pasv.read_until_eof_to_fstream(ofs);
    tcp_pasv.close();

    // done
    CHECK_RESPONSE(tcp, 226);
    tcp.write("QUIT\r\n");
    CHECK_RESPONSE(tcp, 221);
}

std::size_t FTPMethod::ftp_size(const std::string& line) const
{
    std::regex pattern("\\d+\\s*(\\d+)\\r\\n");
    std::smatch match;

    if (std::regex_match(line, match, pattern)) {
        std::string size(match[1]);
        return std::atoll(size.c_str());
    }

    EXCEPTION("Failed to parse size of requested file.");
}

int FTPMethod::ftp_pasv_port(const std::string& line) const
{
    std::regex pattern("\\d+[\\w ]+\\(\\d+,\\d+,\\d+,\\d+,(\\d+),(\\d+)\\).*\\r\\n");
    std::smatch match;

    if (std::regex_match(line, match, pattern)) {
        std::string p1(match[1]), p2(match[2]);
        return std::atoi(p1.c_str()) * 256 + std::atoi(p2.c_str());
    }

    EXCEPTION("Failed to parse PASV port.");
}

int FTPMethod::ftp_epsv_port(const std::string& line) const
{
    // grr, stupid c++ regex, using c style parsing instead
    for (decltype(line.size()) i = 0; i < line.size(); ++i)
        if (!strncmp(line.c_str() + i, "|||", 3))
            return std::atoi(line.c_str() + i + 3);

    EXCEPTION("Failed to parse EPSV port.");
}

int FTPMethod::ftp_ret_code(const std::string& response) const
{
    char a, b, c;

    if (response.size() < 3)
        EXCEPTION("Received garbage from FTP server.");

    a = response[0];
    b = response[1];
    c = response[2];

    if (!std::isdigit(a) || !std::isdigit(b) || !std::isdigit(c))
        EXCEPTION("Received garbage from FTP server.");

    return (a-'0')*100 + (b-'0')*10 + (c-'0');
}

void FTPMethod::check_response(const TCPConnection& tcp, int expected_response,
                               const std::string& file, int line) const
{
    check_response(tcp.read_ln(), expected_response, file, line);
}

void FTPMethod::check_response(const std::string& line, int expected_response,
                               const std::string& file, int src_line) const
{
    auto response = ftp_ret_code(line);

    if (response != expected_response)
        EXCEPTION("Received unexpected response code from FTP server " << response <<
                  " while " << expected_response << " was expected.\n"
                  "Origin of failure: " << file << ":" << src_line);
}
