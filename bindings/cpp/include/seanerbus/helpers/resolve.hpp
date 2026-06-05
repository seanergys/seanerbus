// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <seanerbus/client/connection.hpp>
#include <seanerbus/client/simple_event_loop.hpp>
#include <seanerbus/internal/message.hpp>
#include <seanerbus/msg/register_resolve_req_v1.hpp>
#include <seanerbus/msg/register_resolve_res_v1.hpp>
#include <seanerbus/msg/resolve_req_v1.hpp>
#include <seanerbus/msg/resolve_res_v1.hpp>
#include <seanerbus/msg/reverse_resolve_req_v1.hpp>
#include <seanerbus/msg/reverse_resolve_res_v1.hpp>
#include <seanerbus/msg/unregister_resolve_req_v1.hpp>
#include <seanerbus/msg/unregister_resolve_res_v1.hpp>

#include <map>
#include <string>

namespace seanerbus
{
namespace helpers
{

/**
 * Resolves a name (e.g. {{"service", "time_series_db"}}) to a list
 * of addresses (e.g. {"46703055-ebbb-470a-8d89-6c006fd76ce5"})
 *
 * For this, the naming server needs to run.
 *
 * A single resolve can lead to many returned addresses:
 *
 * If {{"node" : "*"}} is requested to be resolved, this will give every address of every node
 * (If such thing is defined in the naming service.
 *
 * @param name The name to be resolved
 *
 * @returns List of addresses that match the query.
 */
std::vector<Address> resolve(std::map<std::string, std::string> name);

/**
 * Does the inverse of resolve: translates an address (e.g. "46703055-ebbb-470a-8d89-6c006fd76ce5")
 * back to a name (e.g. {{"service", "time_series_db"}}
 *
 * For this, the naming server needs to run.
 *
 * @param addr Address to get the name of
 * @returns The name belonging to that addr
 */
std::map<std::string, std::string> reverse_resolve(Address addr);

/**
 * Registers a name to address mapping with the naming server.
 *
 * For this, the naming server needs to run.
 *
 * If:
 *
 *      register({{"foo", "bar"}}, "46703055-ebbb-470a-8d89-6c006fd76ce5")
 *
 * is executed, then after that:
 *
 *      resolve({{"foo", "bar"}}) ->  "46703055-ebbb-470a-8d89-6c006fd76ce5"
 *
 *
 * @param name The name to associate with the address
 * @param addr The address to associate with the name
 *
 */
void register_resolve(std::map<std::string, std::string> name, Address addr);

/**
 * Unregisters a name -> address mapping.
 *
 * After unregister_resolve is called, any name that resolved to this addr, will not resolve to it
 * anymore
 *
 * @param addr The address to unregister.
 */
void unregister_resolve(Address addr);
} // namespace helpers
} // namespace seanerbus
