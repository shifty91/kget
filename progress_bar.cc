#include "progress_bar.h"

#include <iostream>

void ProgressBar::update(std::size_t new_bytes)
{
    m_bytes_received += new_bytes;
    const double  progress = static_cast<double>(m_bytes_received) /
        static_cast<double>(m_bytes);
    const unsigned position = progress * m_width;

    if (m_bytes_received >= m_bytes) {
        std::cout << "\r";
        for (unsigned i = 0; i < m_width + 2; ++i)
            std::cout << " ";
        std::cout << "\r";
        std::cout.flush();
        return;
    }

    std::cout << "\r[";
    for (unsigned i = 0; i < m_width; ++i) {
        if (i < position)
            std::cout << "*";
        else if (i == position)
            std::cout << ">";
        else
            std::cout << " ";
    }
    std::cout << "]";
    std::cout.flush();
}
