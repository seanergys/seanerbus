// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::broker::BrokerChannels;
use crate::demuxer::DemuxMsg;
use seanerbus_async::error::ConnectionError;
use seanerbus_async::reader::MessageReader;
use seanerbus_schemas::message::ConnectionId;
use thiserror::Error;
use tokio::io::AsyncRead;
use tokio::sync::mpsc::error::SendError;

#[derive(Error, Debug)]
pub enum ReaderError {
    #[error("Reading a message failed: {0}")]
    ReadFailed(#[from] ConnectionError),
    #[error("Failed to communicate to other SeanerBUS components: {0}")]
    CommFailed(#[from] SendError<DemuxMsg>),
}

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
                    broker.to_demux.send(DemuxMsg::IncMsg(msg)).await?;
                }
                None => {
                    break;
                }
            },
            Err(e) => {
                broker.disconnect(conn_id).await;
                return Err(e.into());
            }
        }
    }

    broker.disconnect(conn_id).await;
    Ok(())
}

#[cfg(test)]
mod tests {
    use seanerbus_schemas::message::Message;
    use uuid::Uuid;

    use super::*;
    use crate::demuxer::DemuxMsg;
    use crate::muxer::MuxMsg;
    use crate::pubsub::PubSubMsg;
    use crate::reqres::ReqResMsg;

    #[tokio::test]
    async fn test_good() {
        let id = ConnectionId(Uuid::new_v4());

        let (to_mux_tx, mut to_mux_rx) = tokio::sync::mpsc::channel::<MuxMsg>(1);
        let (to_demux_tx, mut to_demux_rx) = tokio::sync::mpsc::channel::<DemuxMsg>(1);
        let (to_pubsub_tx, mut to_pubsub_rx) = tokio::sync::mpsc::channel::<PubSubMsg>(1);
        let (to_reqres_tx, mut to_reqres_rx) = tokio::sync::mpsc::channel::<ReqResMsg>(1);
        let chans = BrokerChannels {
            to_mux: to_mux_tx,
            to_demux: to_demux_tx,
            to_pubsub: to_pubsub_tx,
            to_reqres: to_reqres_tx,
        };

        let rd = tokio_test::io::Builder::new()
            .read(
                &Message::publish_msg(
                    seanerbus_schemas::message::Address(Uuid::new_v4()),
                    0,
                    &vec![],
                )
                .to_capnp()
                .unwrap(),
            )
            .build();

        read(id, rd, chans)
            .await
            .expect("Could transfer correctly!");

        assert_eq!(to_mux_rx.len(), 1);
        assert_eq!(to_demux_rx.len(), 1);
        assert_eq!(to_reqres_rx.len(), 1);
        assert_eq!(to_pubsub_rx.len(), 1);

        let res = to_pubsub_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");

        assert!(matches!(res, PubSubMsg::Disconnect(_)));
        let res = to_mux_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");
        assert!(matches!(res, MuxMsg::Disconnect(_)));
        let res = to_demux_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");
        assert!(matches!(res, DemuxMsg::IncMsg(_)));

        if let DemuxMsg::IncMsg(msg) = res {
            assert_eq!(
                msg.dp_type,
                seanerbus_schemas::message::MessageType::Publish
            );
            assert_eq!(msg.payload_type, 0);
            assert_eq!(msg.payload, vec![]);
        }
        let res = to_reqres_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");
        assert!(matches!(res, ReqResMsg::Disconnect(_)));
    }
    #[tokio::test]
    async fn test_bad() {
        let id = ConnectionId(Uuid::new_v4());

        let (to_mux_tx, mut to_mux_rx) = tokio::sync::mpsc::channel::<MuxMsg>(1);
        let (to_demux_tx, to_demux_rx) = tokio::sync::mpsc::channel::<DemuxMsg>(1);
        let (to_pubsub_tx, mut to_pubsub_rx) = tokio::sync::mpsc::channel::<PubSubMsg>(1);
        let (to_reqres_tx, mut to_reqres_rx) = tokio::sync::mpsc::channel::<ReqResMsg>(1);
        let chans = BrokerChannels {
            to_mux: to_mux_tx,
            to_demux: to_demux_tx,
            to_pubsub: to_pubsub_tx,
            to_reqres: to_reqres_tx,
        };

        let rd = tokio_test::io::Builder::new()
            .read(b"definitily not a capnp message!")
            .build();

        assert!(matches!(read(id, rd, chans).await, Err(_)));

        assert_eq!(to_mux_rx.len(), 1);
        assert_eq!(to_demux_rx.len(), 0);
        assert_eq!(to_reqres_rx.len(), 1);
        assert_eq!(to_pubsub_rx.len(), 1);

        let res = to_pubsub_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");

        assert!(matches!(res, PubSubMsg::Disconnect(_)));
        let res = to_mux_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");
        assert!(matches!(res, MuxMsg::Disconnect(_)));
        let res = to_reqres_rx
            .recv()
            .await
            .expect("Can not take message from pubsub!");
        assert!(matches!(res, ReqResMsg::Disconnect(_)));
    }
}
