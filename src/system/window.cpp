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
#include "window.h"
#include "../engine/engine.h"
#include "../gui/manager.h"
#include "system.h"
#include <format>
#include <imgui.h>
#include <imgui_notify.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <tahoma.h>

namespace January::System {

#pragma region Vulkan Functions
static void SetupVulkan(JWindowRender &win,
                        std::vector<const char *> instance_extensions) {
  VkResult err;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkInitialize();
#endif

  VCreateInstance(instance_extensions, win.g_Instance, win.g_Allocator);

  VGetPhysicalDeviceFront(win.g_Instance, win.g_PhysicalDevice);
  IM_ASSERT(win.g_PhysicalDevice != VK_NULL_HANDLE);

  // Select graphics queue family
  win.g_QueueFamily = VGetQueueFamily(win.g_PhysicalDevice);
  IM_ASSERT(win.g_QueueFamily != (uint32_t)-1);

  VGetLogicalDevice(win.g_PhysicalDevice, win.g_QueueFamily, win.g_Allocator,
                    win.g_Queue, win.g_Device);
  VGetDescriptionPool(win.g_Device, win.g_Allocator, win.g_DescriptorPool);
}
// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used
// by the demo. Your real engine/app may not use them.
static void SetupVulkanWindow(JWindow &win, ImGui_ImplVulkanH_Window *wd,
                              VkSurfaceKHR surface, int32_t width,
                              int32_t height) {
  wd->Surface = surface;

  // Check for WSI support
  VkBool32 res;
  vkGetPhysicalDeviceSurfaceSupportKHR(win.g_PhysicalDevice, win.g_QueueFamily,
                                       wd->Surface, &res);
  if (res != VK_TRUE) {
    fprintf(stderr, "Error no WSI support on physical device 0\n");
    exit(-1);
  }

  // Select Surface Format
  const VkFormat requestSurfaceImageFormat[] = {
      VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM};
  const VkColorSpaceKHR requestSurfaceColorSpace =
      VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(
      win.g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat,
      (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat),
      requestSurfaceColorSpace);

  // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_MAILBOX_KHR,
                                      VK_PRESENT_MODE_IMMEDIATE_KHR,
                                      VK_PRESENT_MODE_FIFO_KHR};
#else
  VkPresentModeKHR present_modes[] = {VK_PRESENT_MODE_FIFO_KHR};
#endif
  wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(
      win.g_PhysicalDevice, wd->Surface, &present_modes[0],
      IM_ARRAYSIZE(present_modes));
  // printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

  // Create SwapChain, RenderPass, Framebuffer, etc.
  IM_ASSERT(win.g_MinImageCount >= 2);
  ImGui_ImplVulkanH_CreateOrResizeWindow(
      win.g_Instance, win.g_PhysicalDevice, win.g_Device, wd, win.g_QueueFamily,
      win.g_Allocator, width, height, win.g_MinImageCount, 0);
}

static void CleanupVulkan(JWindow &win) {
  vkDestroyDescriptorPool(win.g_Device, win.g_DescriptorPool, win.g_Allocator);

#ifdef APP_USE_VULKAN_DEBUG_REPORT
  // Remove the debug report callback
  auto f_vkDestroyDebugReportCallbackEXT =
      (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
          win.g_Instance, "vkDestroyDebugReportCallbackEXT");
  f_vkDestroyDebugReportCallbackEXT(win.g_Instance, g_DebugReport,
                                    win.g_Allocator);
#endif // APP_USE_VULKAN_DEBUG_REPORT

  vkDestroyDevice(win.g_Device, win.g_Allocator);
  vkDestroyInstance(win.g_Instance, win.g_Allocator);
}

static void CleanupVulkanWindow(JWindow &win) {
  ImGui_ImplVulkanH_DestroyWindow(win.g_Instance, win.g_Device,
                                  &win.g_MainWindowData, win.g_Allocator);
}

