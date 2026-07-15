#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

set -euo pipefail

uv cache clean
rm -rf .venv

# Generate C++ client library documentation
cd ../bindings/cpp
doxygen
cd ../../docs
# If somehow this folder exists before generating the doc
# the sphinx-rustdocgen tends to die randomly.
rm -rf docs/crates
uv run sphinx-build sphinx ../public
rm -rf ../public/rust_msg
rm -rf ../public/rust_client
rm -rf ../public/rust_naming
cd ../bindings/rust/msg
cargo doc -p seanerbus_msg --no-deps --target-dir ../../../public/rust_msg
cd ../client
cargo doc -p seanerbus_client  --no-deps --target-dir ../../public/rust_client
cd ../../../naming_scheme
cargo doc -p naming_scheme --no-deps --target-dir ../public/rust_naming
cd ../../
rm -rf public/rust_msg/debug
rm -rf public/rust_client/debug
rm -rf public/rust_naming_debug
