// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <cstdint>

#include <fmt/format.h>
#include <schemas/internal.capnp.h>

namespace seanerbus
{
struct Uuid
{
    uint64_t upper;
    uint64_t lower;

    Uuid(uint64_t upper, uint64_t lower) : upper(upper), lower(lower)
    {
    }

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

    explicit operator std::string() const
    {
        return fmt::format("{:08x}-{:04x}-{:04x}-{:04x}-{:012x}", upper >> 32,
                           (upper >> 16) & 0xffff, upper & 0xffff, lower >> 48,
                           lower & 0x0000'ffff'ffff'ffff);
    }

    friend bool operator<(const Uuid& lhs, const Uuid& rhs)
    {
        if (lhs.upper == rhs.upper)
        {
            return lhs.lower < rhs.lower;
        }

        return lhs.upper < rhs.lower;
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

struct Address
{
    explicit Address(Uuid u) : u_(u)
    {
    }
    Uuid u_;

    friend bool operator<(const Address&lhs, const Address&rhs)
    {
        return lhs.u_ < rhs.u_;
    }
};

class Message
{
    public:
        explicit Message() : address({0,0})
        {}

        static Message subscribe(Address addr);
        static Message publish(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload);
        static Message request(Address addr, std::pair<uint64_t, std::vector<std::byte>> payload);
        static Message response(Message msg, std::pair<uint64_t, std::vector<std::byte>> payload);
        static Message register_endpoint(Address addr);

        ::Message::MessageType type;
        uint64_t matching_id;
        std::optional<ConnectionId> to = std::nullopt;
        std::optional<ConnectionId> from = std::nullopt;
        Address address = Address(Uuid{0,0});
        std::pair<uint64_t, std::vector<std::byte>> payload =
            std::pair{ 0, std::vector<std::byte>() };

        static std::pair<size_t, Message> from_buf_capnp(kj::ArrayPtr<capnp::word> ptr);
        static Message from_msg(::Message::Reader& rd);
        void to_fd_capnp(int fd);
};
}
