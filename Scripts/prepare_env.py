import subprocess as sub
import sys
import os
import platform

def check_process_status_code(code, proc_stderr):
    if code != 0:
        print("Error: Conan preparation failed")
        print("Error message: ", proc_stderr)
        sys.exit(1)

if len(sys.argv) < 3:
    print("Usage: python prepare_env.py <profile name> <is running ci>")
    sys.exit(1)

profile_name = sys.argv[1]
is_ci = sys.argv[2].lower() == "true"
is_windows = platform.system() == "Windows"

print("Configuring python env")
sub.run("poetry install", shell=True)

print("Preparing conan env")
sub.run("poetry run conan config install -t dir Scripts/conan", shell=True)

from custom_packages import install_custom_packages

install_custom_packages(profile_name)

win_sdk_version = "10.0.26100.0"

if not is_ci:
    if is_windows:
        try:
            # Try Community edition first
            sub.run(f"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat {win_sdk_version}")
        except:
            try:
                # Try Professional edition
                sub.run(f"C:\\Program Files\\Microsoft Visual Studio\\2022\\Professional\\VC\\Auxiliary\\Build\\vcvars64.bat {win_sdk_version}") 
            except:
                try:
                    # Try Enterprise edition
                    sub.run(f"C:\\Program Files\\Microsoft Visual Studio\\2022\\Enterprise\\VC\\Auxiliary\\Build\\vcvars64.bat {win_sdk_version}")
                except:
                    print("Error: Could not find Visual Studio installation")
                    sys.exit(1)

def install_all(profile_name: str, build_type: str):
    print(f"Installing {build_type} conan packages")
    status_code = sub.run(f"poetry run conan install . --deployer=dll_deployer -c tools.cmake.cmake_layout:build_folder=\".build/conan\" --build=missing --profile:host={profile_name} --profile:build={profile_name} -s build_type={build_type}", shell=True, check=True, text=True)
    check_process_status_code(status_code.returncode, status_code.stderr)

install_all(profile_name, "Debug")
install_all(profile_name, "Release")

if not os.path.exists(".build"):
    os.mkdir(".build")
