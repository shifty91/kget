#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

/**
 * This class provides networking utilities for the tcp connections.
 */
class NetUtils
{
public:
    static int tcp_connect(const char *host, const char *service);
};

#endif /* _NET_UTILS_H_ */
