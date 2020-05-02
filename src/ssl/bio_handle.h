/*
 * Copyright (C) 2015-2020 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _BIO_HANDLE_H_
#define _BIO_HANDLE_H_

#include "get_config.h"

#ifdef HAVE_OPENSSL

#include <openssl/bio.h>

#include "logger.h"

class BIOHandle
{
public:
    inline BIOHandle() :
        m_handle{nullptr}
    {}

    template<typename T>
    inline explicit BIOHandle(T&& type)
    {
        new_bio(type);
    }

    inline ~BIOHandle()
    {
        if (m_handle != nullptr)
            BIO_free(m_handle);
    }

    BIOHandle(const BIOHandle& handle) = delete;
    BIOHandle(BIOHandle&& handle) = delete;
    BIOHandle& operator=(const BIOHandle& handle) = delete;
    BIOHandle& operator=(BIOHandle&& handle) = delete;

    inline BIO *handle() noexcept
    {
        return m_handle;
    }

    template<typename T>
    inline void new_bio(T&& type)
    {
        m_handle = BIO_new(type);
        if (m_handle == nullptr)
            EXCEPTION("BIO_new() failed.");
    }

    inline int write(const void *buf, int len) noexcept
    {
        return BIO_write(m_handle, buf, len);
    }

    inline void push(BIOHandle& append) noexcept
    {
        BIO_push(m_handle, append.handle());
    }

    inline void pop() noexcept
    {
        BIO_pop(m_handle);
    }

    inline int flush() noexcept
    {
        return BIO_flush(m_handle);
    }

    inline BUF_MEM *get_mem_ptr() const noexcept
    {
        BUF_MEM *p;
        BIO_get_mem_ptr(m_handle, &p);
        return p;
    }

private:
    BIO *m_handle;
};

#endif

#endif /* _BIO_HANDLE_H_ */
