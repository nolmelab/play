#include <play/robust/ensure/ensure.hpp>

namespace play { namespace robust { namespace ensure {

ensure::ensure()
{
  //
}

bool ensure::start(std::string_view jconf)
{
  /**
   * {
   *   "server" : { "port" : port, "concurrency" : 8 }, 
   *   "ensure" : { "bot_count" : 8, "bot_start_index" : 1, "bot_prefix" : "bot_", 
   *    "flow" : [
   *        {
   *          "type" : "act_delay", 
   *          "signal" : { "fail" : { "cmd" : "next" }, "success" : { "cmd" : "next"}}
   *        }, 
   *        {
   *          "type" : "act_message", 
   *          "message" : "hello ensure", 
   *          "signal" : { "fail" : { "cmd" : "exit" }, "success" : { "cmd" : "exit"}}
   *        }
   *     ]
   *  }, 
   * }
   */
  base::start(jconf);

  // load josn
  // create bots
  // start bots
}

void ensure::stop()
{
  // stop bots

  base::stop();
}

}}}  // namespace play::robust::ensure
