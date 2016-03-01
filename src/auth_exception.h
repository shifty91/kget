#ifndef _AUTH_EXCEPTION_H_
#define _AUTH_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <sstream>

class AuthException : public std::runtime_error
{
private:
    std::string m_additionalInfo;

public:
    AuthException(const std::string& additionalInfo = "") :
        std::runtime_error("HTTP Authorization required"),
        m_additionalInfo{additionalInfo}
    {}

    virtual const char* what() const throw()
    {
        std::stringstream ss;
        ss << std::runtime_error::what();
        if (m_additionalInfo != "")
            ss << ": " << m_additionalInfo;
        return ss.str().c_str();
    }
};

#endif /* _AUTH_EXCEPTION_H_ */
