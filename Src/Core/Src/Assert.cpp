#include <Core/Assert.hpp>
#include <Core/Log.hpp>

#ifdef R_OS_WIN32
#include <intrin.h>
#endif

namespace core
{
    void BreakInDebugger()
    {
#ifdef R_OS_WIN32
        __debugbreak();
#elif defined(__APPLE__)
        __builtin_trap();
#elif defined(__linux__)
        raise(SIGTRAP);
#else
        std::abort();
#endif
    }

    void Assert(std::string_view tag, std::string_view code, std::string_view message)
    {
        if (message.empty())
        {
            log::error("[{}] Assertion failed: {}", tag, code);
        }
        else
        {
            log::error("[{}] Assertion failed: {}: '{}'", tag, code, message);
        }

        BreakInDebugger();
    }
} // core
