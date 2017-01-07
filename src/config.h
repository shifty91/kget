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

#ifndef _CONFIG_H_
#define _CONFIG_H_

class Config final
{
public:
    ~Config()
    {
        delete m_instance;
    }

    static Config *instance()
    {
        if (!m_instance)
            m_instance = new Config();
        return m_instance;
    }

    inline const bool& show_pg() const noexcept
    {
        return m_show_pg;
    }

    inline bool& show_pg() noexcept
    {
        return m_show_pg;
    }

    inline const bool& follow_redirects() const noexcept
    {
        return m_follow_redirects;
    }

    inline bool& follow_redirects() noexcept
    {
        return m_follow_redirects;
    }

    inline const bool& verify_peer() const noexcept
    {
        return m_verify_peer;
    }

    inline bool& verify_peer() noexcept
    {
        return m_verify_peer;
    }

    inline const bool& use_sslv2() const noexcept
    {
        return m_use_sslv2;
    }

    inline bool& use_sslv2() noexcept
    {
        return m_use_sslv2;
    }

    inline const bool& use_sslv3() const noexcept
    {
        return m_use_sslv3;
    }

    inline bool& use_sslv3() noexcept
    {
        return m_use_sslv3;
    }

    inline const bool& debug() const noexcept
    {
        return m_debug;
    }

    inline bool& debug() noexcept
    {
        return m_debug;
    }

private:
    static Config *m_instance;

    Config() :
        m_show_pg{false}, m_follow_redirects{true}, m_verify_peer{false},
        m_use_sslv2{false}, m_use_sslv3{false}, m_debug{false}
    {}

    bool m_show_pg;
    bool m_follow_redirects;
    bool m_verify_peer;
    bool m_use_sslv2;
    bool m_use_sslv3;
    bool m_debug;
};

#endif /* _CONFIG_H_ */
