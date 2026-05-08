
#!/usr/bin/env bash
set -euo pipefail

# CMake-based build helper (runs in MSYS2 MINGW64 shell)
BUILD_DIR=build
mkdir -p ${BUILD_DIR}

echo "Configuring with CMake..."
# Allow overriding generator via env var; default to Unix Makefiles in MSYS
GEN="${CMAKE_GENERATOR:-Unix Makefiles}"
echo "Using CMake generator: ${GEN}"
cmake -S . -B ${BUILD_DIR} -G "${GEN}"
echo "Building..."
cmake --build ${BUILD_DIR} -- -j$(nproc || echo 4)
cd ${BUILD_DIR} && mv libgame.so game.so
echo "Build finished. Output placed in ${BUILD_DIR}/ (executable: ${BUILD_DIR}/game.so)"
../../Hotones/build/habenero --pak ../build
