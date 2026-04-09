cmake_minimum_required(VERSION 3.16)
project(Habenero LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE OFF)

# Sources
file(GLOB_RECURSE HAB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

add_executable(habenero ${HAB_SOURCES})

# Link libraries (keep minimal; install deps in MSYS2/mingw64)
if(WIN32)
    target_link_libraries(habenero PRIVATE raylib lua assimp ws2_32)
else()
    target_link_libraries(habenero PRIVATE raylib lua assimp pthread)
endif()

# Ensure Winsock library is linked when available (e.g. MinGW/MSYS environments)
find_library(WS2_LIB NAMES ws2_32 HINTS /mingw64/lib C:/msys64/mingw64/lib)
if(WS2_LIB)
    target_link_libraries(habenero PRIVATE ${WS2_LIB})
endif()

# Output into existing build/ directory so scripts and assets keep working
set_target_properties(habenero PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

# Post-build: copy commonly-needed DLLs from MSYS2 mingw64 if present
if(WIN32)
    set(MSYS_ROOT "C:/msys64")
    set(MSYS_MINGW_BIN "${MSYS_ROOT}/mingw64/bin")
    set(MINGW_DLLS
        libstdc++-6.dll
        libassimp-6.dll
        libgcc_s_seh-1.dll
        libwinpthread-1.dll
        libraylib.dll
        lua54.dll
        glfw3.dll
        libminizip-1.dll
        libbz2-1.dll
        libzstd.dll
    )
    foreach(dll ${MINGW_DLLS})
        set(dllpath "${MSYS_MINGW_BIN}/${dll}")
        if(EXISTS "${dllpath}")
            add_custom_command(TARGET habenero POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${dllpath}" "$<TARGET_FILE_DIR:habenero>/${dll}"
            )
        endif()
    endforeach()
endif()
