@echo off

poetry install --no-root
poetry env use python
poetry env activate
poetry run conan remote update conancenter --url https://center2.conan.io
poetry run python scripts/prepare_env.py win-64-ninja-clang false

pause