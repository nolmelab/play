#include <play/robust/ensure/bot.hpp>

namespace play { namespace robust { namespace ensure {

bot::bot(ensure& app, const nlohmann::json& json, const std::string& name, size_t index)
    : app_{app},
      json_{json},
      name_{name},
      index_{index}
{
}

bool bot::start()
{
  /**
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
   */

  // create a flow
  // - create acts
  // activate the flow
  //
  return true;
}

void bot::stop()
{
  // deactivate the flow
}

}}}  // namespace play::robust::ensure