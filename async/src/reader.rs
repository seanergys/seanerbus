// SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>
//
// SPDX-License-Identifier: MIT

use seanerbus_schemas::message::Message;
use tokio::io::{AsyncRead, AsyncReadExt};

use crate::error::ConnectionError;
/// Represents a message reader
pub struct MessageReader {
    reader: Box<dyn AsyncRead + Unpin + std::marker::Send + 'static>,
    buffer: Vec<u8>,
    cursor: usize,
}

impl MessageReader {
    /// Creates a new SeanerBUS message reader from the given reader,
    /// which must implement AsyncRead.
    pub fn new<Reader>(reader: Reader) -> MessageReader
    where
        Reader: AsyncRead + Unpin + Send + 'static,
    {
        MessageReader {
            reader: Box::new(reader),
            buffer: vec![0; 4096],
            cursor: 0,
        }
    }

    /// Tries to read a SeanerBUS `Message` from the connection.
    ///
    /// Returns a `Result` containing the read message or an Error
    /// Returns:
    /// * [`ConnectionError`](seanerbus_async/error/enum.ConnectionError.html) if the Connection exploded.
    /// * `None` if the end of the stream was reached.
    /// * `Some(Message)` If a message could be succesfully read.
    pub async fn read_message(&mut self) -> Result<Option<Message>, ConnectionError> {
        loop {
            if let Some(msg) = Message::from_capnp(&self.buffer) {
                self.buffer = vec![0; 4096];
                self.cursor = 0;
                return Ok(Some(msg));
            }

            if self.buffer.len() == self.cursor {
                self.buffer.resize(self.cursor * 2, 0);
            }

            let n = self.reader.read(&mut self.buffer[self.cursor..]).await?;

            if 0 == n {
                if self.cursor == 0 {
                    return Ok(None);
                } else {
                    return Err(ConnectionError::GeneralError(
                        "connection reset by peer".into(),
                    ));
                }
            } else {
                self.cursor += n;
            }
        }
    }
}
