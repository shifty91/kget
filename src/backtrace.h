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

#ifndef _BACKTRACE_H_
#define _BACKTRACE_H_

#include "get_config.h"

#ifdef HAVE_BACKTRACE

#include <iostream>
#include <cstddef>
#include <execinfo.h>

/**
 * This class can be used to determine the stacktrace of the current execution.
 */
class BackTrace
{
public:
    inline BackTrace() noexcept
    {
        m_size    = backtrace(m_array, MAX_STACK_SIZE);
        m_strings = backtrace_symbols(m_array, m_size);
    }

    BackTrace(const BackTrace& other) = delete;
    BackTrace(const BackTrace&& other) = delete;
    BackTrace& operator=(const BackTrace& other) = delete;
    BackTrace& operator=(const BackTrace&& other) = delete;

    inline ~BackTrace() noexcept
    {
        if (m_strings)
            free(m_strings);
    }

    inline void print_bt() const noexcept
    {
        if (m_size == 0 || !m_strings)
            return;

        std::cout << "Stacktrace:" << std::endl;
        for (std::size_t i = 0; i < m_size; ++i)
            std::cout << "  " << m_strings[i] << std::endl;
    }

private:
    static constexpr std::size_t MAX_STACK_SIZE = 10;

    std::size_t m_size;
    void *m_array[MAX_STACK_SIZE];
    char **m_strings;
};

#endif

#endif /* _BACKTRACE_H_ */
