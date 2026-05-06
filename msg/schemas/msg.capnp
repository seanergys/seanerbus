@0xec449ac8f0e6b560;


enum MessageTypes
{
    timeValueV1 @0;
    namingServiceReqV1 @1;
    namingServiceResV1 @2;
    helloWorldReqV1 @3;
    helloWorldResV1 @4;
}

struct TimeValueV1
{
    time @0 :UInt64;
    value @1 :Float64;
}

enum PayloadType {
 timeValueV1 @0;
}

struct NamingServiceReqV1
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

struct NamingServiceResV1
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
