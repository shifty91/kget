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

#ifndef _AUTH_EXCEPTION_H_
#define _AUTH_EXCEPTION_H_

#include <stdexcept>
#include <string>
#include <sstream>

class AuthException : public std::runtime_error
{
private:
    std::string m_additionalInfo;

public:
    AuthException(const std::string& additionalInfo = "") :
        std::runtime_error("HTTP Authorization required"),
        m_additionalInfo{additionalInfo}
    {}

    virtual const char* what() const throw()
    {
        std::stringstream ss;
        ss << std::runtime_error::what();
        if (m_additionalInfo != "")
            ss << ": " << m_additionalInfo;
        return ss.str().c_str();
    }
};

#endif /* _AUTH_EXCEPTION_H_ */
