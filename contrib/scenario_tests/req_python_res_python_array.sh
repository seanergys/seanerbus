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

# identity function: f(x)->y with x=y
timeout 10s python3 response_test_array.py a366335b-52f0-4658-a42b-7e73e83ec91e &

RESPONSE_PID=$!

sleep 1

kill -0 $RESPONSE_PID || exit 1;

sleep 1

# request asks "[1., 2., 3.]" and should receive "[1., 2., 3.]"
timeout 10s python3 request_test_array.py a366335b-52f0-4658-a42b-7e73e83ec91e >> /tmp/log.txt &

REQUEST_PID=$!

sleep 1

EXPECTED_CONTENT="1.0, 2.0, 3.0"

grep -q "$EXPECTED_CONTENT" /tmp/log.txt || exit 1

echo "ok"

