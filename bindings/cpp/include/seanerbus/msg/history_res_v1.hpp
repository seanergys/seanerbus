// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include <cstddef>
#include <cstdint>

#include <capnp/endian.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/common.h>
#include <seanerbus/msg/msg.capnp.h>

namespace seanerbus

{
namespace msg
{

/**
 * The SeanerBUS Time-Series DB support automatic pre-aggregation.
 *
 * This type encodes those Aggregates.
 */
struct Aggregate
{
public:
    Aggregate(const ::Aggregate::Reader& rd)
    : minimum(rd.getMinimum()), maximum(rd.getMaximum()), sum(rd.getSum()), count(rd.getCount()),
      integral(rd.getIntegral()), activeTime(rd.getActiveTime())
    {
    }

    Aggregate()
    {
    }

    void to_capnp(::Aggregate::Builder bld)
    {
        bld.setMinimum(minimum);
        bld.setMaximum(maximum);
        bld.setSum(sum);
        bld.setCount(count);
        bld.setIntegral(integral);
        bld.setActiveTime(activeTime);
    }

    /**
     * The minimum value in the interval
     */
    double minimum;
    /**
     * The maximum value in the interval
     */
    double maximum;
    /**
     * The sum of the values in the interval
     */
    double sum;
    /**
     * The number of raw metric events that fall into the interval.
     *
     * The mean over the Aggregate is thus:
     *
     *      aggr.sum/aggr.count
     */
    uint64_t count;
    /**
     * The integral of the metric over the interval.
     *
     * TODO: Explain difference to sum
     */
    double integral;
    /**
     * TODO: Explain
     */
    uint64_t activeTime;
};

/**
 * A list of values as queried from a metric in the Time-Series DB
 */
class History
{
public:
    History() : time_delta({}), values(std::vector<double>{})
    {
    }

    History(std::vector<int64_t> time_delta, std::vector<double> values)
    : time_delta(time_delta), values(values)
    {
    }

    History(std::vector<int64_t> time_delta, std::vector<Aggregate> aggrs)
    : time_delta(time_delta), values(aggrs)
    {
    }

    History(::History::Reader rd)
    {
        time_delta.reserve(rd.getTimeDelta().size());

        for (const auto& td : rd.getTimeDelta())
        {
            time_delta.emplace_back(td);
        }

        auto values_rd = rd.getRawOrAgg();
        switch (values_rd.which())
        {
        case ::History::RawOrAgg::AGGREGATE:
        {
            values = std::vector<Aggregate>();

            auto& aggr_values = std::get<std::vector<Aggregate>>(values);

            for (const auto& aggr : values_rd.getAggregate())
            {
                aggr_values.emplace_back(Aggregate{ aggr });
            }
            break;
        }
        case ::History::RawOrAgg::VALUE:
        {
            values = std::vector<double>();
            auto& double_values = std::get<std::vector<double>>(values);
            for (const auto& value : values_rd.getValue())
            {
                double_values.emplace_back(value);
            }
        }
        break;
        }
    }

    void to_capnp(::History::Builder bld)
    {
        auto tds = bld.initTimeDelta(time_delta.size());

        for (size_t i = 0; i < time_delta.size(); i++)
        {
            tds.set(i, time_delta[i]);
        }

        if (std::holds_alternative<std::vector<double>>(values))
        {
            auto& raw_values = std::get<std::vector<double>>(values);

            auto raw_values_bld = bld.getRawOrAgg().initValue(raw_values.size());
            for (size_t i = 0; i < raw_values.size(); i++)
            {
                raw_values_bld.set(i, raw_values[i]);
            }
        }
        else
        {
            auto& aggr_values = std::get<std::vector<Aggregate>>(values);

            auto aggr_values_bld = bld.getRawOrAgg().initAggregate(aggr_values.size());

            for (size_t i = 0; i < aggr_values.size(); i++)
            {
                aggr_values[i].to_capnp(aggr_values_bld[i]);
            }
        }
    }

    /**
     * Checks if the queried data is made of raw values, e.g. pure pairs of integer timestamp
     * and double value
     */
    bool is_raw()
    {
        return std::holds_alternative<std::vector<double>>(values);
    }

    /**
     * Retrieves the values from the query, if the History contains raw metric values.
     *
     * (e.g. is_raw() MUST be true for this function to return values)
     *
     * @returns A vector of the values in the query.
     */
    std::vector<double> get_raw()
    {
        if (std::holds_alternative<std::vector<double>>(values))
        {
            return std::get<std::vector<double>>(values);
        }

        throw std::runtime_error("Tried to get raw values from an aggregated history!");
    }

