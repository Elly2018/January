/**
MIT License

Copyright (c) 2026 ZhuElly

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#ifndef SYSTEM_WINDOW_H
#define SYSTEM_WINDOW_H
#include <cinttypes>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
#endif

namespace January {

    namespace Engine {
        struct JEngine;
    }

    namespace System {
        struct JSystem;
        // Pure render window struct
        // This should be generate by engine or editor
        struct JWindowRender {
            SDL_WindowFlags          g_windowFlags;
            SDL_Window*              g_window = nullptr;
            VkAllocationCallbacks*   g_Allocator = nullptr;
            VkInstance               g_Instance = VK_NULL_HANDLE;
            VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
            VkDevice                 g_Device = VK_NULL_HANDLE;
            uint32_t                 g_QueueFamily = (uint32_t)-1;
            VkQueue                  g_Queue = VK_NULL_HANDLE;
            VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
            VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

            bool                     g_done = false;
            uint32_t                 g_MinImageCount = 2;
            bool                     g_SwapChainRebuild = false;
        };

        // Main window struct
        // This handles the editor window
        struct JWindow : JWindowRender {
            ImGui_ImplVulkanH_Window g_MainWindowData;
            ImGuiID                  g_dockerspace = 0;
        };
        // The init config for the window
        struct JRWindowInit {
            SDL_WindowFlags          g_flags = 0;
            int32_t                  g_width = 800;
            int32_t                  g_height = 600;
        };

        // The editor window drawing function call
        void DrawLoop(struct JSystem& jsystem);

        // Editor window context initialization
        int32_t JInit(JWindow& jwindow, JRWindowInit init);
        // Editor window release resource
        void JDeInit(JWindow& jwindow);
        // Pure window context initialization
        int32_t JRInit(JWindowRender& jrwindow, JRWindowInit init);
        // Pure window release resource
        void JRDeInit(JWindowRender& jwindow);
    }
}
#endif