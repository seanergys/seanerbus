// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/helpers/resolve.hpp>
#include <seanerbus/internal/message.hpp>
#include <seanerbus/msg/history_req_v1.hpp>
#include <seanerbus/msg/history_res_v1.hpp>

#include <cstdint>

namespace seanerbus
{
namespace helpers
{
/**
 * For the metric associated with addr, query data from timepoints start to end.
 *
 * @param addr The address of the metric we want to query.
 * @param start The timepoint from which the query starts
 * @param end  The timepoint at which the query stops
 * @param interval_max TODO: Describe, leave as 0 for now
 *
 * @returns The msg::History data pertaining to that query.
 *
 */
msg::History get_timeline(client::Connection& conn, Address addr, uint64_t start, uint64_t end,
                          uint64_t interval_max = 0);

/**
 * Get ONE aggregate of information about the a metric in the time frame start to end.
 *
 * The msg::Aggregate will contain certain statistical measures of the metric in the time-frame
 * [start,end] e.g. minimum, maximum, how many values fall into the time-frame etc.
 *
 * @param addr The address of the metric to query
 * @param start startpoint of Aggregate
 * @param end endpoint of Aggregate
 *
 * @returns msg::Aggregate for the given interval
 */
msg::Aggregate get_aggregate(client::Connection&, Address addr, uint64_t start, uint64_t end);
} // namespace helpers
} // namespace seanerbus
