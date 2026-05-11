#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

set -euo pipefail

cd ../../server
cargo build
./target/debug/seanerbus &
SERVER_PID=$!

echo "server PID is $SERVER_PID"

sleep 1

cd ../contrib/scenario_tests
timeout 10s ./publisher_test.py &
PUBLISHER_PID=$!

cd build
sleep 5
./scenario_publisher

wait $PUBLISHER_PID

sleep 5
kill -int $SERVER_PID
sleep 5
