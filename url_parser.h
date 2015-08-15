#ifndef _URL_PARSER_H_
#define _URL_PARSER_H_

#include <string>

class URLParser
{
private:
    const std::string& m_url;
    std::string m_method;
    std::string m_host;
    std::string m_object;

public:
    explicit URLParser(const std::string& url) :
        m_url{url}
    {}

    void parse();

    inline
    const std::string& method() const
    {
        return m_method;
    }

    inline
    const std::string& host() const
    {
        return m_host;
    }

    inline
    const std::string& object() const
    {
        return m_object;
    }
};

#endif /* _URL_PARSER_H_ */
