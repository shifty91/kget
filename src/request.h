#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <string>

/**
 * This class represents an user request.
 */
class Request
{
private:
    std::string m_host;
    std::string m_object;

public:
    Request(const std::string& host, const std::string& object) :
        m_host{host}, m_object{object}
    {}

    inline
    const std::string& host() const
    {
        return m_host;
    }

    inline
    std::string& host()
    {
        return m_host;
    }

    inline
    const std::string& object() const
    {
        return m_object;
    }

    inline
    std::string& object()
    {
        return m_object;
    }
};

#endif /* _REQUEST_H_ */
