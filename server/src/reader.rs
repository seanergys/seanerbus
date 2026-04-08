// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use crate::demuxer::DemuxMsg;
use seanerbus_async::reader::MessageReader;
use seanerbus_schemas::message::ConnectionId;
use thiserror::Error;
use tokio::io::AsyncRead;

#[derive(Error, Debug)]
pub enum ReaderError {}

pub async fn read<Reader>(
    conn_id: ConnectionId,
    reader: Reader,
    broker: BrokerChannels,
) -> Result<(), ReaderError>
where
    Reader: AsyncRead + Unpin + Send + 'static,
{
    let mut conn = MessageReader::new(reader);

    loop {
        match conn.read_message().await {
            Ok(opt) => match opt {
                Some(mut msg) => {
                    tracing::debug!("{:?}", msg);
                    msg.from = Some(conn_id);
                    broker.to_demux.send(DemuxMsg::IncMsg(msg)).await.unwrap();
                }
                None => {
                    break;
                }
            },
            Err(e) => {
                panic!("Reading message yielded error: {}", e);
            }
        }
    }

    broker.disconnect(conn_id).await;
    Ok(())
}
