# protocol implementation 

- write components 

- protocol uses std::function to send bytes
  - remvoe dependency on session<ProtocoL> 
  - require to do unit test without session implementation

- plain_protocol<uint16_t, BytesFrame>
- plain_protocol<uint16_t, flatbuffer::NativeTable>
- secure_protocol<uint16_t, BytesFrame>
- secure_protocol<uint16_t, flatbuffer::NativeTable>

## character display in vscode

- cmake invokes visual studio cl.exe
  - cl.exe messages displayed in incompatible character encoding 
  - how to fix this?
    - "cmake.outputLogEncoding": "utf8" 
    - the above fixes the problem

## plain_protocol 

- starting with a plain_protocol<uint16_t, BytesFrame>
  - then, finish upto secure_protocol<>
  - length, bytes
  - memory allocation and copy


### codec

- length_delimited_codec
  - plain_protocol, secure_protocol 
    - use it to get frame range
    - use it to put length field 
    - protocol has send_buf : asio::stream_buf
    - protocol has recv_buf : asio::stream_buf 
    - codec can have buffer for working 
      - and copy to the stream_buf 

- encode
  - app call protocol::send(Frame::ptr f)
  - f serialized into bytes 
    - serialized buffer is used to encode 
    - encode(src : streambuf&, dst : streambuf&)
      - use dst buffer to encode
      - put encoded bytes into the dst buffer
      - provide more buffer types if required
    - protocol prepares send buffer to reuse it for mulitple codecs. 
      - protocol's responsibility
  
- decode
  - session call protocol::recv(bytes)
    - put bytes into recv buffer : streambuf
    - call decode of lenth_delimited_codec
    - works on two buffers
   
### streambuf

- learn streambuf 
  - prepare 
  - commit 
  - consume 

boost::asio::streambuf inherits from std::streambuf and provides 
commit and consume functions. 


