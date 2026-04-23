@0xec449ac8f0e6b560;


enum MessageTypes
{
    timeValueV1 @0;
    namingServiceReq @1;
    namingServiceRes @2;
    helloWorldReq @3;
    helloWorldRes @4;
}

struct TimeValueV1
{
    time @0 :UInt64;
    value @1 :Float64;
}

enum PayloadType {
 timeValueV1 @0;
}

struct NamingServiceRequestV1
{
struct KV
{
key @0 :Text;
value @1 :Text;
}
attributes @0 :List(KV);
}

struct Address{
lower @0 :UInt64;
upper @1 :UInt64;
}

struct NamingServiceResponseV1
{
matchingInterfaces @0 :List(Address);
}

struct HelloWorldReqV1
{
msg @0 :Text;
}

struct HelloWorldResV1
{
msg @0 :Text;
}
