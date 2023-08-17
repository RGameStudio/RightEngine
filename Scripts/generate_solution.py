import subprocess as sub

print("Downloading submodules...")
sub.run("git submodule update --init")
print("Finished downloading of the submodules!")

print(f"Generating solution...")
sub.run(f"conan install . --output-folder=.build/lib --build=missing --profile=win-64 -s build_type=Debug")
sub.run(f"cmake -B .build/Win -DCMAKE_BUILD_TYPE=Debug --preset conan-default .")
print("Solution was successfully generated!")