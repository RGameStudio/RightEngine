#!/bin/bash

set -e
set -x

poetry install --no-root
poetry env use python3
poetry env activate
poetry run conan remote update conancenter --url https://center2.conan.io
poetry run python3 Scripts/prepare_env.py mac-arm64-ninja-clang false