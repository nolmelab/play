# net 

## usage 

### app 

app can be subclassed and can be used as the starting point.
- app has an executor (io_context, threads)
- app has services and starts them
- app has a net::server and setup it to prepare communication
- app has a net::client and setup it to prepare communication

### app::start

```c++
server.listen();
server.add_topic(topic, creator);
server.add_topic(topic, creator);
server.add_topic(topic, creator);
...
server.sub_connection(handler);
server.sub_topic(topic, handler);
server.sub_topic(topic, handler);
server.sub_topic(topic, handler);
... 
```

## design 

- play::robust::net namespace

- protocol
  - cipher
  - codec
- session<Protocol, Handler>
  - notifies to handler on events
  - asio::stream_buf 
- client<Protocol>
  - has session<Protocol, client<Protocol>>
- server<Protocol>
  - has session<Protocol, client<Protocol>>
 
## impl

- protocol 
- play::robust::net::executor
  - executor::get().post() available.
- 


