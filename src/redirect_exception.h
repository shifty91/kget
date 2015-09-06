#ifndef _REDIRECT_EXCEPTION_H_
#define _REDIRECT_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <sstream>

class RedirectException : public std::runtime_error
{
private:
    const std::string m_url;

public:
    RedirectException(const std::string& url) :
        std::runtime_error("HTTP Redirection"), m_url{url}
    {}

    virtual const char* what() const throw()
    {
        std::stringstream ss;
        ss << std::runtime_error::what() << " to the new URL: " << m_url;
        return ss.str().c_str();
    }

    const std::string& url() const
    {
        return m_url;
    }
};

#endif /* _REDIRECT_EXCEPTION_H_ */
