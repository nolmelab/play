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
using protocol = secure_protocol<uint16_t, flatbuffers::NativeTable>;

server.listen();
protocol::add_topic(topic, creator);
protocol::add_topic(topic, creator);
protocol::add_topic(topic, creator);
...
protocol::sub_accept(handler);
protocol::sub_disconnect(handler);
protocol::sub_topic(topic, handler);
protocol::sub_topic(topic, handler);
protocol::sub_topic(topic, handler);
... 
```


## design 

- play::robust::net namespace

- [protocol](./net/protocol.md)
  - cipher
  - codec

- session<Protocol>
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




