# Версия CMake
cmake_minimum_required(VERSION 3.15)

# Название приложения
set(TARGET_NAME "SamplePoint")
set(TARGET_BIN_NAME "SamplePoint")

# Добавляем .exe (проект в Visual Studio)
add_executable(${TARGET_NAME}
        "Main.cpp")

# Меняем название запускаемого файла в зависимости от типа сборки
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME "${TARGET_BIN_NAME}$<$<CONFIG:Debug>:_Debug>_${PLATFORM_BIT_SUFFIX}")

# Статическая линковка рантайма и стандартных библиотек
if(MSVC)
    set_property(TARGET ${TARGET_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set_property(TARGET ${TARGET_NAME} PROPERTY LINK_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

# Линковка с библиотекой для работы с математикой
target_link_libraries(${TARGET_NAME} PUBLIC "Math")

# Линковка с библиотекой для работы с графикой
target_link_libraries(${TARGET_NAME} PUBLIC "Gfx")