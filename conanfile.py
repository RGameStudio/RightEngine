from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class RightEngineRecipe(ConanFile):
    name = "RightEngine"
    version = "0.0.1"
    package_type = "application"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    requires = [
        "argparse/2.9",
        "assimp/5.3.1",
        "doctest/2.4.11",
        "eastl/3.21.12",
        "entt/3.13.0",
        "fmt/[~10]",
        "glfw/3.3.8",
        "glm/cci.20230113",
        "glslang/1.3.268.0",
        "imgui/1.90.2-docking",
        "imguizmo/1.83.2",
        "nlohmann_json/3.11.3",
        "rttr/0.9.7",
        "spdlog/1.12.0",
        "spirv-cross/1.3.296.0",
        "stb/cci.20230920",
        "stduuid/1.2.3",
        "simdjson/3.10.1",
        "taskflow/3.6.0",
        "tracy/0.10",
        "vulkan-headers/1.3.268.0",
    ]

    exports_sources = "CMakeLists.txt", "Src/*"

    default_options = {
        "assimp*:shared": True,
        "eastl*:shared": True,
        "glslang*:shared": False,
        "glslang*:build_executables": False,
        "glslang*:spv_remapper": False,
        "glslang*:hlsl": False,
        "glslang*:enable_optimizer": False,
        "spirv-cross*:build_executable": False,
        "spirv-cross*:hlsl": False,
        "spirv-cross*:msl": False,
        "spirv-cross*:c_api": False,
        "spirv-cross*:util": False,
        "glfw*:shared": True,
        "rttr*:shared": True,
        "tracy*:shared": True
    }

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
