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
