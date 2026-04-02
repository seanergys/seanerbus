// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use log::debug;
use seanerbus_async::writer::MessageWriter;
use seanerbus_schemas::message::ConnectionId;
use seanerbus_schemas::message::Message;
use thiserror::Error;
use tokio::io::AsyncWrite;
use tokio::sync::mpsc;

pub enum SenderMsg {
    OutMsg(Message),
}

#[derive(Error, Debug)]
pub enum WriterError {}

pub async fn write<Writer>(
    conn_id: ConnectionId,
    writer: Writer,
    broker: BrokerChannels,
) -> Result<(), WriterError>
where
    Writer: AsyncWrite + Unpin + Send + 'static,
{
    let (tx, mut rx) = mpsc::channel::<SenderMsg>(1);

    broker.register_sender(conn_id, tx).await;
    let mut msg_write = MessageWriter::new(writer);
    while let Some(e) = rx.recv().await {
        match e {
            SenderMsg::OutMsg(msg) => {
                debug!("Sending message to {}", conn_id);
                debug!("{:?}", msg);
                if let Err(e) = msg_write.write_message(&msg).await {
                    debug!("Could not transmit message to: {}: {}", conn_id, e);
                    break;
                }
            }
        }
    }

    Ok(())
}
