# protocol 

protocol is application specific payload management layer. 

## related concepts (classes)

- codec 
    - encode / decode bytes to bytes    

- handshaker 
    - cipher_handshaker

- codecs
  - length_delimited
  - sodium_cipher  

- frame is a packet
- frame_factory<Frame>
- frame_dispatcher<Frame>

## responsibility 

- handles session state notification frames 
- transforms frame into network packets and send with session 
- make frames from received bytes and dispatches them with frame_dispatcher 

## protocl examples 

- plain_protocol<Topic, Frame> : frame_factory<Frame>, frame_dispathcer<Frame>
    - length_delimited 

- secure_protocol<Topic, Frame>
    - length_delimited 
    - sodidum_cipher 
    - cipher_handshaker

- secure_mark_protocol<Frame>
    - mark_delimited
    - sodidum_cipher 
    - cipher_handshaker

- secure_flatbuffers_protocol : secure_protocol<Topic, flatbuffers::NativeTable>
- secure_json_protocol : secure_mark_protocol<nlohmann::json>

## relations

- automating composition is hard 
    - it can make code convoluted and hard to debug 
    - build robust components and composition class manually

- session and protocol relation 
    - protocol::send(Frame::ptr f), orprotocol::send(Frame const& f) 
        - encode
        - put header 
        - session::send(bytes)
    - session::handle_receive(bytes)
        - protocol::receive(bytes)
        - put to pending buffer? 
        - get frame bytes from codec 
        - create frame from frame_fractory<Frame>
        - dispatch with frame_dispatcher<Frame>::dispatch(Frame::ptr)

- session_handler and protocol relation 
    - the same session events are forwarded to protocol 
    - those events can be made into Frame and dispatched 
    - protocol::handle_accepted() 
    - protocol::handle_connected() 
    - protocol::handle_error()

- protocol can be either topic based or not
    - topic based protocol 
      - uses topic field in header or in frame payload 
      - then dispatches with it 
    - this handling of topic field is implemented in protocol 
  
## limitations 

- std::shared_ptr<Frame> is Frame::ptr
- Frame::ptr is used to dispatch Frames 
- Frame& or Frame::Ptr is used to send 

## extensions

- Application depends on the specific protocol 
  - This is unavoidable dependency 
  - Hence, utility functions can be implemented in protocol 

