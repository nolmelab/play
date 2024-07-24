#pragma once

#include <atomic>
#include <chrono>
#include <mutex>

namespace play { namespace base {

// 동적으로 시스템 시작 후 고유한 아이디를 제공
/**
 * uuid를 제공하는 클래스가 아니고 서버 시작 후 종료까지만
 * 고유한 아이디를 밀리초당 24바이트 만큼 생성해야 하므로 안전 
 */
class dynamic_snowflake
{
public:
    static dynamic_snowflake& get() 
    {
        static dynamic_snowflake instance_;
        return instance_;
    }
public:
    dynamic_snowflake()
    {
        // 64bit만 지원.
        static_assert(sizeof(size_t) == 8);
        start_ = std::chrono::steady_clock::now();
    }

    size_t next()
    {
        std::scoped_lock guard(lock_);

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();
        auto half = (elapsed << 24) & 0xFFFFFFFF; // 32 bit parts
        size_t id = 0;
        auto seq = ++seq_;
        seq = seq & 0xFFFFFFFF; // 3 bytes
        return half | seq_;
    }

private:
  std::recursive_mutex lock_;
  std::chrono::time_point<std::chrono::steady_clock> start_;
  std::atomic<uint32_t> seq_;  
};

}} //