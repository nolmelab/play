# protocol 

- codec 
    - encode / decode bytes to bytes    

- handshaker 
    - cipher_handshaker

- length_delimited
- sodium_cipher  

- frame is a packet

- frame_factory<Frame>
- frame_dispatcher<Frame>

- plain_protocol<Frame> : frame_factory<Frame>, frame_dispathcer<Frame>
    - length_delimited 

- secure_protocol<Frame>
    - length_delimited 
    - sodidum_cipher 
    - cipher_handshaker

- secure_mark_protocol<Frame>
    - mark_delimited
    - sodidum_cipher 
    - cipher_handshaker

- secure_flatbuffers_protocol : secure_protocol<flatbuffers::NativeTable>
- secure_json_protocol : secure_protocol<nlohmann::json>

- automating composition is hard 
    - it can make code convoluted and hard to debug 
    - build robust components and composition class manually

- session and protocol relation 
    - protocol::send(Frame::ptr f) 
        - encode
        - put header 
        - session::send(bytes)
    - session::handle_receive(bytes)
        - protocol::receive(bytes)
        - put to pending buffer? 
        - get frame bytes from codec 
        - create frame from frame_fractory<Frame>
        - dispatch with frame_dispatcher<Frame>::dispatch(Frame::ptr)
