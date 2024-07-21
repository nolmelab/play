#include <play/robust/app/app_fb.hpp>

namespace play { namespace robust { namespace app {

app_fb& app_fb::get()
{
    return static_cast<app_fb>(app::get());
}

bool app_fb::start()
{
    auto rc1 = server_.start();
    PLAY_RETURN_VALUE_IF(!rc1, false);

    return on_start();
}

void spp_fb::stop()
{
    server_.stop();
    runner_.stop();

    on_stop();
} 

}}}  // namespace play::robust::app