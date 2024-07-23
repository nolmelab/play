#include <fstream>
#include <play/robust/base/json_reader.hpp>
#include <play/robust/ensure/ensure.hpp>

namespace play { namespace robust { namespace ensure {

ensure::ensure(const std::string& config_file)
    : config_file_{config_file}
{
}

bool ensure::start()
{
  try
  {
    std::ifstream ifs("file.json");
    ifs >> jconf_;

    auto jserver = base::json_reader::read(jconf_, "server");

    auto concurrency =
        base::json_reader::read(jconf_, "ensure.concurrency", std::thread::hardware_concurrency());
    auto port = base::json_reader::read(jconf_, "ensure.port", 7000);

    runner_ = std::make_unique<net::thread_runner>(concurrency, "ensure");
    server_ = std::make_unique<server>(*runner_.get(), handler_);

    server_->start(port);

    bot_count_ = base::json_reader::read(jconf_, "ensure.bot_count", 1);
    bot_start_index_ = base::json_reader::read(jconf_, "ensure.bot_start_index", 1);
    bot_prefix_ = base::json_reader::read(jconf_, "ensure.bot_prefix", std::string{"bot_"});

    start_bots();

    return true;
  }
  catch (std::exception& ex)
  {
    LOG()->error("exception: {} while string ensure", ex.what());
    return false;
  }
}

void ensure::stop()
{
  stop_bots();
  runner_->stop();
}

bool ensure::start_bots()
{
  for (size_t i = 0; i < bot_count_; ++i)
  {
    auto index = bot_start_index_ + i;
    auto bp = std::make_shared<bot>(*this, jconf_, fmt::format("{}{}", bot_prefix_, index), index);
    auto rc = bp->start();
    if (rc)
    {
      bots_.push_back(bp);
    }
  }

  return true;
}

void ensure::stop_bots()
{
  for (auto& bp : bots_)
  {
    bp->stop();
  }
}

}}}  // namespace play::robust::ensure
