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
