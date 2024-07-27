#pragma once

#include <chrono>

namespace play {

class stop_watch
{
public:
  stop_watch()
  {
    start_ = std::chrono::steady_clock::now();
  }

  size_t get_elapsed()
  {
    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
    return elapsed;
  }

  size_t stop()
  {
    return get_elapsed();
  }

private:
  std::chrono::time_point<std::chrono::steady_clock> start_;
};

}  // namespace play