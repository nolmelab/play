# tasks

- [ ] setup vcpkg 
- [ ] project setup

- [ ] base 
    - [ ] logger
    - [ ] macros 

- [ ] net 
    - [ ] protocol 
        - [ ] cipher 
            - [ ] plain_cipher
            - [ ] sodium_cipher 
        - [ ] codec 
            - [ ] length_delimited_codec
            - [ ] line_codec
        - [ ] header<Topic, size_t length_field_size = 4>
        - [ ] state 
            - handshaking
            - estabilished

    - session<Protocol, Handler>
    - client<Protocol>
    - server<Protocol>
    - executor 
        - Runner 
            - poll_runner, thread_runner 
    

                 