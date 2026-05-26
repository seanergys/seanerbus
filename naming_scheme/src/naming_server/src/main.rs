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

fn get_matching_addresses(attrs: Vec<(String, String)>, services: &[Services]) -> Vec<Uuid> {
    let mut services = services.to_owned();
    for attr in &attrs {
        services.retain(|s| {
            if let Some(v) = s.attrs.get(&attr.0) {
                if attr.1 == "*" { true } else { *v == attr.1 }
            } else {
                false
            }
        });
    }
    services.into_iter().map(|s| s.uuid).collect()
}

fn get_name_for_address(addr: Uuid, services: &[Services]) -> Vec<(String, String)> {
    for service in services {
        if service.uuid == addr {
            return service
                .attrs
                .clone()
                .into_iter()
                .collect::<Vec<(String, String)>>();
        }
    }

    Vec::new()
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();
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

                match naming_msg {
                    seanerbus_msg::message::Message::ResolveReqV1(attrs) => {
                        println!("Resolving: {:?}", attrs);

                        let uuids = get_matching_addresses(attrs, &services);

                        println!("Resolved To: {:?}", uuids);
                        let (payload_type, payload) =
                            seanerbus_msg::message::Message::ResolveResV1(uuids).to_capnp()?;
                        conn.respond_to(&msg, payload_type, &payload).await?;
                    }
                    seanerbus_msg::message::Message::ReverseResolveReqV1(uuid) => {
                        let (payload_type, payload) =
                            seanerbus_msg::message::Message::ReverseResolveResV1(
                                get_name_for_address(uuid, &services),
                            )
                            .to_capnp()?;
                        conn.respond_to(&msg, payload_type, &payload).await?;
                    }
                    _ => {
                        panic!(
                            "Got request of type {}  although NamingServiceReqV1 was expected!",
                            msg.payload_type
                        );
                    }
                }
            }
            _ => panic!(
                "Got message of type {:?}, while expecting Req!",
                msg.dp_type
            ),
        }
    }
    Ok(())
}
