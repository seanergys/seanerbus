// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use seanerbus_msg::message::Message;
use seanerbus_schemas::message::Address;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;
use uuid::Uuid;

#[derive(Serialize, Deserialize, Clone)]
struct Services {
    address: Uuid,
    name: HashMap<String, String>,
}

fn get_matching_addresses(name: Vec<(String, String)>, services: &[Services]) -> Vec<Uuid> {
    let mut services = services.to_owned();
    for attr in &name {
        services.retain(|s| {
            if let Some(v) = s.name.get(&attr.0) {
                if attr.1 == "*" { true } else { *v == attr.1 }
            } else {
                false
            }
        });
    }
    services.into_iter().map(|s| s.address).collect()
}

fn get_name_for_address(addr: Uuid, services: &[Services]) -> Vec<(String, String)> {
    for service in services {
        if service.address == addr {
            return service
                .name
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

    let mut services: Vec<Services> = serde_json::from_reader(file)?;

    let mut conn = Connection::new("localhost:5398").await?;

    conn.register(Address(Uuid::from_u64_pair(0, 0x10ca105)))
        .await?;

    while let Some(msg) = conn.read_msg().await? {
        match msg.dp_type {
            seanerbus_schemas::message::MessageType::Request => {
                let naming_msg = Message::from_capnp(msg.payload_type, &msg.payload)?;

                match naming_msg {
                    Message::ResolveReqV1(name) => {
                        println!("Resolving: {:?}", name);

                        let uuids = get_matching_addresses(name, &services);

                        println!("Resolved To: {:?}", uuids);
                        let (payload_type, payload) = Message::ResolveResV1(uuids).to_capnp()?;
                        conn.respond_to(&msg, payload_type, &payload).await?;
                    }
                    Message::ReverseResolveReqV1(uuid) => {
                        let (payload_type, payload) =
                            Message::ReverseResolveResV1(get_name_for_address(uuid, &services))
                                .to_capnp()?;
                        conn.respond_to(&msg, payload_type, &payload).await?;
                    }
                    Message::RegisterResolveReqV1(name, addr) => {
                        if services.iter().any(|service| service.address == addr) {
                            println!("Can not register {}, already registered!", addr);
                            let (payload_type, payload) = Message::RegisterResolveResV1(
                                false,
                                "Service already exists!".to_string(),
                            )
                            .to_capnp()?;

                            conn.respond_to(&msg, payload_type, &payload).await?;
                        } else {
                            println!("Registered {:?} to {}", name, addr);
                            services.push(Services {
                                address: addr,
                                name: name.into_iter().collect::<HashMap<String, String>>(),
                            });
                            let (payload_type, payload) =
                                Message::RegisterResolveResV1(true, String::new()).to_capnp()?;

                            conn.respond_to(&msg, payload_type, &payload).await?;
                        }
                    }
                    Message::UnregisterResolveReqV1(addr) => {
                        if services.iter().any(|service| service.address == addr) {
                            services.retain(|service| service.address != addr);

                            println!("Deregistering {}", addr);
                            let (payload_type, payload) =
                                Message::UnregisterResolveResV1(true, String::new()).to_capnp()?;
                            conn.respond_to(&msg, payload_type, &payload).await?;
                        } else {
                            println!("Can not deregister {}, does not exist!", addr);
                            let (payload_type, payload) = Message::UnregisterResolveResV1(
                                false,
                                "No service under that address to deregister!".to_string(),
                            )
                            .to_capnp()?;

                            conn.respond_to(&msg, payload_type, &payload).await?;
                        }
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
