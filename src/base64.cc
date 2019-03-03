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

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include "base64.h"
#include "logger.h"
#include "ssl/ssl_wrapper.h"

std::string Base64::encode() const
{
    std::string result;
    BIOHandle b64(BIO_f_base64()), bmem(BIO_s_mem());

    b64.push(bmem);
    b64.write(m_content.data(), m_content.size());
    b64.flush();
    auto *bptr = b64.get_mem_ptr();

    result.reserve(bptr->length);
    result.insert(0, bptr->data, bptr->length-1);
    result[bptr->length-1] = '\0';

    b64.pop();

    return result;
}

#endif
