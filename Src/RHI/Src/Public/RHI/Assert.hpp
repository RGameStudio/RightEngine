#pragma once

#include <Core/AssertImpl.hpp>

#ifdef R_ENABLE_ASSERTS
#define RHI_ASSERT(x) do { if(!(x)) { core::Assert("RHI", #x); } } while(false)
#define RHI_ASSERT_WITH_MESSAGE(x, ...) do { if(!(x)) { core::Assert("RHI", #x, __VA_ARGS__); } } while(false)
#else
#define RHI_ASSERT(x)
#define RHI_ASSERT_WITH_MESSAGE(x, ...)
#endif