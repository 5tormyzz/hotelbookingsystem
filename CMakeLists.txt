cmake_minimum_required(VERSION 3.30)
project(Hotelbookingsystem)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${PROJECT_SOURCE_DIR}/roomLogic
        ${PROJECT_SOURCE_DIR}/utilities
        ${PROJECT_SOURCE_DIR}/createWindow
        ${PROJECT_SOURCE_DIR}/libs/imGui
        ${PROJECT_SOURCE_DIR}/libs/imGui/backends
        ${PROJECT_SOURCE_DIR}/libs/GLFW/include/GLFW
        ${PROJECT_SOURCE_DIR}/libs/GLFW/lib-mingw-w64
        ${PROJECT_SOURCE_DIR}/libs/GLFW/lib-vc2022
        ${PROJECT_SOURCE_DIR}/libs/json
)

add_executable(Hotelbookingsystem
        src/main.cpp
        utilities/inputValidation.cpp
        roomLogic/bookingData.cpp
        createWindow/initializeWindow.cpp
        libs/imGui/imgui.cpp
        libs/imGui/imgui_widgets.cpp
        libs/imGui/imgui_tables.cpp
        libs/imGui/imgui_draw.cpp
        libs/imGui/imgui_demo.cpp
        libs/imGui/backends/imgui_impl_glfw.cpp
        libs/imGui/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(Hotelbookingsystem PRIVATE
        ${CMAKE_SOURCE_DIR}/libs/GLFW/lib-mingw-w64/libglfw3.a
        opengl32
)