# 🐑 SeanerBUS server -- Prototype implementation of the Data-Plane

This is a prototype implementation of the core data-plane services.

The purpose of this prototype implementation is to provide an easy to use, complete and correct (to the current state of discussions in this project) implementation
of the Data-Plane that can be used for the purpose of rapid prototyping.

This implementation of the Data-Plane is not meant for production use. No 

It should facilitate sending and receiving data with the pub/sub and req

## Usage

```sh
cargo run
``` 

will start a SeanerBUS server.


```sh
RUST_LOG=debug cargo run
```

will start a SeanerBUS server with extra debugging information.


