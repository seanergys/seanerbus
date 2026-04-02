@0xec449ac8f0e6b560;
struct TimeValueV1
{
    time @0 :UInt64;
    value @1 :UInt64;
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
