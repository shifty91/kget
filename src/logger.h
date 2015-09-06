#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <iostream>
#include <stdexcept>
#include <sstream>

#define log_err(msg)                                                    \
    do {                                                                \
        std::cerr << "[ERROR " << __FILE__ << ":" << __LINE__ << "]: " << msg << std::endl; \
    } while (0)

#ifndef NDEBUG
#define log_dbg(msg)                                                    \
    do {                                                                \
        std::cout << "[DEBUG " << __FILE__ << ":" << __LINE__ << "]: " << msg << std::endl; \
    } while (0)
#else
#define log_dbg(msg)
#endif

#define log_info(msg)                                                   \
    do {                                                                \
        std::cout << "[INFO " << __FILE__ << ":" << __LINE__ << "]: " << msg << std::endl; \
    } while (0)

#define EXCEPTION_TYPE(type, msg)               \
    do {                                        \
        std::stringstream ss;                   \
        ss << msg;                              \
        log_err(ss.str());                      \
        throw std::type(ss.str());              \
    } while (0)

#define EXCEPTION(msg)                          \
    EXCEPTION_TYPE(logic_error, msg)

#endif /* _LOGGER_H_ */
