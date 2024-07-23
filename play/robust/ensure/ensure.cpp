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

    // auto jserver = base::json_reader::read(jconf_, "server");
    // TODO: start server to forward ensure states

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
  // TODO: stop server

  stop_bots();
}

bool ensure::start_bots()
{
  for (size_t i = 0; i < bot_count_; ++i)
  {
    auto index = bot_start_index_ + i;
    auto bp = std::make_shared<bot>(*this, fmt::format("{}{}", bot_prefix_, index), index);
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
