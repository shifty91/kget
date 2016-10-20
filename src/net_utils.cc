
/*
 * Copyright (C) 2015-2016 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "logger.h"
#include "net_utils.h"

int NetUtils::tcp_connect(const std::string& host, const std::string& service)
{
    int res;
    int sock;
    struct addrinfo *sa_head, *sa, hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    // prefer IPv6
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags  = AI_ADDRCONFIG;

    res = getaddrinfo(host.c_str(), service.c_str(), &hints, &sa_head);
    if (res)
        EXCEPTION("getaddrinfo() for host " << host << " failed: " << gai_strerror(res));

    // try to connect to some record...
    for (sa = sa_head; sa; sa = sa->ai_next) {
        sock = socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
        if (sock < 0) {
            log_dbg("socket() failed: " << strerror(errno) <<
                    ". Trying next address.");
            continue;
        }

        if (!connect(sock, sa->ai_addr, sa->ai_addrlen))
            break;

        close(sock);
    }
    freeaddrinfo(sa_head);

    if (!sa)
        EXCEPTION("connect() for host " << host << " on service " << service <<
                  " failed: " << strerror(errno));

    return sock;
}
