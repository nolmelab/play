#pragma once

#include <functional>
#include <play/robust/ensure/act.hpp>
#include <map>

namespace play { namespace robust { namespace ensure {

class act_factory
{
public: 
    using creator = std::function<act::ptr()>;

public:     
    static act_factory& get();

public: 
    act::ptr create(std::string_view name);

    void reg(std::string_view name, creator fn);

private:
    using creator_map = std::map<std::string, creator>;

private:
    creator_map creators_;
};

}}} // 