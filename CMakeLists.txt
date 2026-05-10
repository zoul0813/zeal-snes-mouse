cmake_minimum_required(VERSION 3.16)

# -------------------------------
# CMake Setup
# -------------------------------
# Application Version
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)
set(APP_NAME "SNES Mouse")
set(target_name "mouse")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# Ensure the bin directory exists
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# -------------------------------
# Toolchain
# -------------------------------
set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

# -------------------------------
# Project
# -------------------------------
project(${target_name} C ASM)

# -------------------------------
# Include and link SDKs
# -------------------------------
zos_use(zvb)
find_package(ZVB_SDK REQUIRED)

zos_use(zgdk)
find_package(ZGDK REQUIRED)

# -------------------------------
# Executable
# -------------------------------
add_executable(${target_name}
  src/mouse.c
#   src/controller.c
  src/paint.c
)

target_link_libraries(${target_name} PUBLIC zvb)
target_link_libraries(${target_name} PUBLIC zgdk)

# -------------------------------
# Convert to binary
# -------------------------------
zos_add_outputs(${target_name})