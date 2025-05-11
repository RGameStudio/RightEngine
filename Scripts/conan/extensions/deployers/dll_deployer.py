from conan.tools.files import copy
import os

def deploy(graph, output_folder, **kwargs):
    for name, dep in graph.root.conanfile.dependencies.items():
        if (dep.folders.package_folder is None):
            # with open(os.path.expanduser("~/code/file.txt"), "a") as f:
            #     f.write(f"{graph.root.conanfile.build_folder}\n")
            #     f.write(f"dep: {dep}\n")
            #     f.write(f"name: {name}\n")
            continue
        copy(graph.root.conanfile, "*.dll", dep.folders.package_folder, os.path.join(output_folder, f"{graph.root.conanfile.build_folder}/dll", str(dep)))
        copy(graph.root.conanfile, "*.dylib", dep.folders.package_folder, os.path.join(output_folder, f"{graph.root.conanfile.build_folder}/dll", str(dep)))