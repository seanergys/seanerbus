// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use crate::muxer::MuxMsg;
use log::debug;
use seanerbus_schemas::message::{Address, ConnectionId, Message, MessageType};
use std::collections::{HashMap, HashSet};
use tokio::sync::mpsc;

pub enum PubSubMsg {
    IncMsg(Message),
    Disconnect(ConnectionId),
    Shutdown,
}

pub async fn pub_sub(chans: BrokerChannels, mut rx: mpsc::Receiver<PubSubMsg>) {
    let mut subscribers = HashMap::<Address, HashSet<ConnectionId>>::new();
    while let Some(msg) = rx.recv().await {
        match msg {
            PubSubMsg::IncMsg(msg) => match msg.dp_type {
                MessageType::Subscribe => {
                    debug!("Subscribing {} to {}", msg.from.unwrap(), msg.address);

                    subscribers.entry(msg.address).or_default();

                    subscribers
                        .get_mut(&msg.address)
                        .unwrap()
                        .insert(msg.from.unwrap());
                }
                MessageType::Publish => {
                    debug!(
                        "Got publish event for {} from {}",
                        msg.address,
                        msg.from.unwrap()
                    );

                    if let Some(subs) = subscribers.get_mut(&msg.address) {
                        let mut to_delete: Vec<ConnectionId> = vec![];
                        debug!("Forwarding to: {:?}", subs);
                        for sub in subs.iter() {
                            debug!("forwarding message to reader {}", sub);
                            let mut msg = msg.clone();
                            msg.to = Some(*sub);
                            if chans.to_mux.send(MuxMsg::Forward(msg)).await.is_err() {
                                debug!(
                                    "{} suddenly disconnected during forward of publish! disconnecting",
                                    sub
                                );
                                to_delete.push(*sub);
                            }
                        }

                        for del in to_delete {
                            subs.remove(&del);
                        }
                    }
                }
                _ => {
                    panic!(
                        "MessageType {:?} can not be handled by pub/sub!",
                        msg.dp_type
                    );
                }
            },
            PubSubMsg::Disconnect(conn_id) => {
                debug!("{} disconnected, removing it from subscribers", conn_id);
                for (_, subs) in subscribers.iter_mut() {
                    subs.remove(&conn_id);
                }
            }
            PubSubMsg::Shutdown => {
                debug!("Received shutdown signal, shutting down");
                break;
            }
        }
    }
}
