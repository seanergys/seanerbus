#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

set -euo pipefail

# Generate C++ client library documentation
cd ../bindings/cpp
doxygen
cd ../../docs
# If somehow this folder exists before generating the doc
# the sphinx-rustdocgen tends to die randomly.
rm -rf docs/crates
uv run sphinx-build sphinx ../public
