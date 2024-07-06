# protocol implementation 

- write components 

- protocol uses std::function to send bytes
  - remvoe dependency on session<ProtocoL> 
  - require to do unit test without session implementation

- plain_protocol<uint16_t, BytesFrame>
- plain_protocol<uint16_t, flatbuffer::NativeTable>
- secure_protocol<uint16_t, BytesFrame>
- secure_protocol<uint16_t, flatbuffer::NativeTable>

