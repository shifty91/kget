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

#include <iostream>
#include <array>
#include <sstream>
#include <cassert>
#include <cmath>

#include "progress_bar.h"

ProgressBar::UnitPair ProgressBar::unit(std::size_t file_size,
                                        unsigned precision) const
{
    static const std::array<std::string, 5> units = {
        { "B" , "KiB", "MiB", "GiB", "TiB" }
    };

    std::array<std::string, 5>::size_type i = 0;
    double size = file_size;
    while (size >= 1024.0) {
        size /= 1024.0;
        ++i;
    }
    assert(static_cast<decltype(units.size())>(i) < units.size());

    if (precision > 0)
        size = std::round(size * 10. * precision) / (10. * precision);
    else
        size = std::round(size);

    return { size, units[i] };
}

std::string ProgressBar::build_size() const
{
    std::stringstream ss;

    auto unit1 = unit(m_bytes_received);
    auto unit2 = unit(m_bytes);

    ss << unit1.first << " " << unit1.second << " / "
       << unit2.first << " " << unit2.second;

    return ss.str();
}

std::string ProgressBar::rate(std::size_t bytes_received)
{
    // calc
    auto now = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration_cast<
        std::chrono::duration<
            double, std::chrono::seconds::period> >(
                 now - m_old_time).count();
    m_old_time = now;
    std::size_t rate = bytes_received / elapsed;
    auto unit_ = unit(rate, 1);

    // build
    std::stringstream ss;
    ss << unit_.first << " " << unit_.second << "/s";
    return ss.str();
}

void ProgressBar::update(std::size_t new_bytes)
{
    m_bytes_received += new_bytes;
    const double progress = static_cast<double>(m_bytes_received) /
        static_cast<double>(m_bytes);
    const unsigned position = progress * m_width;
    std::string unit_str = build_size();

    // done?
    if (m_bytes_received >= m_bytes) {
        std::cout << "\r";
        for (auto i = 0u; i < m_cols - 1; ++i)
            std::cout << " ";
        std::cout << "\r";
        std::cout.flush();
        return;
    }

    // update?
    if (m_old_position == position)
        return;

    // redraw
    unsigned cnt = 0;
    m_old_position = position;
    auto rate_str = rate(m_bytes_received - m_old_byte_cnt);
    m_old_byte_cnt = m_bytes_received;
    std::cout << "\r[";
    for (unsigned i = 0; i < m_width; ++i, ++cnt) {
        if (i < position)
            std::cout << "*";
        else if (i == position)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] ";
    cnt += 2;
    std::cout << unit_str << " @ " << rate_str;
    cnt += unit_str.size() + rate_str.size() + 3;
    for (auto i = cnt; i < m_cols - 1; ++i)
        std::cout << " ";
    std::cout.flush();
}