static void FrameRender(JWindow &win, ImDrawData *draw_data) {
  VkSemaphore image_acquired_semaphore =
      win.g_MainWindowData.FrameSemaphores[win.g_MainWindowData.SemaphoreIndex]
          .ImageAcquiredSemaphore;
  VkSemaphore render_complete_semaphore =
      win.g_MainWindowData.FrameSemaphores[win.g_MainWindowData.SemaphoreIndex]
          .RenderCompleteSemaphore;
  VkResult err =
      vkAcquireNextImageKHR(win.g_Device, win.g_MainWindowData.Swapchain,
                            UINT64_MAX, image_acquired_semaphore,
                            VK_NULL_HANDLE, &win.g_MainWindowData.FrameIndex);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    win.g_SwapChainRebuild = true;
  if (err == VK_ERROR_OUT_OF_DATE_KHR)
    return;
  if (err != VK_SUBOPTIMAL_KHR)
    check_vk_result(err);

  ImGui_ImplVulkanH_Frame *fd =
      &win.g_MainWindowData.Frames[win.g_MainWindowData.FrameIndex];
  {
    err = vkWaitForFences(
        win.g_Device, 1, &fd->Fence, VK_TRUE,
        UINT64_MAX); // wait indefinitely instead of periodically checking
    check_vk_result(err);

    err = vkResetFences(win.g_Device, 1, &fd->Fence);
    check_vk_result(err);
  }
  {
    err = vkResetCommandPool(win.g_Device, fd->CommandPool, 0);
    check_vk_result(err);
    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
    check_vk_result(err);
  }
  {
    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.renderPass = win.g_MainWindowData.RenderPass;
    info.framebuffer = fd->Framebuffer;
    info.renderArea.extent.width = win.g_MainWindowData.Width;
    info.renderArea.extent.height = win.g_MainWindowData.Height;
    info.clearValueCount = 1;
    info.pClearValues = &win.g_MainWindowData.ClearValue;
    vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
  }

  // Record dear imgui primitives into command buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

  // Submit command buffer
  vkCmdEndRenderPass(fd->CommandBuffer);
  {
    VkPipelineStageFlags wait_stage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &image_acquired_semaphore;
    info.pWaitDstStageMask = &wait_stage;
    info.commandBufferCount = 1;
    info.pCommandBuffers = &fd->CommandBuffer;
    info.signalSemaphoreCount = 1;
    info.pSignalSemaphores = &render_complete_semaphore;

    err = vkEndCommandBuffer(fd->CommandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(win.g_Queue, 1, &info, fd->Fence);
    check_vk_result(err);
  }
}

static void FramePresent(JWindow &win) {
  if (win.g_SwapChainRebuild)
    return;
  VkSemaphore render_complete_semaphore =
      win.g_MainWindowData.FrameSemaphores[win.g_MainWindowData.SemaphoreIndex]
          .RenderCompleteSemaphore;
  VkPresentInfoKHR info = {};
  info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  info.waitSemaphoreCount = 1;
  info.pWaitSemaphores = &render_complete_semaphore;
  info.swapchainCount = 1;
  info.pSwapchains = &win.g_MainWindowData.Swapchain;
  info.pImageIndices = &win.g_MainWindowData.FrameIndex;
  VkResult err = vkQueuePresentKHR(win.g_Queue, &info);
  if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
    win.g_SwapChainRebuild = true;
  if (err == VK_ERROR_OUT_OF_DATE_KHR)
    return;
  if (err != VK_SUBOPTIMAL_KHR)
    check_vk_result(err);
  win.g_MainWindowData.SemaphoreIndex =
      (win.g_MainWindowData.SemaphoreIndex + 1) %
      win.g_MainWindowData
          .SemaphoreCount; // Now we can use the next set of semaphores
}
#pragma endregion

#pragma region MainLoop
void DrawLoop(struct JSystem &jsystem) {
  JWindow &jwindow = *jsystem.window;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  while (!jwindow.g_done) {
    // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
    // SDL_AppIterate() function]
    if (SDL_GetWindowFlags(jwindow.g_window) & SDL_WINDOW_MINIMIZED) {
      SDL_Delay(10);
      continue;
    }

    // Resize swap chain?
    int fb_width, fb_height;
    SDL_GetWindowSize(jwindow.g_window, &fb_width, &fb_height);
    if (fb_width > 0 && fb_height > 0 &&
        (jwindow.g_SwapChainRebuild ||
         jwindow.g_MainWindowData.Width != fb_width ||
         jwindow.g_MainWindowData.Height != fb_height)) {
      ImGui_ImplVulkan_SetMinImageCount(jwindow.g_MinImageCount);
      ImGui_ImplVulkanH_CreateOrResizeWindow(
          jwindow.g_Instance, jwindow.g_PhysicalDevice, jwindow.g_Device,
          &jwindow.g_MainWindowData, jwindow.g_QueueFamily, jwindow.g_Allocator,
          fb_width, fb_height, jwindow.g_MinImageCount, 0);
      jwindow.g_MainWindowData.FrameIndex = 0;
      jwindow.g_SwapChainRebuild = false;
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    jwindow.g_dockerspace = ImGui::DockSpaceOverViewport();
    VDraw(*jsystem.engine->manager, *jsystem.window, *jsystem.engine);

    // Render toasts on top of everything, at the end of your code!
    // You should push style vars here
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f); // Round borders
    ImGui::PushStyleColor(ImGuiCol_WindowBg,
                          ImVec4(43.f / 255.f, 43.f / 255.f, 43.f / 255.f,
                                 100.f / 255.f)); // Background color
    ImGui::RenderNotifications(); // <-- Here we render all notifications
    ImGui::PopStyleVar(1);        // Don't forget to Pop()
    ImGui::PopStyleColor(1);

    // Rendering
    ImGui::Render();
    ImDrawData *main_draw_data = ImGui::GetDrawData();
    const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f ||
                                    main_draw_data->DisplaySize.y <= 0.0f);
    jwindow.g_MainWindowData.ClearValue.color.float32[0] =
        clear_color.x * clear_color.w;
    jwindow.g_MainWindowData.ClearValue.color.float32[1] =
        clear_color.y * clear_color.w;
    jwindow.g_MainWindowData.ClearValue.color.float32[2] =
        clear_color.z * clear_color.w;
    jwindow.g_MainWindowData.ClearValue.color.float32[3] = clear_color.w;
    if (!main_is_minimized)
      FrameRender(jwindow, main_draw_data);

    // Present Main Platform Window
    if (!main_is_minimized)
      FramePresent(jwindow);
  }
}
#pragma endregion

