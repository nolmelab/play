#pragma once

#include <atomic>
#include <play/robust/net/runner.hpp>

namespace play { namespace robust { namespace net {

// io_context를 여러 쓰레드에서 실행
/**
 * 
 */
class thread_runner : public runner
{
public:
  thread_runner(size_t thread_count = 0);

  void stop();

private:
  using work_guard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;

private:
  void run();

private:
  size_t thread_count_;
  std::atomic<bool> stop_;
  work_guard guard_;
  std::vector<std::thread> threads_;
};

}}}  // namespace play::robust::net