<!--
SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>

SPDX-License-Identifier: MIT
-->

# 🐑 SeanerBUS -- Data-Plane Integration Repository.

This is the integration repository for all components of the data-plane.

Most of the SeanerBUS lives in their own repositories:

- Bindings
    - [C++ Bindings](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-cpp)
    - [Python Bindings](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-py)
    - [Rust Bindings](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-rs)
- [SeanerBUS MetricQ-HTA Time-Series DB](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-hta)
- [SeanerBUS Command Line Tool](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-cli)
- [SeanerBUS Message Standard](https://gitlab.seanergys.fz-juelich.de/software/seanerbus-msg)

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
# If you want to bind to another address than localhost:5398
cargo run '0.0.0.0:9000'
```

### `bindings/`

Git submodules of the latest releases of the Python/Rust/C++ bindings.

### `naming_scheme/`

A prototype of the naming scheme proposed for WP2.

### `docs/`

SeanerBUS documentation.

### `cli/`

A command-line interface for sending SeanerBUS message from the shell.

### `scenario_tests/`

End-to-end tests for talking from language A to language B via the seanerBUS
