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
#include <shaderc/shaderc.hpp>

// Volk headers
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE

namespace January {
    void check_vk_result(VkResult err);
    void check_vk_result(VkResult err, uint32_t level);
    bool IsExtensionAvailable(const std::vector<VkExtensionProperties> &properties, const char *extension);
#ifdef APP_USE_VULKAN_DEBUG_REPORT
    VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData);
#endif

    /**
     * @brief Print the physical device information on the log 
     */ 
    void VPrintDeviceProperty(uint32_t index, VkPhysicalDevice &p_device);
    /**
     * @brief Quickly get extensions support on user's system
     * 
     * @return The text array of extensions
     */
    std::vector<const char*> VGetExtensions();

    /**
     * @brief Setup SDL
     * This should be called before everything
     */
    void VInit();
    /**
     * @brief Create vulkan instance
     *
     * @tparam extensions Better call VGetExtensions() first to fill the input
     * @tparam instance The return vulkan instance handle
     * @tparam allocation The return vulkan location callback allocation handler
     */
    void VCreateInstance(std::vector<const char*> extensions, VkInstance& instance, VkAllocationCallbacks* allocation);
    /**
     * @brief Get the first physical device
     * 
     * @tparam instance The created vulkan handle
     * @tparam p_device The return device handler
     */
    void VGetPhysicalDeviceFront(VkInstance& instance, VkPhysicalDevice& p_device);
    /**
     * @brief Get the all physical device
     *
     * @tparam instance The created vulkan handle
     * @tparam arr The return devices handler
     */
    void VGetPhysicalDeviceAll(VkInstance& instance, std::vector<VkPhysicalDevice>& arr);
    /**
     * @brief Get graphics queue from the phisical device
     *
     * @tparam p_device The physical device handler
     * 
     * @return The first queue index with VK_QUEUE_GRAPHICS_BIT enable
     */
    uint32_t VGetQueueFamily(VkPhysicalDevice& p_device);
    /**
     * @brief Create Logical Device (with 1 queue)
     *
     * @tparam p_device The physical device input
     * @tparam queue_family_index The queue index
     * @tparam allocation The location callback handler
     * @tparam queue The return queue handler
     * @tparam device The return logical device
     */
    void VGetLogicalDevice(VkPhysicalDevice& p_device, uint32_t& queue_family_index, VkAllocationCallbacks* allocation, VkQueue& queue, VkDevice& device);
    /**
     * @brief Create Descriptor Pool
     * If you wish to load e.g. additional textures you may need to alter pools sizes and maxSets.
     *
     * @tparam device The logical device handler
     * @tparam allocation The location callback handler
     * @tparam pool The return pool
     */
    void VGetDescriptionPool(VkDevice& device, VkAllocationCallbacks* allocation, VkDescriptorPool& pool);
    /**
     * @brief Compile shader
     *
     * @tparam sourceName The name of the shader
     * @tparam shaderKind Shader type
     * @tparam glslSource Shader string data
     * @tparam optimize Optimization flag
     *
     * @return The binary data compiled result
     */
    std::vector<uint32_t> VCompileGLSLToSPIRV(const std::string& sourceName, shaderc_shader_kind shaderKind, const std::string& glslSource, bool optimize = true);
    VkShaderModule VCreateShaderModule(VkDevice& logicalDevice, const std::vector<uint32_t>& spirvCode);
    VkPipelineLayout VCreatePipelineLayout(VkDevice device, VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE);
    VkPipeline VCreateGraphicsPipeline(VkDevice device, VkShaderModule vertShaderModule, VkShaderModule fragShaderModule,VkPipelineLayout pipelineLayout, VkRenderPass renderPass);
}

#endif