// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT
#pragma once

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <cstdint>

#include <fmt/format.h>
#include <seanerbus/internal/internal.capnp.h>

namespace seanerbus
{
/**
 * A UUID, per RFC9562
 */
struct Uuid
{
    uint64_t upper;
    uint64_t lower;

    /**
     * Create a uuid from two 64-bit integers
     *
     * @param upper The upper 64 bit of the UUID
     * @param lower The lower 64 bit of the UUID
     */
    Uuid(uint64_t upper, uint64_t lower) : upper(upper), lower(lower)
    {
    }

    /**
     * Create a uuid from a formatted string
     *
     * This string has to have the format:
     *   ec80dfdd-3f7e-44c4-ab59-7347195a27a5
     *
     * @param s The string to convert from
     */
    explicit Uuid(std::string_view s)
    {
        // expects format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx (32 hex digits + 4 dashes)
        if (s.size() != 36 || s[8] != '-' || s[13] != '-' || s[18] != '-' || s[23] != '-')
            throw std::invalid_argument("invalid UUID string");

        // upper = time_low (32) + time_mid (16) + time_hi (16)
        upper = (parse_hex(s.substr(0, 8)) << 32) | (parse_hex(s.substr(9, 4)) << 16) |
                parse_hex(s.substr(14, 4));

        // lower = clock_seq (16) + node (48)
        lower = (parse_hex(s.substr(19, 4)) << 48) | parse_hex(s.substr(24, 12));
    }

    /**
     * Convert UUID to string
     */
    std::string as_string() const
    {
        return fmt::format("{:08x}-{:04x}-{:04x}-{:04x}-{:012x}", upper >> 32,
                           (upper >> 16) & 0xffff, upper & 0xffff, lower >> 48,
                           lower & 0x0000'ffff'ffff'ffff);
    }

    friend std::ostream& operator<<(std::ostream& lhs, const Uuid& rhs)
    {
        return lhs << rhs.as_string();
    }

    friend bool operator<(const Uuid& lhs, const Uuid& rhs)
    {
        if (lhs.upper == rhs.upper)
        {
            return lhs.lower < rhs.lower;
        }

        return lhs.upper < rhs.lower;
    }

    friend bool operator==(const Uuid& lhs, const Uuid& rhs)
    {
        return lhs.lower == rhs.lower && lhs.upper == rhs.upper;
    }

private:
    static uint64_t parse_hex(std::string_view part)
    {
        uint64_t val = 0;
        for (char c : part)
        {
            val <<= 4;
            if (c >= '0' && c <= '9')
                val |= c - '0';
            else if (c >= 'a' && c <= 'f')
                val |= c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                val |= c - 'A' + 10;
            else
                throw std::invalid_argument("invalid UUID character");
        }
        return val;
    }
};

struct ConnectionId
{
    explicit ConnectionId(Uuid u) : u_(u)
    {
    }

    Uuid u_;
};

/**
 * A SeanerBUS address.
 *
 * SeanerBUS addresses are currently all UUIDv4s:
 */
struct Address
{
    /**
     * Create an address from the given UUID
     *
     * @param u The UUID
     */
    explicit Address(Uuid u) : u_(u)
    {
    }

    static Address locator()
    {
        return Address{ { 0, 0x10ca105 } };
    }

    Uuid u_;

    friend bool operator<(const Address& lhs, const Address& rhs)
    {
        return lhs.u_ < rhs.u_;
    }

    friend bool operator==(const Address& lhs, const Address& rhs)
    {
        return lhs.u_ == rhs.u_;
    }
};

class Message
{
public:
    explicit Message() : address({ 0, 0 })
    {
    }

    static Message subscribe(Address addr);
    static Message publish(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload);
    static Message request(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload);
    static Message response(Message msg, std::pair<uint64_t, std::vector<std::byte>> payload);
    static Message register_endpoint(Address addr);

    ::Message::MessageType type;
    uint64_t matching_id;
    std::optional<ConnectionId> to = std::nullopt;
    std::optional<ConnectionId> from = std::nullopt;
    Address address = Address(Uuid{ 0, 0 });
    std::pair<uint64_t, std::vector<std::byte>> payload = std::pair{ 0, std::vector<std::byte>() };

    static std::pair<size_t, Message> from_buf_capnp(kj::ArrayPtr<capnp::word> ptr);
    static Message from_msg(::Message::Reader& rd);
    void to_fd_capnp(int fd);
};
} // namespace seanerbus
