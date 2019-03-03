/*
 * Copyright (C) 2015-2018 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <libgen.h>

#include "config.h"
#include "backtrace.h"
#include "get_config.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define GET_BASENAME(str)                       \
    (basename(const_cast<char *>(str)))

template<typename... Args>
static inline void log_common(const std::string& level, const std::string& file,
                              int line, Args&&... args)
{
    std::cerr << "[" << level << ": " << GET_BASENAME(file.c_str()) << ":"
              << line << "]: ";
    (std::cerr << ... << std::forward<Args>(args));
    std::cerr << std::endl;
}

#define log_err(...)                                            \
    do {                                                        \
        log_common("ERROR", __FILE__, __LINE__, __VA_ARGS__);   \
    } while (0)

#define log_info(...)                                            \
    do {                                                         \
        log_common("INFO", __FILE__, __LINE__, __VA_ARGS__);     \
    } while (0)

#define log_dbg(...)                                              \
    do {                                                          \
        if (unlikely(Config::instance()->debug()))                \
            log_common("DEBUG", __FILE__, __LINE__, __VA_ARGS__); \
    } while (0)

#ifdef HAVE_BACKTRACE
#define EXCEPTION_TYPE(type, msg)                       \
    do {                                                \
        std::stringstream ss;                           \
        ss << msg;                                      \
        log_err(ss.str());                              \
        if (unlikely(Config::instance()->debug()))      \
            BackTrace().print_bt();                     \
        throw std::type(ss.str());                      \
    } while (0)
#else
#define EXCEPTION_TYPE(type, msg)                       \
    do {                                                \
        std::stringstream ss;                           \
        ss << msg;                                      \
        log_err(ss.str());                              \
        throw std::type(ss.str());                      \
    } while (0)
#endif

#define EXCEPTION(msg)                          \
    EXCEPTION_TYPE(logic_error, msg)

#endif /* _LOGGER_H_ */
