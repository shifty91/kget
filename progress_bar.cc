#include "progress_bar.h"

#include <iostream>
#include <array>
#include <sstream>
#include <cassert>

std::pair<std::size_t, std::string> ProgressBar::unit(std::size_t file_size) const
{
    const std::array<std::string, 5> units = {
        "B", "KiB", "MiB", "GiB", "TiB"
    };

    int i = 0;
    std::size_t size = file_size;
    while (size >= 1024) {
        size /= 1024;
        ++i;
    }
    assert(i < units.size());

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
        for (unsigned i = 0; i < m_width + unit_str.size() + 10; ++i)
            std::cout << " ";
        std::cout << "\r";
        std::cout.flush();
        return;
    }

    // update?
    if (m_old_position == position)
        return;

    // redraw
    m_old_position = position;
    std::cout << "\r[";
    for (unsigned i = 0; i < m_width; ++i) {
        if (i < position)
            std::cout << "*";
        else if (i == position)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "] ";
    std::cout << unit_str;
    std::cout.flush();
}
