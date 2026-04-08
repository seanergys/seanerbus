// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use log::{debug, warn};
use seanerbus::broker::BrokerError;
use seanerbus::broker::BrokerOwner;
use seanerbus_schemas::message::ConnectionId;
use uuid::Uuid;

async fn accepter() {
    let listener = tokio::net::TcpListener::bind("localhost:5398")
        .await
        .unwrap();
    let mut broker = BrokerOwner::new().await;

    let mut set = tokio::task::JoinSet::<Result<(), BrokerError>>::new();

    debug!("Waiting for connection to accept!");
    loop {
        tokio::select! {
            msg = listener.accept() =>
            match msg {
            Ok((socket, _)) => {
                debug!("Accepting connection!");

                let broker = broker.clone_channels();
                let (reader, writer) = socket.into_split();
                let conn_id = ConnectionId(Uuid::new_v4());

                let broker_tmp = broker.clone();
                set.spawn(async move {
                    if let Err(e) = seanerbus::writer::write(conn_id, writer, broker_tmp).await {
                        debug!("Writer task died unexpectedly: {}", e);
                    }
                    Ok(())
                });

                let broker_tmp = broker.clone();
                set.spawn(async move {
                    if let Err(e) = seanerbus::reader::read(conn_id, reader, broker_tmp).await {
                        debug!("Reader task died unexpectedly: {}", e);
                    }
                    Ok(())
                });
            }
            Err(e) => {
                warn!(
                    "Could not accept any connection (Reason: {}), shutting down",
                    e
                )
            }
        },
            Some(j) = set.join_next() => {
                match j
                {
                    Ok(o) => {
                        if o.is_err()
                        {
                            warn!("Reader/Writer thread exited with error, shutting down");
                            break;
                        }
                        else
                        {
                            debug!("Reader/Writer handler exited gracefully");
                        }
                    },
                    Err(e) =>{
                        warn!("Reader/Writer thread panicked: {}, shutting down", e);
                        break;
                    }
                }
            },

            Some(j) = broker.threads.join_next() => {
                match j
                {
                    Ok(o) => {
                        if o.is_err(){
                            warn!("Broker thread exited with error, shutting down");
                            break;
                        }
                        else
                        {
                            debug!("Broker exited gracefully");
                        }
                    },
                    Err(e) =>{
                        warn!("Broker thread panicked: {}, shutting down", e);
                        break;
                    }
                }

            },
                _ = tokio::signal::ctrl_c() => {
                    warn!("Received Ctrl+C, shutting down");
                    broker.clone_channels().shutdown().await;
                    broker.threads.join_all().await;
                    break;
                }
        }
    }
}
/// Entry-Point of SEANERBUS.
///
/// This accepts connections on localhost:5398 in a loop.
#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();

    accepter().await;
    Ok(())
}
