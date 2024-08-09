#include <alpha/ensure/act/act_client.hpp>
#include <alpha/ensure/ensure_alpha.hpp>
#include <play/ensure/bot.hpp>

namespace alpha {
act_client::act_client(play::actor& owner)
    : act_base{owner, {}, {}, "act_client"},
      handler_{ensure_alpha::get().get_runner()},
      client_{handler_}
{
}

}  // namespace alpha