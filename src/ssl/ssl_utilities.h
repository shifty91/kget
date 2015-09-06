#ifndef _SSL_UTILITIES_H_
#define _SSL_UTILITIES_H_

#include <openssl/err.h>

#include "logger.h"

#define GET_SSL_EXCEPTION(msg)                    \
    do {                                          \
        ERR_print_errors_fp(stderr);              \
        EXCEPTION(msg);                           \
    } while (0)

#endif /* _SSL_UTILITIES_H_ */
