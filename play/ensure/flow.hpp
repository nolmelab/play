#pragma once

#include <nlohmann/json.hpp>
<<<<<<< HEAD:play/ensure/flow.hpp
#include <play/ensure/act.hpp>
=======
#include <play/robust/ensure/act_serial.hpp>
>>>>>>> 91c59b068a24bbbadad9bbc480851488feb7e86b:play/robust/ensure/flow.hpp

namespace play { namespace ensure {

class bot;

// 순차적으로 act들을 관리하면서 실행
/**
 *  
 */
class flow : public act_serial
{
public:
  using act_ptr = std::shared_ptr<act>;

public:
  flow(app::actor& bt, const nlohmann::json& json);

private:
  void exit() final;
};

}}  // namespace play::ensure