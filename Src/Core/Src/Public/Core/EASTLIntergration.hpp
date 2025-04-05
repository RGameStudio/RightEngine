#pragma once

#include <Core/Hash.hpp>
#include <EASTL/functional.h>
#include <EASTL/array.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/vector_map.h>
#include <EASTL/vector_set.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/list.h>
#include <EASTL/deque.h>
#include <EASTL/internal/red_black_tree.h>
#include <EASTL/map.h>
#include <EASTL/set.h>
#include <string>
#include <typeindex>
#include <filesystem>

namespace eastl
{

template<>
struct hash<std::string>
{
    size_t operator()(const std::string& str) const
    {
        return core::hash::HashString(str);
    }
};

template<>
struct hash<std::string_view>
{
    size_t operator()(std::string_view str) const
    {
        return core::hash::HashString(str);
    }
};

template<>
struct hash<std::type_index>
{
    size_t operator()(const std::type_index& tIdx) const
    {
        return tIdx.hash_code();
    }
};

template<>
struct hash<std::filesystem::path>
{
    size_t operator()(const std::filesystem::path& path) const
    {
        return hash<std::string>{}(path.generic_u8string());
    }
};

template <typename T>
struct hash<std::shared_ptr<T>>
{
public:
    inline size_t operator()(const std::shared_ptr<T>& value) const
    {
        return hash<size_t>()(reinterpret_cast<size_t>(value.get()));
    }
};

} // eastl

// TODO: Make an EASTL fork and rewrite their stupid allocator
// Must be defined in each dll that uses EASTL

#define EASTL_ALLOCATOR_IMPL \
void* __cdecl operator new[](size_t size, size_t alignment, size_t offset, const char* pName, int flags, unsigned debugFlags, const char* file, int line) \
{ \
    return new uint8_t[size]; \
} \
void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line) \
{ \
    return new uint8_t[size]; \
}