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

- plain_protocol<Topic, Frame> : frame_factory<Topic, Frame>, frame_dispathcer<Topic, Frame>
    - length_delimited 

- secure_protocol<Topic, Frame>
    - length_delimited 
    - sodidum_cipher 
    - cipher_handshaker

- json_protocol<Topic, Frame>
    - topic field
    - mark_delimited

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

## adapter 구현 

std::function을 갖고 있도록 일단 했다. std::function은 함수 호출보다는 비용이 높다. 
따라서, 함수 포인터나 람다를 직접 갖도록 하는 것이 좋겠다. 
람다도 함수 포인터로 변환이 가능하므로 타잎을 맞춰서 지정할 수 있다. 

가상 함수도 함수 호출만큼 빠르다. 호출 비용은 거의 없다. 

따라서, 일단 std::function으로 구조를 만들고 그 이후에 생각해 본다. 통신 쪽 처리에서는 
충분한 성능을 제공하기 때문이다. 


