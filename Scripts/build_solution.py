import subprocess as sub

print("Started solution build")
try:
    completed_process = sub.run("cmake --build .build/Win --config Debug", shell=True, check=True, text=True)
    
    if completed_process.returncode == 0:
        print("Solution was successfully built!")
    else:
        print("Error: Solution build failed.")
        print("Error output:", completed_process.stderr)
except sub.CalledProcessError as e:
    print("Error while building the solution:", e)
except Exception as e:
    print("An unexpected error occurred:", e)
