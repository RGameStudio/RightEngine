#include "LaunchEngine.hpp"
#include "Event.hpp"
#include "EventDispatcher.hpp"
#include "Logger.hpp"
#include "Window.hpp"
#include "Input.hpp"

#ifdef R_ENABLE_ASSERTS
#define R_ASSERT(x, ...) { if(!(x)) { R_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define R_CORE_ASSERT(x, ...) { if(!(x)) { R_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define R_ASSERT(x, ...)
#define R_CORE_ASSERT(x, ...)
#endif
