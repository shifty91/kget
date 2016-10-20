#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <libgen.h>

#include "config.h"
#include "backtrace.h"

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define GET_BASENAME(str)                       \
    (basename(const_cast<char *>(str)))

#define log_err(msg)                                                    \
    do {                                                                \
        std::cerr << "[ERROR " << GET_BASENAME(__FILE__) << ":"         \
                  << __LINE__ << "]: " << msg << std::endl;             \
    } while (0)

#define log_dbg(msg)                                                    \
    do {                                                                \
        if (unlikely(Config::instance()->debug()))                      \
            std::cout << "[DEBUG " << GET_BASENAME(__FILE__) << ":"     \
                      << __LINE__ << "]: " << msg << std::endl;         \
    } while (0)

#define log_info(msg)                                                   \
    do {                                                                \
        std::cout << "[INFO " << GET_BASENAME(__FILE__) << ":"          \
                  << __LINE__ << "]: " << msg << std::endl;             \
    } while (0)

#define EXCEPTION_TYPE(type, msg)                       \
    do {                                                \
        std::stringstream ss;                           \
        ss << msg;                                      \
        log_err(ss.str());                              \
        if (unlikely(Config::instance()->debug()))      \
            BackTrace().print_bt();                     \
        throw std::type(ss.str());                      \
    } while (0)

#define EXCEPTION(msg)                          \
    EXCEPTION_TYPE(logic_error, msg)

#endif /* _LOGGER_H_ */
