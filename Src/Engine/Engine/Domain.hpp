#pragma once

#include <Engine/Config.hpp>
#include <Engine/EnumOps.hpp>
#include <Engine/Assert.hpp>

#include <cstdint>
#include <string_view>

namespace engine
{

enum class ENGINE_API Domain : uint32_t
{
    NONE = 0x00,
    EDITOR = 0x01,
    CLIENT = 0x02,
    CLIENT_NO_UI = 0x04, // for unit tests

    UI = EDITOR | CLIENT,
    NO_EDITOR = CLIENT | CLIENT_NO_UI,

    ALL = static_cast<uint32_t>(-1)
};
ENGINE_DEFINE_BITWISE_OPS(Domain);

inline std::string_view DomainToString(Domain domain)
{
    switch (domain)
    {
    case Domain::NONE: return "None";
    case Domain::EDITOR: return "Editor";
    case Domain::CLIENT: return "Client";
    case Domain::UI: return "UI";
    case Domain::CLIENT_NO_UI: return "Client no ui";
    case Domain::NO_EDITOR: return "No Editor";
    case Domain::ALL: return "All";
    default: 
        ENGINE_ASSERT(false);
        return "";
    }
}

}
