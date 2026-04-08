// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use seanerbus_schemas::message::Address;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

#[derive(Serialize, Deserialize, Clone)]
struct Services {
    uuid: Uuid,
    attrs: HashMap<String, String>,
}

fn get_matching_addresses(attrs: Vec<(String, String)>, services: &Vec<Services>) -> Vec<Uuid> {
    let mut services = services.clone();
    for attr in &attrs {
        services = services
            .into_iter()
            .filter(|s| {
                if let Some(v) = s.attrs.get(&attr.0) {
                    if attr.1 == "*" { true } else { *v == attr.1 }
                } else {
                    false
                }
            })
            .collect();
    }
    services.into_iter().map(|s| s.uuid).collect()
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let file = std::fs::File::open("services.json")?;

    let services: Vec<Services> = serde_json::from_reader(file)?;

    let mut conn = Connection::new("localhost:5398").await?;

    conn.register(Address(Uuid::from_u64_pair(0, 0x10ca105)))
        .await?;

    while let Some(msg) = conn.read_msg().await? {
        match msg.dp_type {
            seanerbus_schemas::message::MessageType::Request => {
                let naming_msg =
                    seanerbus_msg::message::Message::from_capnp(msg.payload_type, &msg.payload)?;

                if let seanerbus_msg::message::Message::NamingServiceRequestV1(attrs) = naming_msg {
                    println!("Resolving: {:?}", attrs);

                    let uuids = get_matching_addresses(attrs, &services);

                    println!("Resolved To: {:?}", uuids);
                    let (payload_type, payload) =
                        seanerbus_msg::message::Message::NamingServiceResponseV1(
                            uuids,
                        )
                        .to_capnp()?;
                    conn.respond_to(&msg, payload_type, &payload).await?;
                } else {
                    panic!(
                        "Got request of type {}  although NamingServiceRequestV1 was expected!",
                        msg.payload_type
                    );
                }
            }
            _ => panic!(
                "Got message of type {:?}, while expecting Request!",
                msg.dp_type
            ),
        }
    }
    Ok(())
}
