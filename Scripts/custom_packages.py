from conan.api.conan_api import ConanAPI
from conan.api.model import RecipeReference
import subprocess as sub

IMGUIZMO_VERSION = "1.83.2"
RTTR_VERSION = "0.9.8"

def check_lib_version(package_name, version):
    conan_api = ConanAPI()
    recipe_ref = RecipeReference(package_name, version)
    return len(conan_api.list.recipe_revisions(recipe_ref)) > 0

def install_custom_packages(profile_name: str):
    print("Installing custom packages")
    if not check_lib_version("imguizmo", IMGUIZMO_VERSION):
        sub.run(f"poetry run conan create Scripts/lib/imguizmo/all -s build_type=Debug --build=missing --version {IMGUIZMO_VERSION} --profile:host={profile_name} --profile:build={profile_name}", shell=True, check=True, text=True)
        sub.run(f"poetry run conan create Scripts/lib/imguizmo/all -s build_type=Release --build=missing --version {IMGUIZMO_VERSION} --profile:host={profile_name} --profile:build={profile_name}", shell=True, check=True, text=True)

    if not check_lib_version("rttr", RTTR_VERSION):
        sub.run(f"poetry run conan create Scripts/lib/rttr/all -s build_type=Debug --build=missing --version {RTTR_VERSION} --profile:host={profile_name} --profile:build={profile_name}", shell=True, check=True, text=True)
        sub.run(f"poetry run conan create Scripts/lib/rttr/all -s build_type=Release --build=missing --version {RTTR_VERSION} --profile:host={profile_name} --profile:build={profile_name}", shell=True, check=True, text=True)