void setup_catppuccin_mocha_theme() {
  ImGuiStyle &style = ImGui::GetStyle();
  ImVec4 *colors = style.Colors;

  // Catppuccin Mocha Palette
  // --------------------------------------------------------
  const ImVec4 base = ImVec4(0.117f, 0.117f, 0.172f, 1.0f);     // #1e1e2e
  const ImVec4 mantle = ImVec4(0.109f, 0.109f, 0.156f, 1.0f);   // #181825
  const ImVec4 surface0 = ImVec4(0.200f, 0.207f, 0.286f, 1.0f); // #313244
  const ImVec4 surface1 = ImVec4(0.247f, 0.254f, 0.337f, 1.0f); // #3f4056
  const ImVec4 surface2 = ImVec4(0.290f, 0.301f, 0.388f, 1.0f); // #4a4d63
  const ImVec4 overlay0 = ImVec4(0.396f, 0.403f, 0.486f, 1.0f); // #65677c
  const ImVec4 overlay2 = ImVec4(0.576f, 0.584f, 0.654f, 1.0f); // #9399b2
  const ImVec4 text = ImVec4(0.803f, 0.815f, 0.878f, 1.0f);     // #cdd6f4
  const ImVec4 subtext0 = ImVec4(0.639f, 0.658f, 0.764f, 1.0f); // #a3a8c3
  const ImVec4 mauve = ImVec4(0.796f, 0.698f, 0.972f, 1.0f);    // #cba6f7
  const ImVec4 peach = ImVec4(0.980f, 0.709f, 0.572f, 1.0f);    // #fab387
  const ImVec4 yellow = ImVec4(0.980f, 0.913f, 0.596f, 1.0f);   // #f9e2af
  const ImVec4 green = ImVec4(0.650f, 0.890f, 0.631f, 1.0f);    // #a6e3a1
  const ImVec4 teal = ImVec4(0.580f, 0.886f, 0.819f, 1.0f);     // #94e2d5
  const ImVec4 sapphire = ImVec4(0.458f, 0.784f, 0.878f, 1.0f); // #74c7ec
  const ImVec4 blue = ImVec4(0.533f, 0.698f, 0.976f, 1.0f);     // #89b4fa
  const ImVec4 lavender = ImVec4(0.709f, 0.764f, 0.980f, 1.0f); // #b4befe

  // Main window and backgrounds
  colors[ImGuiCol_WindowBg] = base;
  colors[ImGuiCol_ChildBg] = base;
  colors[ImGuiCol_PopupBg] = surface0;
  colors[ImGuiCol_Border] = surface1;
  colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  colors[ImGuiCol_FrameBg] = surface0;
  colors[ImGuiCol_FrameBgHovered] = surface1;
  colors[ImGuiCol_FrameBgActive] = surface2;
  colors[ImGuiCol_TitleBg] = mantle;
  colors[ImGuiCol_TitleBgActive] = surface0;
  colors[ImGuiCol_TitleBgCollapsed] = mantle;
  colors[ImGuiCol_MenuBarBg] = mantle;
  colors[ImGuiCol_ScrollbarBg] = surface0;
  colors[ImGuiCol_ScrollbarGrab] = surface2;
  colors[ImGuiCol_ScrollbarGrabHovered] = overlay0;
  colors[ImGuiCol_ScrollbarGrabActive] = overlay2;
  colors[ImGuiCol_CheckMark] = green;
  colors[ImGuiCol_SliderGrab] = sapphire;
  colors[ImGuiCol_SliderGrabActive] = blue;
  colors[ImGuiCol_Button] = surface0;
  colors[ImGuiCol_ButtonHovered] = surface1;
  colors[ImGuiCol_ButtonActive] = surface2;
  colors[ImGuiCol_Header] = surface0;
  colors[ImGuiCol_HeaderHovered] = surface1;
  colors[ImGuiCol_HeaderActive] = surface2;
  colors[ImGuiCol_Separator] = surface1;
  colors[ImGuiCol_SeparatorHovered] = mauve;
  colors[ImGuiCol_SeparatorActive] = mauve;
  colors[ImGuiCol_ResizeGrip] = surface2;
  colors[ImGuiCol_ResizeGripHovered] = mauve;
  colors[ImGuiCol_ResizeGripActive] = mauve;
  colors[ImGuiCol_Tab] = surface0;
  colors[ImGuiCol_TabHovered] = surface2;
  colors[ImGuiCol_TabActive] = surface1;
  colors[ImGuiCol_TabUnfocused] = surface0;
  colors[ImGuiCol_TabUnfocusedActive] = surface1;
  colors[ImGuiCol_DockingPreview] = sapphire;
  colors[ImGuiCol_DockingEmptyBg] = base;
  colors[ImGuiCol_PlotLines] = blue;
  colors[ImGuiCol_PlotLinesHovered] = peach;
  colors[ImGuiCol_PlotHistogram] = teal;
  colors[ImGuiCol_PlotHistogramHovered] = green;
  colors[ImGuiCol_TableHeaderBg] = surface0;
  colors[ImGuiCol_TableBorderStrong] = surface1;
  colors[ImGuiCol_TableBorderLight] = surface0;
  colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = surface2;
  colors[ImGuiCol_DragDropTarget] = yellow;
  colors[ImGuiCol_NavHighlight] = lavender;
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.35f);
  colors[ImGuiCol_Text] = text;
  colors[ImGuiCol_TextDisabled] = subtext0;

  // Rounded corners
  style.WindowRounding = 6.0f;
  style.ChildRounding = 6.0f;
  style.FrameRounding = 4.0f;
  style.PopupRounding = 4.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabRounding = 4.0f;
  style.TabRounding = 4.0f;

  // Padding and spacing
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.FramePadding = ImVec2(5.0f, 3.0f);
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.IndentSpacing = 21.0f;
  style.ScrollbarSize = 14.0f;
  style.GrabMinSize = 10.0f;

  // Borders
  style.WindowBorderSize = 1.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupBorderSize = 1.0f;
  style.FrameBorderSize = 0.0f;
  style.TabBorderSize = 0.0f;
}

