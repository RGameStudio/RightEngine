#pragma once

#include <Core/Blob.hpp>
#include <Core/Math.hpp>
#include <Core/Assert.hpp>
#include <Core/EASTLIntergration.hpp>
#include <Core/Hash.hpp>
#include <Core/Log.hpp>
#include <Core/Profiling.hpp>

#include <Engine/Assert.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Registration.hpp>
#include <Engine/Service/ThreadService.hpp>
#include <Engine/Service/Filesystem/VirtualFilesystemService.hpp>

#include <taskflow/taskflow.hpp>

#include <string>
#include <memory>
#include <cstdint>
#include <cstddef>