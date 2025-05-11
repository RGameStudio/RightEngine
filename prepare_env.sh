#!/bin/bash

set -e
set -x

poetry install
poetry env use python3
poetry env activate
poetry run python3 Scripts/prepare_env.py mac-arm64-ninja-clang false