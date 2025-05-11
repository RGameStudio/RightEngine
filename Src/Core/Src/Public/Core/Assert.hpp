#pragma once

#include <Core/AssertImpl.hpp>

#ifdef R_ENABLE_ASSERTS
#define CORE_ASSERT(x) do { if(!(x)) { core::Assert("CORE", #x); } } while(false)
#define CORE_ASSERT_WITH_MESSAGE(x, ...) do { if(!(x)) { core::Assert("CORE", #x, __VA_ARGS__); } } while(false)
#else
#define CORE_ASSERT(x)
#define CORE_ASSERT_WITH_MESSAGE(x, ...)
#endif
