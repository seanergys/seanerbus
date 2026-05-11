<!--
SPDX-FileCopyrightText: 2026 SEANERGYS Partners <https://seanergys.eu>

SPDX-License-Identifier: MIT
-->

# Examples of using the C++ client library

This directory contains basic examples for using the C++ client library for SeanerBUS.

## subscribe_test

This example will subscribe to the UUID given on the command line, expecting TimeValueV1 values:

```
$ ./subscribe_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f


time=1, value=1
time=2, value=2
time=3, value=3
[...]
```

## publish_test 

This example publishes TimeValueV1 messages on the address given on the command line.

```
./publish_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f

Publishing time=0, value=0
Publishing time=1, value=1
Publishing time=2, value=2
```

## request_test

This sends a HelloWorldReqV1 to the address specified on the command line, printing the resulting HelloWorldResV1

```
$ ./request_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f
Got answer to request: Appreciate your "Hello World"
```

## response_test

This registers the Address given on the command line as an endpoint for request/response. On an incoming HelloWorldReqV1 it answers with a HelloWorldResV1.

```
$ ./response_test 7bc3d210-a49f-4422-8fe8-26ca480d0e1f
Got incoming HelloWorldReqV1: Hello World
```
