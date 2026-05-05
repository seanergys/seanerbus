# SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
#
# SPDX-License-Identifier: MIT

cd ../server
cargo run &
sleep 5
cd ../cpp_examples/build
./publish_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f &
./subscribe_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f
