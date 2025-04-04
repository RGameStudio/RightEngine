from conan.api.conan_api import ConanAPI
from conan.api.model import RecipeReference
import subprocess as sub

def check_lib_version(package_name, version):
    conan_api = ConanAPI()
    recipe_ref = RecipeReference(package_name, version)
    return len(conan_api.list.recipe_revisions(recipe_ref)) > 0

def install_custom_packages():
    print("Installing custom packages")
    if not check_lib_version("imguizmo", "1.83.2"):
        sub.run("conan create Scripts/lib/imguizmo/all -s build_type=Debug --version 1.83.2")
        sub.run("conan create Scripts/lib/imguizmo/all -s build_type=Release --version 1.83.2")

    if not check_lib_version("rttr", "0.9.7"):
        sub.run("conan create Scripts/lib/rttr/all -s build_type=Debug --version 0.9.7")
        sub.run("conan create Scripts/lib/rttr/all -s build_type=Release --version 0.9.7")

    if not check_lib_version("glslang", "1.3.296.0"):
        sub.run("conan create Scripts/lib/glslang/all -s build_type=Debug --version 1.3.296.0")
        sub.run("conan create Scripts/lib/glslang/all -s build_type=Release --version 1.3.296.0")

    if not check_lib_version("spirv-cross", "1.3.296.0"):
        sub.run("conan create Scripts/lib/spirv-cross/all -s build_type=Debug --version 1.3.296.0")
        sub.run("conan create Scripts/lib/spirv-cross/all -s build_type=Release --version 1.3.296.0")

    if not check_lib_version("spirv-tools", "1.3.296.0"):
        sub.run("conan create Scripts/lib/spirv-tools/all -s build_type=Debug --version 1.3.296.0")
        sub.run("conan create Scripts/lib/spirv-tools/all -s build_type=Release --version 1.3.296.0")