#ifndef _PROGRESS_BAR_H_
#define _PROGRESS_BAR_H_

#include <cstddef>
#include <string>
#include <utility>

/**
 * Progress bar looks like this:
 *
 *  [***>    ] 5MB / 791 MB
 */
class ProgressBar
{
private:
    static const unsigned m_width = 80;
    const std::size_t m_bytes;
    std::size_t m_bytes_received;
    unsigned m_old_position;

    std::pair<std::size_t, std::string> unit(std::size_t file_size) const;
    std::string build_size() const;

public:
    ProgressBar(std::size_t bytes) :
        m_bytes{bytes}, m_bytes_received{0}, m_old_position{0}
    {}

    void update(std::size_t new_bytes = 1);
};

#endif /* _PROGRESS_BAR_H_ */