void SavePreference() {
  std::string save_path = Engine::GetConfigPath("imgui.ini").string();
  ImGui::SaveIniSettingsToDisk(save_path.c_str());
  spdlog::debug("SaveIniSettingsToDisk: {}", save_path);
}

void LoadPreference() {
  std::string save_path = Engine::GetConfigPath("imgui.ini").string();
  ImGui::LoadIniSettingsFromDisk(save_path.c_str());
  spdlog::debug("LoadIniSettingsFromDisk: {}", save_path);
}

int32_t JInit(JWindow &jwindow, JRWindowInit init) {
  spdlog::debug("Application Initialization: Editor");

  VInit();

  // Create window with Vulkan graphics context
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  spdlog::debug("\tmain_scale: {}", main_scale);
  jwindow.g_windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE |
                          SDL_WINDOW_HIGH_PIXEL_DENSITY | init.g_flags;
  jwindow.g_window = SDL_CreateWindow(
      "Dear ImGui SDL3+Vulkan example", (int)(init.g_width * main_scale),
      (int)(init.g_height * main_scale), jwindow.g_windowFlags);
  SDL_SetWindowMinimumSize(jwindow.g_window, init.g_width, init.g_height);
  if (jwindow.g_window == nullptr) {
    throw std::runtime_error(
        std::format("Error: SDL_CreateWindow(): {}\n", SDL_GetError()));
  }
  spdlog::debug("\tSDL window created");

  std::vector<const char *> extensions = VGetExtensions();
  SetupVulkan(jwindow, extensions);
  spdlog::debug("\tVulkan setup finish");

  // Create Window Surface
  VkSurfaceKHR surface;
  VkResult err;
  if (SDL_Vulkan_CreateSurface(jwindow.g_window, jwindow.g_Instance,
                               jwindow.g_Allocator, &surface) == 0) {
    throw std::runtime_error("Failed to create Vulkan surface.\n");
  }
  spdlog::debug("\tVulkan surface finish");

  // Create Framebuffers
  int32_t w, h;
  SDL_MaximizeWindow(jwindow.g_window);
  SDL_GetWindowSize(jwindow.g_window, &w, &h);
  ImGui_ImplVulkanH_Window *wd = &jwindow.g_MainWindowData;
  SetupVulkanWindow(jwindow, wd, surface, w, h);
  SDL_SetWindowPosition(jwindow.g_window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(jwindow.g_window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.IniFilename = Engine::GetConfigPath("imgui.ini").c_str();
  io.LogFilename = Engine::GetConfigPath("imgui.log").c_str();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable
  // Multi-Viewport / Platform Windows io.ConfigFlags |=
  // ImGuiConfigFlags_ViewportsNoTaskBarIcons; io.ConfigFlags |=
  // ImGuiConfigFlags_ViewportsNoMerge;

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  setup_catppuccin_mocha_theme();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(
      main_scale * 1.5f); // Bake a fixed style scale. (until we have a solution
                          // for dynamic style scaling, changing this requires
                          // resetting Style + calling this again)
  style.FontScaleDpi =
      main_scale *
      1.2f; // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes
            // this unnecessary. We leave both here for documentation purpose)
  io.ConfigDpiScaleFonts =
      true; // [Experimental] Automatically overwrite style.FontScaleDpi in
            // Begin() when Monitor DPI changes. This will scale fonts but _NOT_
            // scale sizes/padding for now.
  io.ConfigDpiScaleViewports =
      true; // [Experimental] Scale Dear ImGui and Platform Windows when Monitor
            // DPI changes.

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForVulkan(jwindow.g_window);
  ImGui_ImplVulkan_InitInfo init_info = {};
  // init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your
  // value of VkApplicationInfo::apiVersion, otherwise will default to header
  // version.
  init_info.Instance = jwindow.g_Instance;
  init_info.PhysicalDevice = jwindow.g_PhysicalDevice;
  init_info.Device = jwindow.g_Device;
  init_info.QueueFamily = jwindow.g_QueueFamily;
  init_info.Queue = jwindow.g_Queue;
  init_info.PipelineCache = jwindow.g_PipelineCache;
  init_info.DescriptorPool = jwindow.g_DescriptorPool;
  init_info.MinImageCount = jwindow.g_MinImageCount;
  init_info.ImageCount = wd->ImageCount;
  init_info.Allocator = jwindow.g_Allocator;
  init_info.PipelineInfoMain.RenderPass = wd->RenderPass;
  init_info.PipelineInfoMain.Subpass = 0;
  init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.CheckVkResultFn = check_vk_result;
  ImGui_ImplVulkan_Init(&init_info);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype
  // for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details. If you like the
  // default font but want it to scale better, consider using the 'ProggyVector'
  // from the same author!
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  style.FontSizeBase = 20.0f;
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
  // IM_ASSERT(font != nullptr);
  return 0;
}

void JDeInit(JWindow &jwindow) {
  spdlog::debug("Release Application Resources");
  // Cleanup
  // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your
  // SDL_AppQuit() function]
  VkResult err = vkDeviceWaitIdle(jwindow.g_Device);
  check_vk_result(err);
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  CleanupVulkanWindow(jwindow);
  CleanupVulkan(jwindow);

  SDL_DestroyWindow(jwindow.g_window);
  SDL_Quit();
}

int32_t JRInit(JWindowRender &jrwindow, JRWindowInit init) {
  spdlog::debug("Application Initialization: Render");
  jrwindow = JWindowRender();

  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  jrwindow.g_windowFlags = SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN |
                           SDL_WINDOW_HIGH_PIXEL_DENSITY | init.g_flags;
  jrwindow.g_window = SDL_CreateWindow(
      "Dear ImGui SDL3+Vulkan example", (int32_t)(init.g_width * main_scale),
      (int32_t)(init.g_height * main_scale), jrwindow.g_windowFlags);
  SDL_SetWindowMinimumSize(jrwindow.g_window, init.g_width, init.g_height);
  if (jrwindow.g_window == nullptr) {
    throw std::runtime_error(
        std::format("Error: SDL_CreateWindow(): {}\n", SDL_GetError()));
  }

  std::vector<const char *> extensions = VGetExtensions();
  SetupVulkan(jrwindow, extensions);
  return 0;
}

void JRDeInit(JWindowRender &jwindow) {}
} // namespace January::System
