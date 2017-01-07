/*
 * Copyright (C) 2015-2017 Kurt Kanzenbach <kurt@kmk-computers.de>
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
class Request final
{
public:
    Request(const std::string& method, const std::string& host,
            const std::string& object, const std::string& out_file_name,
            const std::string& user = "", const std::string& pw = "") :
        m_method{method}, m_host{host}, m_object{object},
        m_out_file_name{out_file_name}, m_user{user}, m_pw{pw}
    {}

    inline const std::string& method() const noexcept
    {
        return m_method;
    }

    inline std::string& method() noexcept
    {
        return m_method;
    }

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

    inline const std::string& out_file_name() const noexcept
    {
        return m_out_file_name;
    }

    inline std::string& out_file_name() noexcept
    {
        return m_out_file_name;
    }

    inline const std::string& user() const noexcept
    {
        return m_user;
    }

    inline std::string& user() noexcept
    {
        return m_user;
    }

    inline const std::string& pw() const noexcept
    {
        return m_pw;
    }

    inline std::string& pw() noexcept
    {
        return m_pw;
    }

private:
    std::string m_method;
    std::string m_host;
    std::string m_object;
    std::string m_out_file_name;
    std::string m_user;
    std::string m_pw;
};

#endif /* _REQUEST_H_ */
