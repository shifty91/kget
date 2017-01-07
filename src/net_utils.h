/*
 * Copyright (C) 2015-2017 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _NET_UTILS_H_
#define _NET_UTILS_H_

#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/**
 * This class provides networking utilities for the tcp connections.
 */
class NetUtils
{
public:
    static int tcp_connect(const std::string& host, const std::string& service);

private:
    static void print_ip(const struct addrinfo *sa);
};

#endif /* _NET_UTILS_H_ */
