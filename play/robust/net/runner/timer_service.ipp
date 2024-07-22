#include <play/robust/base/macros.hpp>
#include <play/robust/net/runner/timer_service.hpp>

namespace play { namespace robust { namespace net {

template <typename CompletionToken>
inline void timer::once(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  PLAY_CHECK(!set_);

  set_ = true;
  once_ = true;
  repeat_ = false;
  timer_.expires_from_now(ms);
  if (strand_ != nullptr)
  {
    timer_.async_wait(asio::bind_executor(*strand_,
                                          [this, handler](const error_code& ec)
                                          {
                                            handler(*this);
                                          }));
  }
  else
  {
    timer_.async_wait(
        [this, handler](const error_code& ec)
        {
          handler(*this);
        });
  }
}

template <typename CompletionToken>
inline void timer::repeat(asio::chrono::milliseconds interval_ms, CompletionToken&& handler)
{
  set_ = true;
  once_ = false;
  repeat_ = true;

  timer_.expires_from_now(interval_ms);
  if (strand_ != nullptr)
  {
    timer_.async_wait(asio::bind_executor(*strand_,
                                          [this, handler](const error_code& ec)
                                          {
                                            handler(*this);
                                          }));
  }
  else
  {
    timer_.async_wait(
        [this, interval_ms, handler](const error_code& ec)
        {
          repeat_call(interval_ms, handler, ec);
        });
  }
}

inline void timer::cancel()
{
  if (set_)
  {
    timer_.cancel();
    set_ = false;
  }
}

template <typename CompletionToken>
inline void timer::repeat_call(asio::chrono::milliseconds interval_ms, CompletionToken&& handler,
                               const error_code& ec)
{
  handler(*this);

  if (set_)
  {
    timer_.expires_from_now(interval_ms);
    if (strand_ != nullptr)
    {
      timer_.async_wait(asio::bind_executor(*strand_,
                                            [this, interval_ms, handler](const error_code& ec)
                                            {
                                              repeat_call(interval_ms, handler, ec);
                                            }));
    }
    else
    {
      timer_.async_wait(
          [this, interval_ms, handler](const error_code& ec)
          {
            repeat_call(interval_ms, handler, ec);
          });
    }
  }
}

template <typename CompletionToken>
inline timer::ref timer_service::once(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  auto ref = pool_.construct(std::reference_wrapper<asio::io_context>(ioc_));
  ref->once(ms, std::forward<CompletionToken>(handler));
  return ref;
}

template <typename CompletionToken>
inline timer::ref timer_service::repeat(asio::chrono::milliseconds ms, CompletionToken&& handler)
{
  auto ref = pool_.construct(std::reference_wrapper<asio::io_context>(ioc_));
  ref->repeat(ms, std::forward<CompletionToken>(handler));
  return ref;
}

template <typename CompletionToken>
inline timer::ref timer_service::once(timer::strand_type& strand, asio::chrono::milliseconds ms,
                       CompletionToken&& handler)
{
  auto ref = pool_.construct(std::reference_wrapper<asio::io_context>(ioc_), &strand);
  ref->once(ms, std::forward<CompletionToken>(handler));
  return ref;
}

template <typename CompletionToken>
inline timer::ref timer_service::repeat(timer::strand_type& strand, asio::chrono::milliseconds ms,
                         CompletionToken&& handler)
{
  auto ref = pool_.construct(std::reference_wrapper<asio::io_context>(ioc_), &strand);
  ref->repeat(ms, std::forward<CompletionToken>(handler));
  return ref;
}

}}}  // namespace play::robust::net

// note:
// recursive labmda를 lambda를 리턴하는 람다로 구현 가능하지만
// 함수로 분리하는 것이 가독성이 더 좋고 충분하다.