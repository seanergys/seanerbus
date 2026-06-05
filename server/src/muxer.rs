// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::writer::SenderMsg;
use seanerbus_schemas::message::{ConnectionId, Message};
use std::collections::HashMap;
use tokio::sync::mpsc;

use log::debug;

pub enum MuxMsg {
    Disconnect(ConnectionId),
    BackChannel(ConnectionId, mpsc::Sender<SenderMsg>),
    Forward(Message),
    Shutdown,
}

pub async fn mux(mut rx: mpsc::Receiver<MuxMsg>) {
    let mut back_channels: HashMap<ConnectionId, mpsc::Sender<SenderMsg>> = HashMap::new();
    loop {
        match rx.recv().await {
            Some(msg) => match msg {
                MuxMsg::BackChannel(conn_id, sender) => {
                    debug!("adding back channel for demuxer for {}!", conn_id);
                    back_channels.insert(conn_id, sender);
                }
                MuxMsg::Disconnect(conn_id) => {
                    debug!("{} disconnected, removing it from demuxer", conn_id);
                    back_channels.remove(&conn_id);
                }
                MuxMsg::Forward(msg) => {
                    debug!("Got message to forward to {}", msg.to.unwrap());
                    match back_channels.get(&msg.to.unwrap()) {
                        Some(rx) => {
                            if rx.send(SenderMsg::OutMsg(msg.clone())).await.is_err() {
                                debug!("Sender thread for {} died, removing it", msg.to.unwrap());
                                back_channels.remove(&msg.to.unwrap());
                            }
                        }
                        None => {
                            debug!(
                                "Got message for unknown UUID '{}', dropping!",
                                msg.to.unwrap()
                            );
                        }
                    };
                }
                MuxMsg::Shutdown => {
                    debug!("Received shutdown, shutting down");
                    break;
                }
            },
            None => {
                panic!("Multiplexer received close of incoming message channels! This is a bug!");
            }
        }
    }
}
