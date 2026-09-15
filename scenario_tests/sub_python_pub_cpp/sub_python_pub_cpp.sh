#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

set -euo pipefail

cd ../../server
cargo build
./target/debug/demobus &
SERVER_PID=$!

echo "server PID is $SERVER_PID"

sleep 1

cd ../scenario_tests/sub_python_pub_cpp

uv venv --clear --force
source .venv/bin/activate

uv pip install -r pyproject.toml

python publisher_test.py &
PUBLISHER_PID=$!

cmake .
make -j4
./scenario_publisher

wait $PUBLISHER_PID

sleep 5
kill -int $SERVER_PID
sleep 5

echo "Subscribing with Python and publishing with C++ works!"
