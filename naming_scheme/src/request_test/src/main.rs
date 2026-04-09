// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use std::collections::HashMap;

use seanerbus_client::Connection;
use seanerbus_msg::message::Message;
use seanerbus_schemas::message::{Address, MessageType};
use uuid::Uuid;

use thiserror::Error;
#[derive(Error, Debug)]
pub enum ResolveError {
    #[error("resolve failed!")]
    ResolveFailure,
}
async fn resolve(
    conn: &mut Connection,
    attrs: HashMap<String, String>,
) -> Result<Vec<Uuid>, Box<dyn std::error::Error>> {
    let attrs = attrs.into_iter().collect::<Vec<(String, String)>>();

    let (payload_type, payload) = Message::NamingServiceRequestV1(attrs).to_capnp()?;

    let msg = conn
        .request(
            Address(Uuid::from_u64_pair(0, 0x10ca105)),
            payload_type,
            &payload,
        )
        .await?;

    if let seanerbus_msg::message::Message::NamingServiceResponseV1(uuids) =
        seanerbus_msg::message::Message::from_capnp(msg.payload_type, &msg.payload)?
    {
        Ok(uuids)
    } else {
        Err(Box::<ResolveError>::new(ResolveError::ResolveFailure))
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();

    if !(args.len() - 1).is_multiple_of(2) || args.len() < 3 {
        println!("USAGE: ./request_test KEY1 VALUE1 [KEY2 VALUE2]...");
        return Ok(());
    }

    let mut key_map: HashMap<String, String> = HashMap::new();

    let mut i = 1;
    while i < args.len() {
        key_map.insert(args[i].clone(), args[i + 1].clone());
        i += 2;
    }

    let mut conn = Connection::new("localhost:5398").await?;

    let uuids = resolve(&mut conn, key_map).await?;

    for uuid in uuids {
        println!("Subscribing to: {}", uuid);
        conn.subscribe(Address(uuid)).await?;
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
