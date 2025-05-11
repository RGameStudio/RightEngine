#pragma once

#include <Core/AssertImpl.hpp>

#ifdef R_ENABLE_ASSERTS
#define ENGINE_ASSERT(x) do { if(!(x)) { core::Assert("ENGINE", #x); } } while(false)
#define ENGINE_ASSERT_WITH_MESSAGE(x, ...) do { if(!(x)) { core::Assert("ENGINE", #x, __VA_ARGS__); } } while(false)
#else
#define ENGINE_ASSERT(x)
#define ENGINE_ASSERT_WITH_MESSAGE(x, ...)
#endif