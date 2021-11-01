/*
 * Copyright (C) 2015-2021 Kurt Kanzenbach <kurt@kmk-computers.de>
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

#ifndef _PROTOCOL_DISPATCHER_H_
#define _PROTOCOL_DISPATCHER_H_

#include <string>
#include <memory>
#include <unordered_map>

#include "request.h"
#include "method.h"

class ProtocolDispatcher
{
public:
    using ProtoMap = std::unordered_map<std::string, std::unique_ptr<Method> >;

    ProtocolDispatcher(const std::string& url, const std::string& output = "") :
        m_url{url}, m_output{output}
    {}

    void dispatch();

private:
    static ProtoMap protoMap;
    /**
     * We need to explicitly protoMap, b/o initializer lists make copies of
     * std::unique_ptrs which doesn't work :(.
     */
    static bool initialized;
    static void init();

    std::string m_url;
    std::string m_output;

    Request build_request() const;
};

#endif /* _PROTOCOL_DISPATCHER_H_ */
