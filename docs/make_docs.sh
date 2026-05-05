#!/usr/bin/env bash

set -euo pipefail

# Generate C++ client library documentation
cd ../client/bindings/cpp
doxygen
cd ../../../docs
# Generate C++ message library documentation
cd ../msg/bindings/cpp
doxygen
# Gemerate C++ line protocol documentation
cd ../../../docs
cd ../internal/internal_schemas/bindings/cpp
doxygen
cd ../../../../docs
# If somehow this folder exists before generating the doc
# the sphinx-rustdocgen tends to die randomly.
rm -rf docs/crates
uv run sphinx-build sphinx ../public
