#ifndef _FTP_H_
#define _FTP_H_

#include "method.h"
#include "tcp_connection.h"

class FTPMethod : Method
{
private:
    int ftp_ret_code(const std::string& response) const;
    void check_response(const TCPConnection& tcp, int expected_response) const;
    void check_response(const std::string& line, int expected_response) const;
    std::size_t ftp_size(const std::string& line) const;
    int ftp_pasv_port(const std::string& line) const;
    int ftp_epsv_port(const std::string& line) const;

public:
    FTPMethod(const std::string& host, const std::string& object) :
        Method(host, object)
    {}

    virtual ~FTPMethod()
    {}

    virtual void get(const std::string& fileToSave, const std::string& user = "",
                     const std::string& pw = "") const override;
};

#endif /* _FTP_H_ */
