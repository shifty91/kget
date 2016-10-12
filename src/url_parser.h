/*
 * Copyright (C) 2015-2016 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * This file is part of Get.
 *
 * Get is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Get is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Get.  If not, see <http://www.gnu.org/licenses/>.
 */

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
