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

#ifndef _SSH_INIT_H_
#define _SSH_INIT_H_

#include "get_config.h"

#ifdef HAVE_LIBSSH

#include <libssh2.h>

#include "logger.h"

/**
 * This class is responsible for initializing the LibSSH2
 * library. There should only be exactly one instance of this class.
 */
class SSHInit
{
public:
    inline SSHInit()
    {
        if (libssh2_init(0))
            EXCEPTION("libssh2_init() failed.");
    }

    inline ~SSHInit()
    {
        libssh2_exit();
    }
};

#endif

#endif /* _SSH_INIT_H_ */
