# net 

## usage 

### app 

app can be subclassed and can be used as the starting point.
- app has an executor 
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

- playrobust::net::executor<Runner>
  - thread singleton using thread local pointer
    - executor::get() returns the current executor for the thread
  - detail:thead_runner, detail::poll_runner




