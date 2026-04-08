// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use uuid::Uuid;

// Publishes TimeValueV1 messages with increasing times and values
// to the address given on the command line
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = std::env::args().collect();

    if args.len() != 2 {
        println!("USAGE: ./publish_test UUID");
        return Ok(());
    }

    let mut conn = Connection::new("localhost:5398").await?;

    let mut time = 0;
    let mut value = 0;

    loop {
        time += 1;
        value += 1;

        println!("Publishing time={}, value={}", time, value);

        let (payload_type, payload) =
            seanerbus_msg::message::Message::TimeValueV1(time, value).to_capnp()?;
        conn.publish(
            seanerbus_schemas::message::Address(Uuid::parse_str(&args[1]).unwrap()),
            payload_type,
            &payload,
        )
        .await?;
        tokio::time::sleep(core::time::Duration::new(1, 0)).await
    }
}
