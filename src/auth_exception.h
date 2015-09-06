#ifndef _AUTH_EXCEPTION_H_
#define _AUTH_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <sstream>

class AuthException : public std::runtime_error
{
private:
    std::string m_url;

public:
    AuthException(const std::string& url) :
        std::runtime_error("HTTP Authorization required"), m_url{url}
    {}

    virtual const char* what() const throw()
    {
        std::stringstream ss;
        ss << std::runtime_error::what() << " on URL: " << m_url;
        return ss.str().c_str();
    }

    const std::string& url() const
    {
        return m_url;
    }
};

#endif /* _AUTH_EXCEPTION_H_ */
