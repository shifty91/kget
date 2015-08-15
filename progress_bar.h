#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include <cstddef>

class ProgressBar
{
private:
    static const unsigned m_width = 80;
    const std::size_t m_bytes;
    std::size_t m_bytes_received;

public:
    ProgressBar(std::size_t bytes) :
        m_bytes{bytes}, m_bytes_received{0}
    {}

    void update(std::size_t new_bytes = 1);
};

#endif /* _PROGRESS_BAR_H_ */
