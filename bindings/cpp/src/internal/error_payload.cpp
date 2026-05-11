// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

#include <seanerbus/internal/error_payload.hpp>

#include <capnp/message.h>
#include <capnp/serialize.h>
#include <seanerbus/internal/internal.capnp.h>

namespace seanerbus
{

ErrorPayload ErrorPayload::from_vec(std::vector<std::byte> vec)
{
    capnp::MallocMessageBuilder bld;
    capnp::initMessageBuilderFromFlatArrayCopy(
        kj::ArrayPtr<capnp::word>((capnp::word*)vec.data(), vec.size()), bld);

    ::ErrorPayload::Reader rd = bld.getRoot<::ErrorPayload>();

    ErrorPayload err;

    err.kind = rd.getErrorKind();
    err.str = rd.getErrorStr().cStr();

    return err;
}

std::vector<std::byte> ErrorPayload::to_vec()
{
    capnp::MallocMessageBuilder bld;

    ::ErrorPayload::Builder err = bld.initRoot<::ErrorPayload>();

    err.setErrorKind(kind);
    err.setErrorStr(capnp::Text::Reader(str));

    auto res = capnp::messageToFlatArray(bld);

    return std::vector((std::byte*)res.begin(), (std::byte*)res.end());
}
} // namespace seanerbus
