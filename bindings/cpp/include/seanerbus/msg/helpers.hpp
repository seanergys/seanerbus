// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <map>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <seanerbus/msg/msg.capnp.h>

namespace seanerbus
{
namespace msg
{

/**
 * Converts a payload type number into the text representation,
 *
 * e.g. 0 -> "TimeValueV1"
 *
 * @param payload_type The payload type number (e.g. 0) to convert
 * @returns the name associated with the payload_type number (e.g. "TimeValueV1")
 */
inline std::string payload_type_to_str(uint64_t payload_type)
{
    switch ((MessageTypes)payload_type)
    {
    case MessageTypes::TIME_VALUE_V1:
        return "TimeValueV1";
    case MessageTypes::FLUX_LIFECYCLE_EVENT_V1:
        return "FluxLifecycleEventV1";
    case MessageTypes::HELLO_WORLD_REQ_V1:
        return "HelloWorldReqV1";
    case MessageTypes::HELLO_WORLD_RES_V1:
        return "HelloWorldResV1";
    case MessageTypes::HISTORY_REQ_V1:
        return "HistoryReqV1";
    case MessageTypes::HISTORY_RES_V1:
        return "HistoryResV1";
    case MessageTypes::REGISTER_RESOLVE_REQ_V1:
        return "RegisterResolveReqV1";
    case MessageTypes::REGISTER_RESOLVE_RES_V1:
        return "RegisterResolveResV1";
    case MessageTypes::RESOLVE_REQ_V1:
        return "ResolveReqV1";
    case MessageTypes::RESOLVE_RES_V1:
        return "ResolveResV1";
    case MessageTypes::UNREGISTER_RESOLVE_REQ_V1:
        return "UnregisterResolveReqV1";
    case MessageTypes::UNREGISTER_RESOLVE_RES_V1:
        return "UnregisterResolveResV1";
    case MessageTypes::VECTOR_REQ_V1:
        return "VectorReqV1";
    case MessageTypes::VECTOR_RES_V1:
        return "VectorResV1";
    case MessageTypes::REVERSE_RESOLVE_REQ_V1:
        return "ReverseResolveReqV1";
    case MessageTypes::REVERSE_RESOLVE_RES_V1:
        return "ReverseResolveResV1";
    default:
        return fmt::format("Unknown payload type ({})", payload_type);
    }
}

/**
 * Converts a name map to a a human-readable JSON string
 *
 * @param name The name to convert
 * @returns The name as a string (e.g. {{"foo" : "bar"}})
 */
inline std::string name_map_to_str(const std::map<std::string, std::string>& name)
{
    std::string res;

    std::vector<std::string> temp;
    for (const auto& elem : name)
    {
        temp.emplace_back(fmt::format("\"{}\": \"{}\"", elem.first, elem.second));
    }

    return fmt::format("{{{}}}", fmt::join(temp, ", "));
}
} // namespace msg
} // namespace seanerbus
