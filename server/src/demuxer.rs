// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use crate::pubsub::PubSubMsg;
use crate::reqres::ReqResMsg;
use log::debug;
use seanerbus_schemas::message::{Message, MessageType};
use tokio::sync::mpsc;

pub enum DemuxMsg {
    IncMsg(Message),
    Shutdown,
}

pub async fn demux(chan: BrokerChannels, mut rx: mpsc::Receiver<DemuxMsg>) {
    while let Some(msg) = rx.recv().await {
        match msg {
            DemuxMsg::IncMsg(msg) => match msg.dp_type {
                MessageType::Publish | MessageType::Subscribe => {
                    if let Err(e) = chan.to_pubsub.send(PubSubMsg::IncMsg(msg)).await {
                        debug!(
                            "pub/sub service died (Reason: {}), following it to Valhalla",
                            e
                        );
                        break;
                    }
                }
                MessageType::Request | MessageType::Register | MessageType::Response => {
                    if let Err(e) = chan.to_reqres.send(ReqResMsg::IncMsg(msg)).await {
                        debug!(
                            "pub/sub service died (Reason: {}), following it to Valhalla",
                            e
                        );
                        break;
                    }
                }
                MessageType::Error => {
                    todo!("Error handling not implemented yet");
                }
            },
            DemuxMsg::Shutdown => {
                debug!("Demuxer received shutdown, closing down!");
                break;
            }
        }
    }
}
