# Scenario tests

This directory contains end-to-end test scenarios:


## sub_python_pub_cpp

A subscriber, written in Python, and a publisher written in C++.
The subscribing Python script will wait for three `TimeValueV1` events published by the C++ publisher and verify that they are correct.

## req_python_res_python_array

request/response both written in Python. The requester sends a `VectorReqV1` request that is responded to with a `VectorResV1`.
