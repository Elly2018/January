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
#ifndef SYSTEM_VULKAN_HELPER_H
#define SYSTEM_VULKAN_HELPER_H
#include <vector>
#include <vulkan/vulkan.h>

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
    void VPrintDeviceProperty(VkPhysicalDevice& p_device);
    std::vector<const char*> VGetExtensions();

    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    void VInit();
    void VCreateInstance(std::vector<const char*> extensions, VkInstance& instance, VkAllocationCallbacks* allocation);
    void VGetPhysocalDeviceFront(VkInstance& instance, VkPhysicalDevice& p_device);
    void VGetPhysocalDeviceAll(VkInstance& instance, std::vector<VkPhysicalDevice>& arr);
    // Create Logical Device (with 1 queue)
    uint32_t VGetQueueFamily(VkPhysicalDevice& p_device);
    void VGetLogicalDevice(VkPhysicalDevice& p_device, VkQueue& queue, uint32_t& queue_family_index, VkAllocationCallbacks* allocation, VkDevice& device);
    // Create Descriptor Pool
    // If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
    void VGetDescriptionPool(VkDevice& device, VkAllocationCallbacks* allocation, VkDescriptorPool& pool);
}

#endif