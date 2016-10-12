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

#ifndef _SSH_UTILITIES_H_
#define _SSH_UTILITIES_H_

#include <string>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include "logger.h"

#define SSH_EXCEPTION(session, msg)                                 \
    do {                                                            \
        char *reason;                                               \
        libssh2_session_last_error(session, &reason, nullptr, 0);   \
        EXCEPTION(msg << ": " << reason);                           \
    } while (0)

#define SFTP_EXCEPTION(session, msg)                        \
    do {                                                    \
        auto error = libssh2_sftp_last_error(session);      \
        std::string reason;                                 \
        switch (error) {                                    \
        case LIBSSH2_FX_EOF:                                \
            reason = "End of File"; break;                  \
        case LIBSSH2_FX_NO_SUCH_FILE:                       \
            reason = "No such file or directory"; break;    \
        case LIBSSH2_FX_PERMISSION_DENIED:                  \
            reason = "Permission denied"; break;            \
        case LIBSSH2_FX_FAILURE:                            \
            reason = "Failure"; break;                      \
        case LIBSSH2_FX_BAD_MESSAGE:                        \
            reason = "Bad message"; break;                  \
        case LIBSSH2_FX_NO_CONNECTION:                      \
            reason = "No connection"; break;                \
        case LIBSSH2_FX_CONNECTION_LOST:                    \
            reason = "Connection lost"; break;              \
        case LIBSSH2_FX_OP_UNSUPPORTED:                     \
            reason = "Operation unsupported"; break;        \
        case LIBSSH2_FX_INVALID_HANDLE:                     \
            reason = "Invalid handle"; break;               \
        case LIBSSH2_FX_NO_SUCH_PATH:                       \
            reason = "No such path"; break;                 \
        case LIBSSH2_FX_FILE_ALREADY_EXISTS:                \
            reason = "File already exists"; break;          \
        case LIBSSH2_FX_WRITE_PROTECT:                      \
            reason = "File is write protected"; break;      \
        case LIBSSH2_FX_NO_MEDIA:                           \
            reason = "No media"; break;                     \
        case LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM:             \
            reason = "Filesystem is full"; break;           \
        case LIBSSH2_FX_QUOTA_EXCEEDED:                     \
            reason = "Quota exceeded"; break;               \
        case LIBSSH2_FX_UNKNOWN_PRINCIPLE:                  \
            reason = "Unknown principal"; break;            \
        case LIBSSH2_FX_LOCK_CONFLICT:                      \
            reason = "Lock conflict"; break;                \
        case LIBSSH2_FX_DIR_NOT_EMPTY:                      \
            reason = "Directory is not empty"; break;       \
        case LIBSSH2_FX_NOT_A_DIRECTORY:                    \
            reason = "Directory is not empty"; break;       \
        case LIBSSH2_FX_INVALID_FILENAME:                   \
            reason = "Invalid filename"; break;             \
        case LIBSSH2_FX_LINK_LOOP:                          \
            reason = "Link loop"; break;                    \
        }                                                   \
        EXCEPTION(msg << ": " << reason);                   \
    } while (0)

#endif /* _SSH_UTILITIES_H_ */
