from conan.api.conan_api import ConanAPI
from conan.api.model import RecipeReference
import subprocess as sub

IMGUIZMO_VERSION = "1.83.2"
RTTR_VERSION = "0.9.8"
GLSLANG_VERSION = "1.3.296.0"

def check_lib_version(package_name, version):
    conan_api = ConanAPI()
    recipe_ref = RecipeReference(package_name, version)
    return len(conan_api.list.recipe_revisions(recipe_ref)) > 0

def install_custom_packages():
    print("Installing custom packages")
    if not check_lib_version("imguizmo", IMGUIZMO_VERSION):
        sub.run(f"conan create Scripts/lib/imguizmo/all -s build_type=Debug --version {IMGUIZMO_VERSION}")
        sub.run(f"conan create Scripts/lib/imguizmo/all -s build_type=Release --version {IMGUIZMO_VERSION}")

    if not check_lib_version("rttr", RTTR_VERSION):
        sub.run(f"conan create Scripts/lib/rttr/all -s build_type=Debug --version {RTTR_VERSION}")
        sub.run(f"conan create Scripts/lib/rttr/all -s build_type=Release --version {RTTR_VERSION}")

    if not check_lib_version("glslang", GLSLANG_VERSION):
        sub.run(f"conan create Scripts/lib/glslang/all -s build_type=Debug --version {GLSLANG_VERSION}")
        sub.run(f"conan create Scripts/lib/glslang/all -s build_type=Release --version {GLSLANG_VERSION}")

    if not check_lib_version("spirv-cross", GLSLANG_VERSION):
        sub.run(f"conan create Scripts/lib/spirv-cross/all -s build_type=Debug --version {GLSLANG_VERSION}")
        sub.run(f"conan create Scripts/lib/spirv-cross/all -s build_type=Release --version {GLSLANG_VERSION}")

    if not check_lib_version("spirv-tools", GLSLANG_VERSION):
        sub.run(f"conan create Scripts/lib/spirv-tools/all -s build_type=Debug --version {GLSLANG_VERSION}")
        sub.run(f"conan create Scripts/lib/spirv-tools/all -s build_type=Release --version {GLSLANG_VERSION}")