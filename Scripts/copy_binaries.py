import subprocess as sub
import sys
import os
import shutil

def copy_files(source_dir, destination_dir, extension=".dll"):
    if not os.path.exists(destination_dir):
        os.makedirs(destination_dir)

    copied_files = set()

    for root, _, files in os.walk(source_dir):
        for filename in files:
            if filename.lower().endswith(extension):

                if filename in copied_files:
                    continue
                else:
                    copied_files.add(filename)

                source_path = os.path.join(root, filename)
                destination_path = os.path.join(destination_dir, filename)

                try:
                    shutil.copy2(source_path, destination_path)
                    print(f"Copied '{filename}' to '{destination_dir}'.")
                except Exception as e:
                    print(f"Error copying '{filename}': {e}")
                    sys.exit(1)

if len(sys.argv) < 3:
    print("Usage: python copy_binaries.py <input dir> <output dir>")
    sys.exit(1)

input_dir = sys.argv[1]
output_dir = sys.argv[2]

try:
    copy_files(input_dir, output_dir, ".dll")
    copy_files("Scripts/bin", output_dir, ".exe")

except sub.CalledProcessError as e:
    print("Error while copying binaries:", e)
    sys.exit(1)
except Exception as e:
    print("An unexpected error occurred:", e)
    sys.exit(1)