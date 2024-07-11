#pragma once

#include <functional>

namespace play
{
  namespace robust
  {
    namespace net
    {

      // type used to mark empty topic (topic not used)
      struct empty_topic
      {
      };

      template <typename Topic, typename Frame>
      class protocol
      {
      public:
        using frame = Frame;
        using frame_ptr = std::shared_ptr<frame>;
        using topic = Topic;

        // send function provided by session
        using send_fn = std::function<void(const void *data, size_t len)>;
        // recv function provided by session to pass to session_handler
        using recv_fn = std::function<void(Topic topic, const void *data, size_t len)>;

      public:
        protocol(send_fn send_fn, recv_fn recv_fn)
            : send_fn_{send_fn}, recv_fn_{recv_fn}
        {
        }

      protected:
        void send(const void *data, size_t len)
        {
          send_fn_(data, len);
        }

      private:
        send_fn send_fn_;
        recv_fn recv_fn_;
      };

    }
  }
} // play::robust::net