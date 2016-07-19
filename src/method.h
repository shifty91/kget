#ifndef _METHOD_H_
#define _METHOD_H_

#include <string>

#include "tcp_connection.h"

/**
 * This class provides the interface for a supported method.
 */
class Method
{
protected:
    std::string m_host;
    std::string m_object;

public:
    Method(const std::string& host, const std::string& object) :
        m_host{host}, m_object{object}
    {}

    virtual ~Method()
    {}

    virtual void get(const std::string& fileToSave, const std::string& user = "",
                     const std::string& pw = "") const = 0;
};

#endif /* _METHOD_H_ */
