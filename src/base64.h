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

#ifndef _BASE64_H_
#define _BASE64_H_

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include <string>

#include "logger.h"

class Base64
{
public:
    explicit Base64(const std::string& content) :
        m_content{content}
    {}

    std::string encode() const;

    inline std::string decode() const
    {
        EXCEPTION("Base64::decode is not implemented!");
    }

private:
    std::string m_content;
};

#endif

#endif /* _BASE64_H_ */
