import subprocess as sub

print("Downloading submodules...")
sub.run("git submodule update --init")
print("Finished downloading of the submodules!")

print(f"Generating solution...")
sub.run(f"cmake -B .build/Win .")
print("Solution was successfully generated!")