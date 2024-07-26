#pragma once

#include <play/net/asio.hpp>
#include <play/net/runner/timer_service.hpp>

namespace play {

// io_context runner base class
/**
 * - io_context를 갖고 post()와 post(stran_key) 함수로 처리를 실행
 * - timer들을 갖고 스케줄링을 처리
 */
class runner
{
public:
  using strand_type = asio::io_context::strand;

public:
  runner(std::string_view name = "default", size_t strand_count = 64);

  // 가장 중요한 asio 함수. 임의의 처리를 프로액터에 맡겨서 처리.
  template <typename CompletionToken>
  void post(CompletionToken&& handler)
  {
    ioc_.post(std::forward<CompletionToken>(handler));
  }

  template <typename CompletionToken>
  timer::ref once(asio::chrono::milliseconds ms, CompletionToken&& handler)
  {
    return timer_service_.once(ms, std::forward<CompletionToken>(handler));
  }

  template <typename CompletionToken>
  timer::ref repeat(asio::chrono::milliseconds ms, CompletionToken&& handler)
  {
    return timer_service_.repeat(ms, std::forward<CompletionToken>(handler));
  }

  template <typename CompletionToken>
  void post(size_t strand_key, CompletionToken&& handler)
  {
    auto& strand = get_strand(strand_key);
    strand.post(std::forward<CompletionToken>(handler));
  }

  template <typename CompletionToken>
  timer::ref once(size_t strand_key, asio::chrono::milliseconds ms, CompletionToken&& handler)
  {
    auto& strand = get_strand(strand_key);
    return timer_service_.once(strand, ms, std::forward<CompletionToken>(handler));
  }

  template <typename CompletionToken>
  timer::ref repeat(size_t strand_key, asio::chrono::milliseconds ms, CompletionToken&& handler)
  {
    auto& strand = get_strand(strand_key);
    return timer_service_.repeat(strand, ms, std::forward<CompletionToken>(handler));
  }

  asio::io_context& get_ioc()
  {
    return ioc_;
  }

  const std::string& get_name() const
  {
    return name_;
  }

  // sleep for ms milliseconds
  static void sleep(int ms);

private:
  void make_strands(size_t strand_count);

  strand_type& get_strand(size_t strand_key)
  {
    return strands_[strand_key % strands_.size()];
  }

private:
  asio::io_context ioc_;
  std::string name_;
  timer_service timer_service_;
  std::vector<strand_type> strands_;
};

}  // namespace play