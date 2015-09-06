#ifndef _BASE64_H_
#define _BASE64_H_

#include <string>

#include "logger.h"

class Base64
{
private:
    std::string m_content;

public:
    explicit Base64(const std::string& content) :
        m_content{content}
    {}

    std::string encode() const;

    inline std::string decode() const
    {
        EXCEPTION("Base64::decode is not implemented!");
    }
};

#endif /* _BASE64_H_ */