    /**
     * Retrieves the values from the query, if the History contains pre-aggregated Aggregates.
     *
     * (e.g. is_raw() MUST be false for this function to return values)
     *
     * @returns A vector of the Aggregate in the query
     */
    std::vector<Aggregate> get_aggr()
    {
        if (std::holds_alternative<std::vector<Aggregate>>(values))
        {
            return std::get<std::vector<Aggregate>>(values);
        }
        throw std::runtime_error("Tried to get aggregated values from a raw history!");
    }

    /**
     * Retrieves the timestamps from the query.
     *
     *      hist.get_time_delta().size() == hist.get_raw().size()
     *
     * Is required to hold.
     *
     * @returns A vector of timestamps
     */
    std::vector<int64_t> get_time_delta()
    {
        return time_delta;
    }

private:
    std::vector<int64_t> time_delta;
    std::variant<std::vector<double>, std::vector<Aggregate>> values;
};

class HistoryResV1
{
public:
    /**
     * Decode a payload as a HistoryResV1
     */
    HistoryResV1(std::pair<uint64_t, std::vector<std::byte>> payload)
    {
        if (payload.first != (uint64_t)::MessageTypes::HISTORY_RES_V1)
        {
            throw std::runtime_error(
                "Trying to decode a message as a HistoryResV1 that is not a HistoryResV1");
        }
        capnp::MallocMessageBuilder msg;

        capnp::initMessageBuilderFromFlatArrayCopy(
            kj::ArrayPtr(reinterpret_cast<capnp::word*>(payload.second.data()),
                         payload.second.size() / sizeof(capnp::word)),
            msg);

        ::HistoryResV1::Reader rd = msg.getRoot<::HistoryResV1>();

        switch (rd.getOkOrErr().which())
        {
        case ::HistoryResV1::OkOrErr::ERR:
            ok_or_err = rd.getOkOrErr().getErr();
            break;
        case ::HistoryResV1::OkOrErr::OK:
            ok_or_err = History(rd.getOkOrErr().getOk());
            break;
        }
    }

    /**
     * Creates a error HistoryResV1
     *
     * @param err_str error message to include in the HistoryResV1
     */
    static HistoryResV1 make_error(std::string err_str)
    {
        HistoryResV1 res;
        res.ok_or_err = err_str;

        return res;
    }

    /**
     * Creates a HistoryResV1 containing raw time-value pairs
     *
     *      time_delta.size() == values.size()
     *
     * must be true
     *
     * @param time_delta timestamps
     * @param values values
     */
    static HistoryResV1 make_raw(std::vector<int64_t> time_delta, std::vector<double> values)
    {
        HistoryResV1 res;
        res.ok_or_err = History(time_delta, values);

        return res;
    }

    /**
     * Creates a HistoryResV1 containing pre-aggregated values
     *
     *      time_delta.size() == aggrs.size()
     *
     * must be true
     *
     * @param time_delta timestamps
     * @param aggr aggregates
     */
    static HistoryResV1 make_aggr(std::vector<int64_t> time_delta, std::vector<Aggregate> aggrs)
    {
        HistoryResV1 res;
        res.ok_or_err = History(time_delta, aggrs);

        return res;
    }

    /**
     * Checks if this is a successful History request, e.g. if it contains values and not an error.
     */
    bool success()
    {
        return std::holds_alternative<History>(ok_or_err);
    }

    /**
     * Returns the History object of this response.
     * The History object contains the metric data that belongs to this response
     *
     * Only call this if
     *
     *      req.success() == true
     */
    History get_history()
    {
        return std::get<History>(ok_or_err);
    }

    /**
     * Returns the error message of this response.
     *
     * Only call this if
     *
     *      req.success() == false
     */
    std::string get_error()
    {
        return std::get<std::string>(ok_or_err);
    }

    /**
     * Output the Cap'n'Proto encoded data, which can then be send via Connection::publish, for
     * example.
     */
    std::pair<uint64_t, std::vector<std::byte>> to_capnp()
    {
        capnp::MallocMessageBuilder msg;

        ::HistoryResV1::Builder tv_msg = msg.initRoot<::HistoryResV1>();

        if (std::holds_alternative<History>(ok_or_err))
        {
            auto history_bld = tv_msg.getOkOrErr().initOk();
            std::get<History>(ok_or_err).to_capnp(history_bld);
        }
        else
        {
            tv_msg.getOkOrErr().setErr(std::get<std::string>(ok_or_err));
        }

        auto res = capnp::messageToFlatArray(msg);

        return std::make_pair(
            (uint64_t)::MessageTypes::HISTORY_RES_V1,
            std::vector<std::byte>((std::byte*)res.begin(), (std::byte*)res.end()));
    }

private:
    HistoryResV1() : ok_or_err("")
    {
    }

    std::variant<std::string, History> ok_or_err;
};

} // namespace msg
} // namespace seanerbus
