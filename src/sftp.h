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

#ifndef _SFTP_H_
#define _SFTP_H_

#include "get_config.h"

#ifdef HAVE_LIBSSH

#include <string>
#include <vector>
#include <utility>

#include "method.h"
#include "tcp_connection.h"
#include "ssh/ssh_wrapper.h"

using KeyPairVector = std::vector<std::pair<std::string, std::string> >;

class SFTPMethod : Method
{
public:
    SFTPMethod(const std::string& host, const std::string& object) :
        Method(host, object)
    {}

    virtual void get(const std::string& fileToSave, const std::string& user = "",
                     const std::string& pw = "") const override;

private:
    static SSHInit m_ssh_init;

    KeyPairVector find_user_keys() const;
    void print_fingerprint(const std::string& fingerprint) const;
    void publickey_auth(SSHSession& session, const std::string& user) const;
};

#endif

#endif /* _SFTP_H_ */
