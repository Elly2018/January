cmake_minimum_required(VERSION 4.1)
project(january C CXX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_definitions(VK_PROTOTYPES)

# Libraries
find_package(Vulkan REQUIRED)
find_package(Freetype REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# SDL3
add_subdirectory(third-party/SDL3 EXCLUDE_FROM_ALL)

# shaderc
set(SHADERC_SKIP_TESTS ON CACHE BOOL "Skip shaderc tests" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "Skip shaderc examples" FORCE)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE BOOL "Skip shaderc copyright check" FORCE)
set(DISABLE_RTTI ON CACHE BOOL "Disable RTTI for shaderc dependencies" FORCE)
set(SHADERC_ENABLE_SHARED_CRT ON CACHE BOOL "Use shared CRT" FORCE)
set(SHADERC_DIR third-party/shaderc)
add_subdirectory(${SHADERC_DIR})

# clip
set(CLIP_DIR third-party/clip)
set(CLIP_EXAMPLES OFF CACHE BOOL "Compile clip examples" FORCE)
set(CLIP_TESTS OFF CACHE BOOL "Compile clip tests" FORCE)
add_subdirectory(${CLIP_DIR} EXCLUDE_FROM_ALL)

# libcurl
set(LIBCURL_DIR third-party/libcurl)
add_subdirectory(${LIBCURL_DIR} EXCLUDE_FROM_ALL)

# entt
set(ENTT_DIR third-party/entt)
add_subdirectory(${ENTT_DIR} EXCLUDE_FROM_ALL)

# ned
set(NED_DIR third-party/ned)
add_subdirectory(${NED_DIR} EXCLUDE_FROM_ALL)
include_directories(${NED_DIR}/editor ${NED_DIR}/lsp ${NED_DIR}/lib ${NED_DIR}/util)
file(COPY ${NED_DIR}/shaders
    DESTINATION ${CMAKE_BINARY_DIR}/
)

# Dear ImGui
set(IMGUI_DIR third-party/imgui)
include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ${IMGUI_DIR}/misc/cpp)

# CR
set(CR_DIR third-party/cr)
include_directories(${CR_DIR}/cr)

# Node editorc
set(Node_DIR third-party/node-editor)
include_directories(${Node_DIR})

# asio
set(ASIO_DIR third-party/asio)
include_directories(${ASIO_DIR}/include)

# uuid
set(UUID_DIR third-party/uuid_v4)
include_directories(${UUID_DIR})

# file watch
set(FileWatch_DIR third-party/filewatch)
include_directories(${FileWatch_DIR})

# cmd
set(CMD_DIR third-party/argh)
include_directories(${CMD_DIR})

# json
set(JSON_DIR third-party/json)
include_directories(${JSON_DIR})

# ini
set(INI_DIR third-party/mINI)
include_directories(${INI_DIR}/src/mini)

# spdlog
set(LOG_DIR third-party/spdlog)
include_directories(${LOG_DIR}/include)

# glm
set(Math_DIR third-party/glm)
include_directories(${Math_DIR})

# notify
set(Notify_DIR third-party/notify/include)
include_directories(${Notify_DIR})

file(GLOB_RECURSE sources src/**.cpp src/**.h)
file(GLOB_RECURSE ned_sources
    ${NED_DIR}/editor/*.cpp 
    ${NED_DIR}/lsp/*.cpp 
    ${NED_DIR}/lib/*.cpp 
    ${NED_DIR}/util/*.cpp
    ${NED_DIR}/lib/*.c      # Crucial: ned contains some pure C helper frameworks
)

add_executable(january
    ${sources}
    ${ned_sources}
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${Node_DIR}/crude_json.cpp
    ${Node_DIR}/imgui_canvas.cpp
    ${Node_DIR}/imgui_node_editor.cpp
    ${Node_DIR}/imgui_node_editor_api.cpp
)

target_compile_options(january PRIVATE -mavx2)

target_include_directories(january PUBLIC
    ${LOG_DIR}/src
    ${SDL3_INCLUDE_DIRS}
    ${LIBCURL_DIR}/include
    ${CLIP_DIR}
    ${SHADERC_DIR}/libshaderc/include
    ${FREETYPE_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
)

target_link_libraries(january PRIVATE
    glfw
    clip
    EnTT::EnTT
    Vulkan::Vulkan
    SDL3::SDL3
    CURL::libcurl
    shaderc
    ${FREETYPE_LIBRARIES}
    ${GTK3_LIBRARIES}
)

target_compile_definitions(january PRIVATE SDL_MAIN_USE_CALLBACKS)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(january PRIVATE DEBUG_MODE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(january PRIVATE RELEASE_MODE)
endif()

file(GLOB_RECURSE FONT_OTF "${CMAKE_SOURCE_DIR}/*.otf")

if(WIN32)
    add_custom_command(TARGET january POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${FONT_OTF}
        $<TARGET_FILE_DIR:january>
        COMMENT "Copying Font to output directory"
    )
else()
endif()