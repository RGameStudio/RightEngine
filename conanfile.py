from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class RightEngineRecipe(ConanFile):
    name = "RightEngine"
    version = "0.0.1"
    package_type = "application"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    requires = [
        "argparse/3.1",
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
        "rttr/0.9.8",
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
        "assimp*:with_gltf": False,
        "assimp*:with_gltf_exporter": False,
        "assimp*:with_3mf_exporter": False,
        "assimp*:with_blend": False,
        "assimp*:with_ifc": False,
        "assimp*:with_m3d": False,
        "assimp*:with_m3d_exporter": False,
        "assimp*:with_pbrt_exporter": False,
        "assimp*:with_opengex": False,
        "eastl*:shared": False,
        "glslang*:build_executables": False,
        "glslang*:spv_remapper": False,
        "glslang*:hlsl": False,
        "glslang*:enable_optimizer": False,
        "spirv-cross*:build_executable": False,
        "spirv-cross*:hlsl": False,
        "spirv-cross*:msl": False,
        "spirv-cross*:c_api": False,
        "spirv-cross*:util": False,
        "rttr*:with_rtti": True
    }

    def configure(self):
        if self.settings.os == "Macos":
            self.options["assimp"].shared = False
            self.options["glslang"].shared = False
            self.options["glfw"].shared = False
            self.options["tracy"].shared = False
            self.options["imgui"].shared = False
            self.options["fmt"].shared = False
            self.options["spdlog"].shared = False
        else:
            self.options["assimp"].shared = True
            self.options["glfw"].shared = True
            self.options["tracy"].shared = True

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
