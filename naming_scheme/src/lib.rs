#![allow(async_fn_in_trait)]
// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_client::Connection;
use seanerbus_msg::message::Message;
use seanerbus_schemas::message::Address;
use thiserror::Error;
use uuid::Uuid;

#[derive(Error, Debug)]
pub enum ResolveError {
    #[error("Registering name {0:?} for address {1} failed: {2}")]
    RegisterError(Vec<(String, String)>, Address, String),
    #[error("Unregistering address {0} failed: {1}")]
    UnregisterError(Address, String),
}

pub trait ResolveExt {
    async fn resolve(&mut self, name: Vec<(String, String)>) -> Vec<Address>;
    async fn reverse_resolve(&mut self, addr: Address) -> Vec<(String, String)>;
    async fn register_resolve(
        &mut self,
        name: Vec<(String, String)>,
        addr: Address,
    ) -> Result<(), ResolveError>;
    async fn unregister_resolve(&mut self, addr: Address) -> Result<(), ResolveError>;
}

impl ResolveExt for Connection {
    async fn resolve(&mut self, name: Vec<(String, String)>) -> Vec<Address> {
        let (payload_type, payload) = Message::ResolveReqV1(name).to_capnp().unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::ResolveResV1(addrs) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            addrs.into_iter().map(Address).collect()
        } else {
            panic!("Got wrong message type as answer");
        }
    }
    async fn reverse_resolve(&mut self, addr: Address) -> Vec<(String, String)> {
        let (payload_type, payload) = Message::ReverseResolveReqV1(addr.0).to_capnp().unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::ReverseResolveResV1(name) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            name
        } else {
            panic!("Got wrong message type as answer");
        }
    }

    async fn register_resolve(
        &mut self,
        name: Vec<(String, String)>,
        addr: Address,
    ) -> Result<(), ResolveError> {
        let (payload_type, payload) = Message::RegisterResolveReqV1(name.clone(), addr.0)
            .to_capnp()
            .unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::RegisterResolveResV1(success, err_msg) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            if success {
                Ok(())
            } else {
                Err(ResolveError::RegisterError(name, addr, err_msg))
            }
        } else {
            panic!("Got wrong message type as answer");
        }
    }
    async fn unregister_resolve(&mut self, addr: Address) -> Result<(), ResolveError> {
        let (payload_type, payload) = Message::UnregisterResolveReqV1(addr.0).to_capnp().unwrap();
        let msg = self
            .request(
                Address(Uuid::from_u64_pair(0, 0x10ca105)),
                payload_type,
                &payload,
            )
            .await
            .unwrap();

        if let Message::UnregisterResolveResV1(success, err_msg) =
            Message::from_capnp(msg.payload_type, &msg.payload).unwrap()
        {
            if success {
                Ok(())
            } else {
                Err(ResolveError::UnregisterError(addr, err_msg))
            }
        } else {
            panic!("Got wrong message type as answer");
        }
    }
}
