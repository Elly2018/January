#pragma once
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <memory>
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

struct JWindow {
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

    ImGui_ImplVulkanH_Window g_MainWindowData;
    uint32_t                 g_MinImageCount = 2;
    bool                     g_SwapChainRebuild = false;
};

// Window context initialization
std::shared_ptr<JWindow> JInit();
// Escape the mainloop, release resource
void JDeInit(std::weak_ptr<JWindow> w_win);
// January application main loop
void JMainloop(std::weak_ptr<JWindow> w_win);