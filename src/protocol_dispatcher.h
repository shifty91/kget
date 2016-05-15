#ifndef _PROTOCOL_DISPATCHER_H_
#define _PROTOCOL_DISPATCHER_H_

#include <string>

class ProtocolDispatcher
{
private:
    std::string m_url;
    std::string m_user;
    std::string m_pw;
    std::string m_output;

public:
    ProtocolDispatcher(const std::string& url, const std::string& user = "",
                       const std::string& pw = "", const std::string& output = "") :
        m_url{url}, m_user{user}, m_pw{pw}, m_output{output}
    {}

    void dispatch();
};

#endif /* _PROTOCOL_DISPATCHER_H_ */
