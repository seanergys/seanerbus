// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use seanerbus_schemas::message::MessageType;

use naming_scheme::ResolveExt;

use thiserror::Error;
#[derive(Error, Debug)]
pub enum ResolveError {
    #[error("resolve failed!")]
    ResolveFailure,
}
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();

    if !(args.len() - 1).is_multiple_of(2) || args.len() < 3 {
        println!("USAGE: ./request_test KEY1 VALUE1 [KEY2 VALUE2]...");
        return Ok(());
    }

    let mut key_map: Vec<(String, String)> = Vec::new();

    let mut i = 1;
    while i < args.len() {
        key_map.push((args[i].clone(), args[i + 1].clone()));
        i += 2;
    }

    let mut conn = Connection::new("localhost:5398").await?;

    let addrs = conn.resolve(key_map).await;

    for addr in addrs {
        println!("Subscribing to: {}", addr);
        conn.subscribe(addr).await?;
    }

    while let Some(msg) = conn.read_msg().await? {
        if msg.dp_type == MessageType::Publish {
            let payload_msg =
                seanerbus_msg::message::Message::from_capnp(msg.payload_type, &msg.payload)?;

            if let seanerbus_msg::message::Message::TimeValueV1(t, v) = payload_msg {
                println!("{}: time={}, value={}", msg.address, t, v)
            }
        }
    }
    Ok(())
}
