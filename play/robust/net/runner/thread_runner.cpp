#include <play/robust/base/logger.hpp>
#include <play/robust/net/runner/thread_runner.hpp>

namespace play { namespace robust { namespace net {

thread_runner::thread_runner(size_t thread_count, const std::string& name)
    : runner(name),
      thread_count_{thread_count},
      guard_{get_ioc().get_executor()},
      stop_{false}
{
  if (thread_count == 0)
  {
    thread_count_ = std::thread::hardware_concurrency();
  }

  LOG()->info("starting thread_runner: {}. concurrency: {}", get_name(), thread_count_);

  std::vector<std::thread> threads;
  threads.resize(thread_count_);

  stop_ = false;

  for (size_t i = 0; i < thread_count_; ++i)
  {
    threads[i] = std::thread(
        [this]()
        {
          this->run();
        });
  }

  threads_.swap(threads);

  LOG()->info("started thread_runner: {}. concurrency: {}", get_name(), thread_count_);
}

void thread_runner::stop()
{
  PLAY_RETURN_IF(stop_);

  stop_ = true;
  guard_.reset();
  get_ioc().stop(); // needs to call to escape from run()

  for (auto& thread : threads_)
  {
    thread.join();
  }

  LOG()->info("stopped thread_runner: {}", get_name());
}

void thread_runner::run()
{
  while (!stop_)
  {
    size_t run_count = get_ioc().run();
    if (run_count == 0)
    {
      sleep(1);
    }
  }
}

}}}  // namespace play::robust::net