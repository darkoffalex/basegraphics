# Версия CMake
cmake_minimum_required(VERSION 3.15)

# Название библиотеки
set(TARGET_NAME "Math")

# Добавляем header-only библиотеку
add_library(${TARGET_NAME} INTERFACE)
target_include_directories(${TARGET_NAME} INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)