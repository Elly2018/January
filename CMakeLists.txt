# Example usage:
#  mkdir build
#  cd build
#  cmake -g "Visual Studio 14 2015" ..

cmake_minimum_required(VERSION 2.8)
project(january C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# SDL3
find_package(SDL3 REQUIRED)

# Dear ImGui
set(IMGUI_DIR third-party/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends)

# cmd
set(CMD_DIR third-party/argh)
include_directories(${CMD_DIR})

# Libraries
find_package(Vulkan REQUIRED)
#find_library(VULKAN_LIBRARY
  #NAMES vulkan vulkan-1)
#set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
# FIXME: Linux needs linking with "X11" as well?

# Use vulkan headers from glfw:
include_directories(${GLFW_DIR}/deps)

file(GLOB_RECURSE sources src/**.cpp src/**.h)

add_executable(january 
  ${sources} 
  ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp 
  ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp 
  ${IMGUI_DIR}/imgui.cpp 
  ${IMGUI_DIR}/imgui_draw.cpp 
  ${IMGUI_DIR}/imgui_demo.cpp 
  ${IMGUI_DIR}/imgui_tables.cpp 
  ${IMGUI_DIR}/imgui_widgets.cpp)
target_link_libraries(january PRIVATE glfw;Vulkan::Vulkan SDL3::SDL3)
target_include_directories(january PUBLIC ${SDL3_INCLUDE_DIRS})
target_compile_definitions(january PRIVATE SDL_MAIN_USE_CALLBACKS)
