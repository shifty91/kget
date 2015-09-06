#include <regex>
#include <stdexcept>

#include "logger.h"
#include "url_parser.h"

void URLParser::parse()
{
    std::regex pattern("(\\w+)://(.+?)/(.+)");
    std::smatch match;

    std::regex_match(m_url, match, pattern);

    if (match.size() != 4)
        EXCEPTION("Failed to parse url: " << m_url);

    m_method = match[1];
    m_host   = match[2];
    m_object = match[3];
}
