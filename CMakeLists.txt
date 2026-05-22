cmake_minimum_required(VERSION 4.1)
project(january C CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(VK_PROTOTYPES)

# diligent_engine
set(DILIGENT_BUILD_SAMPLES OFF CACHE BOOL "Disable Diligent samples" FORCE)
set(DILIGENT_BUILD_TOOLS OFF CACHE BOOL "Disable Diligent tools" FORCE)
set(DILIGENT_BUILD_FX OFF CACHE BOOL "Disable DiligentFX framework" FORCE)
set(DILIGENT_NO_FORMAT_VALIDATION ON CACHE BOOL "Disable format validation" FORCE)
set(DILIGENT_DIR third-party/diligent_engine)
add_subdirectory(${DILIGENT_DIR} EXCLUDE_FROM_ALL)

# SDL3
add_subdirectory(third-party/SDL3 EXCLUDE_FROM_ALL)

# libcurl
set(LIBCURL_DIR third-party/libcurl)
add_subdirectory(${LIBCURL_DIR} EXCLUDE_FROM_ALL)

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

# Libraries
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE sources src/**.cpp src/**.h)

add_executable(january 
  ${sources} 
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
)

target_link_directories(january PRIVATE 
  ${DILIGENT_DIR}/DiligentCore
)

if(WIN32)
    target_link_libraries(january PRIVATE 
        glfw
        Vulkan::Vulkan 
        SDL3::SDL3
        CURL::libcurl
        Diligent-GraphicsEngineD3D11-static
        Diligent-GraphicsEngineD3D12-static
        Diligent-GraphicsEngineVk-static
    )
elseif(APPLE)
    target_link_libraries(january PRIVATE 
        glfw
        Vulkan::Vulkan 
        SDL3::SDL3
        CURL::libcurl
        Diligent-GraphicsEngineMetal-static
    )
else() # Linux
    target_link_libraries(january PRIVATE 
        glfw
        Vulkan::Vulkan 
        SDL3::SDL3
        CURL::libcurl
        Diligent-GraphicsEngineVk-static
        Diligent-GraphicsEngineOpenGL-static
    )
endif()

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