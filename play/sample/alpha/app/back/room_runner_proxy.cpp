#include <alpha/share/fb/error_code_generated.h>
#include <alpha/share/fb/room_generated.h>
#include <alpha/share/fb/topic_generated.h>
#include <alpha/app/back/room_master.hpp>
#include <alpha/app/back/room_runner_proxy.hpp>

namespace alpha {

bool room_runner_proxy::on_start()
{
  pulse_ = std::make_unique<app::pulse>();
  pulse_->as_child(master_.get_pulse()).with_strand(master_.get_id()).mark_final().start();

  return true;
}

void room_runner_proxy::on_stop()
{
  pulse_.reset();
}

}  // namespace alpha