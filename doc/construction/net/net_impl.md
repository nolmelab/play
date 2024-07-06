# net implementation 

- steps 
  - setup vcpkg
  - 
  - setup project

  - write protocol components 
    - codec, length_delimited, sodium_cipher, handshaker
    - write skeletal code
    - write unit tests 
    - fill the gap and finish components 

  - write client, server, session, protocol 
    - plain_protocol<Topic, Frame> 
      - plain_protocol<uint16_t, BytesFrame>
      - plain_protocol<uint16_t, flatbuffers::NativeTable>
    - secure_protocl<Topic, Frame> 
      - secure_protocol<uint16_t, BytesFrame>
      - secure_protocol<uint16_t, flatbuffers::NativeTable>
    - skeletal, unit tests, fill the gap

## setup vcpkg

## setup project 



