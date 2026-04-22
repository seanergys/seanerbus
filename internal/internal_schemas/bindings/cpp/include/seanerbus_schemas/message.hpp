// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <optional>
#include <vector>
#include <schemas/msg.capnp.h>

namespace seanerbus
{
struct Uuid
{
    uint64_t upper;
    uint64_t lower;

    friend bool operator<(const Uuid& lhs, const Uuid& rhs)
    {
        if (lhs.upper == rhs.upper)
        {
            return lhs.lower < rhs.lower;
        }

        return lhs.upper < rhs.lower;
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

struct Payload
{
    uint64_t payload_type;
    std::vector<std::byte> payload;
};
class Message
{
    public:
        explicit Message() : address({0,0})
        {}

        static Message subscribe(Address addr);
        static Message publish(Address addr, uint64_t payload_type, std::vector<std::byte> payload);

        ::Message::MessageType type;
        std::optional<ConnectionId> to = std::nullopt;
        std::optional<ConnectionId> from = std::nullopt;
        Address address = Address(Uuid{0,0});
        struct Payload payload = Payload{0, std::vector<std::byte>()};

        static Message from_fd_capnp(int fd);

        void to_fd_capnp(int fd);
};
}
