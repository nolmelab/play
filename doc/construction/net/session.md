# session 

a wrapper for socket and asio async operations. 

## responsibility 

- sends bytes 
- recv bytes 
- notifies socket (session) state to session_handlers
  - client and server are those session_handlers 
- provides functions to send frames 

- protocol provides application functionality, but session does not 
  - session, client, server are intended for reuse 
  - protocol is intended to provide application specific needs

## relation with session_handler 

- on_session_accepted
- on_session_connected 
- on_session_connect_error 
- on_session_error 

- session<SessionHandler, Protocol>

