# Prototype Implementation of the Naming Scheme

This is a prototype implementation of the naming scheme presented in the WP2 meeting.

This is an ensemble of three applications:

## `request_test`

```sh
cargo run request_test node 1 system \*
```

This application is a subscriber for time-series data.

It takes pairs of key=value as command line arguments (In the above example, `node=1` and `system=*`).
It will send those key=value pairs to the naming server, which will look up if there are any communication partners matching the
given attributes.

`request_test` then subscribes to all of the Data-Plane addresses returned by the naming server. Printing any values it receives to stdout.


