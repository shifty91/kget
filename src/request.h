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

#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <string>

/**
 * This class represents an user request.
 */
class Request
{
public:
    Request(const std::string& host, const std::string& object) :
        m_host{host}, m_object{object}
    {}

    inline const std::string& host() const noexcept
    {
        return m_host;
    }

    inline std::string& host() noexcept
    {
        return m_host;
    }

    inline const std::string& object() const noexcept
    {
        return m_object;
    }

    inline std::string& object() noexcept
    {
        return m_object;
    }

private:
    std::string m_host;
    std::string m_object;
};

#endif /* _REQUEST_H_ */
