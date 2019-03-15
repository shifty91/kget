/*
 * Copyright (C) 2015-2019 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#include <string>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "connection.h"
#include "logger.h"

std::string Connection::get_ip(const struct addrinfo *sa)
{
    char addr[INET6_ADDRSTRLEN + 1];
    const char *res = NULL;
    void *in_addr = NULL;

    if (!sa)
        return "";

    switch (sa->ai_family) {
    case AF_INET6:
        in_addr = &(((struct sockaddr_in6 *)sa->ai_addr)->sin6_addr);
        break;
    case AF_INET:
        in_addr = &(((struct sockaddr_in *)sa->ai_addr)->sin_addr);
        break;
    }

    if (!in_addr)
        return "";

    res = inet_ntop(sa->ai_family, in_addr, addr, sizeof(addr));
    if (!res) {
        log_dbg("inet_ntop() failed: ", strerror(errno));
        return "";
    }

    return addr;
}

void Connection::tcp_connect(const std::string& host, const std::string& service)
{
    struct addrinfo *sa_head, *sa, hints;
    auto *config = Config::instance();
    int res;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = config->use_ipv4_only() ? AF_INET :
        config->use_ipv6_only() ? AF_INET6 : AF_UNSPEC;
    hints.ai_flags  = AI_ADDRCONFIG;

    res = getaddrinfo(host.c_str(), service.c_str(), &hints, &sa_head);
    if (res)
        EXCEPTION("getaddrinfo() for host ", host, " failed: ", gai_strerror(res));

    // try to connect to some record...
    for (sa = sa_head; sa; sa = sa->ai_next) {
        m_sock = ::socket(sa->ai_family, sa->ai_socktype, sa->ai_protocol);
        if (m_sock < 0) {
            log_dbg("socket() failed: ", strerror(errno),
                    ". Trying next address.");
            continue;
        }

        if (!::connect(m_sock, sa->ai_addr, sa->ai_addrlen)) {
            log_dbg("Connected to ", host, "(", get_ip(sa), ") @ ", service);
            break;
        }

        ::close(m_sock);
    }
    freeaddrinfo(sa_head);

    if (!sa)
        EXCEPTION("connect() for host ", host, " on service ", service,
                  " failed: ", strerror(errno));
}

void Connection::set_default_timeout()
{
    struct timeval tv;

    tv.tv_sec = 30;
    tv.tv_usec = 0;

    if (setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, &tv,
                   sizeof(struct timeval)))
        EXCEPTION("setsockopt() failed: ", strerror(errno));
}
