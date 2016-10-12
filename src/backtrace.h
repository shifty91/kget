#ifndef _BACKTRACE_H_
#define _BACKTRACE_H_

#include <iostream>
#include <cstddef>
#include <execinfo.h>

/**
 * This class can be used to determine the stacktrace of the
 * current execution.
 */
class BackTrace
{
private:
    static const std::size_t MAX_STACK_SIZE = 10;
    std::size_t m_size;
    void *m_array[MAX_STACK_SIZE];
    char **m_strings;

public:
    inline BackTrace()
    {
        m_size    = backtrace(m_array, MAX_STACK_SIZE);
        m_strings = backtrace_symbols(m_array, m_size);
    }

    BackTrace(const BackTrace& other) = delete;
    BackTrace(const BackTrace&& other) = delete;
    BackTrace& operator=(const BackTrace& other) = delete;
    BackTrace& operator=(const BackTrace&& other) = delete;

    inline ~BackTrace()
    {
        if (m_strings)
            free(m_strings);
    }

    inline void print_bt()
    {
        if (m_size == 0 || !m_strings)
            return;

        std::cout << "Stacktrace:" << std::endl;
        for (std::size_t i = 0; i < m_size; ++i)
            std::cout << "  " << m_strings[i] << std::endl;
    }
};

#endif /* _BACKTRACE_H_ */
