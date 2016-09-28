#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

#include <string>

/**
 * This class provides networking utilities for the tcp connections.
 */
class NetUtils
{
public:
    static int tcp_connect(const std::string& host, const std::string& service);
};

#endif /* _NET_UTILS_H_ */
