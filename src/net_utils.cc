#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "logger.h"
#include "net_utils.h"

int NetUtils::tcp_connect(const char *host, const char *service)
{
    int res;
    int sock;
    struct addrinfo *sa_head, *sa, hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    // prefere IPv6
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags  = AI_ADDRCONFIG;

    res = getaddrinfo(host, service, &hints, &sa_head);
    if (res)
        EXCEPTION("getaddrinfo() for host " << host << " failed: " << gai_strerror(res));

    // try to connect to some record...
    for (sa = sa_head; sa != NULL; sa = sa->ai_next) {
        sock = socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
        if (sock < 0) {
            log_err("socket() failed: " << strerror(errno));
            goto out;
        }

        if (!connect(sock, sa->ai_addr, sa->ai_addrlen))
            break;

        close(sock);
    }

    if (!sa)
        EXCEPTION("connect() for host " << host << " on service " << service <<
                  " failed: " << strerror(errno));

out:
    freeaddrinfo(sa_head);

    return sock;
}
