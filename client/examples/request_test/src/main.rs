// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

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

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();

    if args.len() != 2 {
        println!("USAGE: ./request_test UUID");
        return Ok(());
    }

    let mut conn = Connection::new("localhost:5398").await?;

    let (payload_type, payload) = Message::TimeValueV1(0, 0).to_capnp().unwrap();

    let msg = conn.request(Address(Uuid::parse_str(&args[1]).unwrap()), payload_type, &payload).await?;

    if msg.dp_type == MessageType::Response
    {
        if let Ok(Message::TimeValueV1(t, v)) = Message::from_capnp(msg.payload_type, &msg.payload)
        {
            println!("Got response! time={}, value={}", t, v);
        }
        else
        {
            panic!("Got something else than a Message::TimeValueV1 as a response!");
        }
    }
    else
    {
        panic!("Expected Response to Request, got {:?}", msg.dp_type);
    }
    Ok(())
}
