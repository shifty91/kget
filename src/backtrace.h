/*
 * Copyright (C) 2015-2020 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifdef HAVE_LIBUNWIND

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstddef>
#include <cxxabi.h>

#define UNW_LOCAL_ONLY
#include <libunwind.h>

/**
 * This class can be used to determine the stacktrace of the current execution.
 */
class BackTrace
{
public:
    inline BackTrace() noexcept
    {}

    inline ~BackTrace() noexcept
    {}

    inline void print_bt() const noexcept
    {
        std::stringstream ss;
        unw_cursor_t cursor; unw_context_t uc;

        auto ret = unw_getcontext(&uc);
        if (ret)
            return;
        ret = unw_init_local(&cursor, &uc);
        if (ret)
            return;

        while (unw_step(&cursor) > 0) {
            unw_word_t ip, offset;
            char symbol_name[1024];
            int status;

            ret = unw_get_reg(&cursor, UNW_REG_IP, &ip);
            if (ret)
                return;
            ret = unw_get_proc_name(&cursor, symbol_name, sizeof(symbol_name), &offset);
            if (ret)
                return;

            auto *demangled = abi::__cxa_demangle(symbol_name, nullptr, nullptr, &status);
            auto *symbol = status == 0 ? demangled : symbol_name;

            ss << "  " << symbol << "+0x" << std::hex << offset
               << " [0x" << std::hex << ip << "]" << std::endl;

            if (demangled)
                std::free(demangled);
        }

        std::cout << "Stacktrace:" << std::endl;
        std::cout << ss.str();
    }
};

#endif

#endif /* _BACKTRACE_H_ */
