// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use crate::muxer::MuxMsg;

use log::debug;
use seanerbus_schemas::message::ConnectionId;
use seanerbus_schemas::message::{Address, Message, MessageType};
use std::collections::HashMap;
use tokio::sync::mpsc;
pub enum ReqResMsg {
    IncMsg(Message),
    Disconnect(ConnectionId),
    Shutdown,
}

pub async fn req_res(chans: BrokerChannels, mut rx: mpsc::Receiver<ReqResMsg>) {
    let mut providers = HashMap::<Address, ConnectionId>::new();

    while let Some(msg) = rx.recv().await {
        match msg {
            ReqResMsg::IncMsg(mut msg) => match msg.dp_type {
                MessageType::Request => {
                    if let Some(conn_id) = providers.get(&msg.address) {
                        msg.to = Some(*conn_id);
                        if chans
                            .to_mux
                            .send(MuxMsg::Forward(msg.clone()))
                            .await
                            .is_err()
                        {
                            debug!("Demuxer does not live anymore, shutting down");
                            chans.shutdown().await;
                            break;
                        }
                    } else {
                        debug!(
                            "{} requesting non-existing interface {}, sending error",
                            msg.from.unwrap(),
                            msg.address
                        );
                        if chans
                            .to_mux
                            .send(MuxMsg::Forward(Message::create_error(
                                msg.from.unwrap(),
                                msg.address,
                                seanerbus_schemas::msg_capnp::error_payload::ErrorKind::NoSuchInterface,
                                "requested interface does not exist!",
                            )))
                            .await
                            .is_err()
                        {
                            debug!("Demuxer does not live anymore, shutting down");
                            chans.shutdown().await;
                            break;
                        };
                    }
                }
                MessageType::Response => {
                    if chans.to_mux.send(MuxMsg::Forward(msg)).await.is_err() {
                        debug!("Demuxer does not live anymore, shutting down");
                        chans.shutdown().await;
                        break;
                    }
                }
                MessageType::Register => {
                    debug!(
                        "Registering {} as an endpoint for {} requests",
                        msg.from.unwrap(),
                        msg.address
                    );
                    providers.insert(msg.address, msg.from.unwrap());
                }
                _ => {
                    panic!(
                        "MessageType {:?} can not be handled by req/res!",
                        msg.dp_type
                    );
                }
            },
            ReqResMsg::Disconnect(conn_id) => {
                debug!(
                    "{} disconnected, removing it from req/res services",
                    conn_id
                );
                providers.retain(|_, v| *v != conn_id);
            }
            ReqResMsg::Shutdown => {
                debug!("Received shutdown signal, shutting down");
                break;
            }
        }
    }
}
