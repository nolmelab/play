#pragma once

#include <play/robust/base/object_pool.hpp>
#include <play/robust/net/asio.hpp>

namespace play { namespace robust { namespace net {

class timer
{
public:
  using ref = std::shared_ptr<timer>;
  using strand_type = asio::io_context::strand;

public:
  timer(std::reference_wrapper<asio::io_context> ioc)
      : timer_{ioc.get()},
        strand_{nullptr},
        ms_{0},
        set_{false},
        once_{false},
        repeat_{false}
  {
  }

  timer(std::reference_wrapper<asio::io_context> ioc, strand_type* strand)
      : timer_{ioc.get()},
        strand_{strand},
        ms_{0},
        set_{false},
        once_{false},
        repeat_{false}
  {
  }

  ~timer()
  {
    cancel();
  }

  template <typename CompletionToken>
  void once(asio::chrono::milliseconds ms, CompletionToken&& handler);

  template <typename CompletionToken>
  void repeat(asio::chrono::milliseconds interval_ms, CompletionToken&& handler);

  void cancel();

private:
  template <typename CompletionToken>
  void repeat_call(asio::chrono::milliseconds interval_ms, CompletionToken&& handler,
                   const error_code& ec);

private:
  asio::steady_timer timer_;
  strand_type* strand_;
  asio::chrono::milliseconds ms_;
  std::atomic<bool> set_;  // used for cancel. hence requires safety.
  bool once_;
  bool repeat_;
};

class timer_service
{
public:
  timer_service(asio::io_context& ioc)
      : ioc_{ioc}
  {
  }

  template <typename CompletionToken>
  timer::ref once(asio::chrono::milliseconds ms, CompletionToken&& handler);

  template <typename CompletionToken>
  timer::ref repeat(asio::chrono::milliseconds ms, CompletionToken&& handler);

  template <typename CompletionToken>
  timer::ref once(timer::strand_type& strand, asio::chrono::milliseconds ms,
                  CompletionToken&& handler);

  template <typename CompletionToken>
  timer::ref repeat(timer::strand_type& strand, asio::chrono::milliseconds ms,
                    CompletionToken&& handler);

private:
  inline static base::object_pool<timer> pool_;
  asio::io_context& ioc_;
};

}}}  // namespace play::robust::net

#include <play/robust/net/runner/timer_service.ipp>