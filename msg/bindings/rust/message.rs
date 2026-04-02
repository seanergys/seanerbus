// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use thiserror::Error;
use uuid::Uuid;

use crate::msg_capnp::naming_service_request_v1;
use crate::msg_capnp::naming_service_response_v1;
use crate::msg_capnp::time_value_v1;
pub enum Message {
    TimeValueV1(u64, u64),
    NamingServiceRequestV1(Vec<(String, String)>),
    NamingServiceResponseV1(Vec<Uuid>),
}

#[derive(Error, Debug)]
pub enum MessageError {
    #[error("Deserialization failed: {0}")]
    CapnpError(#[from] capnp::Error),
    #[error("Got message of unknown payload type: {0}")]
    PayloadError(u64),
}

impl Message {
    /// Creates a Message from the given Cap'n'Proto buffer.
    ///
    /// * `data`: Buffer with Cap'n'Proto
    ///
    /// Returns Some(Message) if the data is parsable as a Message and None if not.
    pub fn from_capnp(payload_type: u64, data: &[u8]) -> Result<Message, MessageError> {
        match payload_type {
            1 => {
                let reader =
                    capnp::serialize::read_message(data, capnp::message::ReaderOptions::new())?;

                let reader = reader.get_root::<time_value_v1::Reader>()?;

                Ok(Message::TimeValueV1(reader.get_time(), reader.get_value()))
            }
            2 => {
                let reader =
                    capnp::serialize::read_message(data, capnp::message::ReaderOptions::new())?;
                let reader = reader.get_root::<naming_service_request_v1::Reader>()?;

                let mut attrs: Vec<(String, String)> = vec![];
                for kv in reader.get_attributes()? {
                    attrs.push((
                        kv.get_key()?.to_string().unwrap(),
                        kv.get_value()?.to_string().unwrap(),
                    ));
                }

                Ok(Message::NamingServiceRequestV1(attrs))
            },
            3 => {
                let reader =
                    capnp::serialize::read_message(data, capnp::message::ReaderOptions::new())?;
                let reader = reader.get_root::<naming_service_response_v1::Reader>()?;

                let mut attrs: Vec<Uuid> = vec![];
                for addr in reader.get_matching_interfaces()? {
                    attrs.push(Uuid::from_u64_pair(addr.get_upper(), addr.get_lower()));
                }

                Ok(Message::NamingServiceResponseV1(attrs))
            }
            _ => Err(MessageError::PayloadError(payload_type)),
        }
    }

    /// Serializes the Message to the Cap'n'Proto format.
    ///
    /// Returns the serialized message or an Error if no Serialization could be performed.
    pub fn to_capnp(&self) -> Result<(u64, Vec<u8>), MessageError> {
        match self {
            Message::TimeValueV1(t, v) => {
                let mut bld = capnp::message::TypedBuilder::<time_value_v1::Owned>::new_default();
                let mut msg = bld.init_root();

                msg.set_time(*t);
                msg.set_value(*v);
                let mut buffer = vec![];
                capnp::serialize::write_message(&mut buffer, bld.borrow_inner())?;

                Ok((1, buffer))
            }
            Message::NamingServiceRequestV1(attrs) => {
                let mut bld =
                    capnp::message::TypedBuilder::<naming_service_request_v1::Owned>::new_default();
                let msg = bld.init_root();

                let mut attr_builder = msg.init_attributes(attrs.len().try_into().unwrap());

                for (i, attr) in attrs.iter().enumerate() {
                    attr_builder
                        .reborrow()
                        .get(i as u32)
                        .set_key(attr.0.clone());
                    attr_builder
                        .reborrow()
                        .get(i as u32)
                        .set_value(attr.1.clone());
                }

                let mut buffer = vec![];
                capnp::serialize::write_message(&mut buffer, bld.borrow_inner())?;

                Ok((2, buffer))
            }
            Message::NamingServiceResponseV1(uuids) => {
                let mut bld =
                    capnp::message::TypedBuilder::<naming_service_response_v1::Owned>::new_default(
                    );
                let msg = bld.init_root();

                let mut attr_builder =
                    msg.init_matching_interfaces(uuids.len().try_into().unwrap());

                for (i, uuid) in uuids.iter().enumerate() {
                    let (upper, lower) = uuid.as_u64_pair();
                    attr_builder.reborrow().get(i as u32).set_upper(upper);
                    attr_builder.reborrow().get(i as u32).set_lower(lower);
                }
                let mut buffer = vec![];
                capnp::serialize::write_message(&mut buffer, bld.borrow_inner())?;

                Ok((3, buffer))
            }
        }
    }
}
