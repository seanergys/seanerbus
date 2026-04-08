// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use crate::demuxer::DemuxMsg;
use crate::muxer::MuxMsg;
use crate::pubsub::PubSubMsg;
use crate::reqres::ReqResMsg;

use crate::writer::SenderMsg;
use log::debug;
use seanerbus_schemas::message::ConnectionId;
use tokio::sync::mpsc;

pub struct BrokerError;

pub struct BrokerOwner {
    channels: BrokerChannels,
    pub threads: tokio::task::JoinSet<Result<(), BrokerError>>,
}

impl BrokerOwner {
    pub async fn new() -> BrokerOwner {
        let (to_mux_tx, to_mux_rx) = tokio::sync::mpsc::channel::<MuxMsg>(1);
        let (to_demux_tx, to_demux_rx) = tokio::sync::mpsc::channel::<DemuxMsg>(1);
        let (to_pubsub_tx, to_pubsub_rx) = tokio::sync::mpsc::channel::<PubSubMsg>(1);
        let (to_reqres_tx, to_reqres_rx) = tokio::sync::mpsc::channel::<ReqResMsg>(1);

        let mut broker = BrokerOwner {
            channels: BrokerChannels {
                to_mux: to_mux_tx,
                to_pubsub: to_pubsub_tx,
                to_demux: to_demux_tx,
                to_reqres: to_reqres_tx,
            },
            threads: tokio::task::JoinSet::new(),
        };

        broker.threads.spawn(async move {
            crate::muxer::mux(to_mux_rx).await;

            Ok(())
        });

        let chans = broker.clone_channels();
        broker.threads.spawn(async move {
            crate::demuxer::demux(chans, to_demux_rx).await;
            Ok(())
        });

        let chans = broker.clone_channels();
        broker.threads.spawn(async move {
            crate::pubsub::pub_sub(chans, to_pubsub_rx).await;
            Ok(())
        });

        let chans = broker.clone_channels();
        broker.threads.spawn(async move {
            crate::reqres::req_res(chans, to_reqres_rx).await;
            Ok(())
        });

        broker
    }

    pub fn clone_channels(&self) -> BrokerChannels {
        self.channels.clone()
    }
}

#[derive(Clone)]
pub struct BrokerChannels {
    pub to_mux: mpsc::Sender<MuxMsg>,
    pub to_pubsub: mpsc::Sender<PubSubMsg>,
    pub to_demux: mpsc::Sender<DemuxMsg>,
    pub to_reqres: mpsc::Sender<ReqResMsg>,
}

impl BrokerChannels {
    pub async fn register_sender(&self, id: ConnectionId, tx: mpsc::Sender<SenderMsg>) {
        if let Err(e) = self.to_mux.send(MuxMsg::BackChannel(id, tx)).await {
            debug!(
                "Mux channel died unexpectedly (Reason {}), shutting down",
                e
            );
            self.shutdown().await;
        }
    }

    pub async fn disconnect(&self, conn_id: ConnectionId) {
        if let Err(e) = self.to_pubsub.send(PubSubMsg::Disconnect(conn_id)).await {
            debug!(
                "pub/sub service died (Reason: {}), following it to Valhallah",
                e
            );
            self.shutdown().await
        }
        if let Err(e) = self.to_reqres.send(ReqResMsg::Disconnect(conn_id)).await {
            debug!(
                "req/res service died (Reason: {}), following it to Valhallah",
                e
            );
            self.shutdown().await
        }
        if let Err(e) = self.to_mux.send(MuxMsg::Disconnect(conn_id)).await {
            debug!(
                "mux service died (Reason: {}), following it to Valhallah",
                e
            );
            self.shutdown().await
        }
    }

    pub async fn shutdown(&self) {
        self.to_mux
            .send(MuxMsg::Shutdown)
            .await
            .expect("Could send mux shutdown");
        self.to_demux
            .send(DemuxMsg::Shutdown)
            .await
            .expect("Could send demux shutdown");
        self.to_pubsub
            .send(PubSubMsg::Shutdown)
            .await
            .expect("Could send pub/sub shutdown");
        self.to_reqres
            .send(ReqResMsg::Shutdown)
            .await
            .expect("Could send req/res shutdown");
    }
}
