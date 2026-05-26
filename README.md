<!--
SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>

SPDX-License-Identifier: MIT
-->

# 🐑 SeanerBUS -- Prototype Implementation of the Data Plane

This is a prototype implementation of the core transport facilities of the Data-Plane.

The purpose of this project is to provide an easy to use, complete and correct implementation of the Data-Plane to the current state discussed in the project to enable rapid design and development of further SEANERGYS components.

## Components of the SeanerBUS

### `server/`

The server/broker component. 

If Rust is installed, it can be started with:

```
rustup default stable # if Rust is not installed another way
sudo apt install capnproto # Package containing the `capnp` tool
cd server
cargo run
# If more debug info is wanted:
RUST_LOG=trace cargo run
```

It will listen on `localhost:5398` for incoming connections.

### `(rust|cpp/python)_examples/`

Examples using the Rust/(C/C++) client library for:

- publishing time-series data
- subscribing to time-series data
- requesting a "Hello World" request
- responding to a "Hello World" request

Try them out, any combination of languages works together:

```sh
# First Shell
cd server
cargo run
# Second Shell:
cd rust_examples
cargo run --bin publish_test  a366335b-52f0-4658-a42b-7e73e83ec91f
# Third Shell:
cd cpp_examples/build
./subscribe_test a366335b-52f0-4658-a42b-7e73e83ec91f

```

```sh
# First Shell
cd server
cargo run
# Second Shell:
cd rust_examples
cargo run --bin response_test  a366335b-52f0-4658-a42b-7e73e83ec91f
# Third Shell (from seanerbus root, with .venv activated — see python_examples/README.md):
python3 python_examples/src/request_test.py  a366335b-52f0-4658-a42b-7e73e83ec91f

```
### `bindings/python`

This contains the Python bindings for the SeanerBUS.

### `bindings/cpp`

This contains the C++ bindings to the SeanerBUS.

### `naming_scheme/`

A prototype of the naming scheme proposed for WP2.

### `msg/`

Contains the Cap'n'Proto schemas for the Data-Plane messages to be communicated over the Data-Plane for purposes such as requesting name-resolution and publishing TimeSeries data on the Data-Plane.

### `client/`

C++ and Rust client libraries for `req/res` and `pub/sub` with the server.

### `internal`

Internal shared code between the server and client(s)

### `cli/`

A command-line interface for sending SeanerBUS message from the shell.

### `contrib`

Various scripts, usually tests.
