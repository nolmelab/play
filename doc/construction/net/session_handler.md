# session_handler 

client and server are session_handlers. 

## responsibilty 

- handle state and lifetime of sessions 
- listen or connect to other nodes 
- reconnect to other nodes if disconnected 

## relations 

- Runner 
  - runner 
    - post(), timer, strand
  - poll_runner 
  - thread_runner 

- client 
  - has a session
  - start connecting 
  - reconnect on disconnect 

- server 
  - has sessions 
  - listen 
  - accept 
  - handle disconnect 

