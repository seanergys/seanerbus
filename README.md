# 🐑 SeanerBUS -- Prototype Implementation of the Data Plane

This is a prototype implementation of the core transport facilities of the Data-Plane.

The purpose of this project is to provide an easy to use, complete and correct implementation of the Data-Plane to the current state discussed in the project to enable rapid design and development of further SEANERGYS components.

**THIS IMPLEMENTATION IS NOT INTENDED FOR PRODUCTION USE**

At no time, any thought was spent on optimizing this implementation for use with massively parallel HPC systems.


## Components of the SeanerBUS

### `server/`

The server/broker component. Will listen on `localhost:5398` for incoming connections.

### `msg/`

Contains the Cap'n'Proto schemas for the Data-Plane messages to be communicated over the Data-Plane for purposes such as requesting name-resolution and publishing TimeSeries data on the Data-Plane.

This is accompanied by Rust and C++ convenience libraries for creating those messages.

### `client/`

C++ and Rust client libraries for the Data-Plane. Contains additional helpful examples for creating pub/sub and req/res components.

### `naming_scheme/`

A prototype of the naming scheme proposed for WP2.

### Data-Plane internal components

#### `async/`

Rust async [tokio](tokio.rs) wrappers around the underlying TCP connection that produce and consume Data-Plane messages.

Shared code between the client and server libraries.

#### `internal_schemas/`

Internal Cap'n'Proto definitions for the request/response and publish/subscribe messages.
