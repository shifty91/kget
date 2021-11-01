/*
 * Copyright (C) 2015-2021 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _REDIRECT_EXCEPTION_H_
#define _REDIRECT_EXCEPTION_H_

#include <exception>
#include <string>
#include <sstream>

class RedirectException : public std::exception
{
public:
    RedirectException(const std::string& url) :
        std::exception(),
        m_url{url}
    {
        std::stringstream ss;
        ss << "HTTP Redirection to the new URL: " << m_url;
        m_what = ss.str();
    }

    virtual const char *what() const noexcept override
    {
        return m_what.c_str();
    }

    const std::string& url() const
    {
        return m_url;
    }

private:
    const std::string m_url;
    std::string m_what;
};

#endif /* _REDIRECT_EXCEPTION_H_ */
