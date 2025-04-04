// #include <Core/Assert.hpp>
// #include <Core/Log.hpp>
// #include <intrin.h>
//
// namespace core
// {
//     void BreakInDebugger()
//     {
// #ifdef R_WIN32
//         __debugbreak();
// #endif
//     }
//
//     void Assert(std::string_view tag, std::string_view code, std::string_view message)
//     {
//         if (message.empty())
//         {
//             log::error("[{}] Assertion failed: {}", tag, code);
//         }
//         else
//         {
//             log::error("[{}] Assertion failed: {}: '{}'", tag, code, message);
//         }
//
//         BreakInDebugger();
//     }
// } // core
