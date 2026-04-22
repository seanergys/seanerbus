// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <seanerbus/internal/message.hpp>

#include <stdexcept>
#include <utility>
#include <vector>

#include <cstddef>
#include <cstdint>

#include <capnp/endian.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <seanerbus/msg/msg.capnp.h>

namespace seanerbus

{
namespace msg
{

/**
 * A request containing a Vector of float values.
 *
 * Useful for encoding tuples of doubles, for example
 */

enum class HistoryType : uint64_t
{
    AGGREGATE = 0,
    FLEX_TIMELINE = 1
};

class HistoryReqV1
{
public:
    /**
     * Decode a payload as a HistoryReqV1
     */
    HistoryReqV1(std::pair<uint64_t, std::vector<std::byte>> payload) : addr(Address::locator())
    {
        if (payload.first != (uint64_t)::MessageTypes::HISTORY_REQ_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a HistoryReqV1 that is not a HistoryReqV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::HistoryReqV1::Reader rd = msg.getRoot<::HistoryReqV1>();

        addr = Address(seanerbus::Uuid(rd.getAddress().getUpper(), rd.getAddress().getLower()));
        start = rd.getStartTime();
        end = rd.getEndTime();
        interval_max = rd.getIntervalMax();
        type = (seanerbus::msg::HistoryType)rd.getType();
    }

    /**
     * Create a HistoryReqV1
     *
     * @param addr Address of the metric to query
     * @param start start of the time interval to query
     * @param end end of the time interval to query
     * @param interval_max TODO: explain (can be 0 for now)
     * @param type Either HistoryType::AGGREGATE to query ONE aggregation over the whole interval or
     * HistoryType::FLEX_TIMELINE to retrieve multiple values over the interval.
     */
    explicit HistoryReqV1(Address addr, int64_t start, int64_t end, int64_t interval_max,
                          HistoryType type)
    : addr(addr), start(start), end(end), interval_max(interval_max), type(type)
    {
    }

    /**
     * Output the Cap'n'Proto encoded data, as ingested by the C++ client library
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::HistoryReqV1::Builder tv_msg = msg.initRoot<::HistoryReqV1>();

        auto addr_bld = tv_msg.initAddress();

        addr_bld.setLower(addr.u_.lower);
        addr_bld.setUpper(addr.u_.upper);
        tv_msg.setStartTime(start);
        tv_msg.setEndTime(end);
        tv_msg.setIntervalMax(interval_max);
        tv_msg.setType((::HistoryType)type);

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::HISTORY_REQ_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

    Address addr;
    int64_t start, end;
    int64_t interval_max;
    HistoryType type;
};

} // namespace msg
} // namespace seanerbus
