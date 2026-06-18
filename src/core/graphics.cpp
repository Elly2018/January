#include "graphics.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <format>
#include <imgui_impl_vulkan.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

#ifdef APP_USE_VULKAN_DEBUG_REPORT
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
#endif

template <> struct fmt::formatter<VkResult> : fmt::formatter<int> {
  template <typename FormatContext>
  auto format(const VkResult &res, FormatContext &ctx) const {
    // You can either pass it down as an integer representation:
    return fmt::formatter<int>::format(static_cast<int>(res), ctx);

    // OR if you prefer strings automatically:
    // return fmt::formatter<string_view>::format(string_VkResult(res), ctx);
  }
};

namespace January {
void check_vk_result(VkResult err) { check_vk_result(err, 0); }
void check_vk_result(VkResult err, uint32_t level) {
  std::string msg = "";
  for (int32_t i = 0; i < level; i++) {
    msg.append("\t");
  }
  if (err == VK_SUCCESS)
    return;
  spdlog::critical("{}[vulkan] Error: VkResult = {}", msg,
                   static_cast<int32_t>(err));
  if (err < 0)
    abort();
}

bool IsExtensionAvailable(const std::vector<VkExtensionProperties> &properties,
                          const char *extension) {
  for (const VkExtensionProperties &p : properties)
    if (strcmp(p.extensionName, extension) == 0)
      return true;
  return false;
}

#ifdef APP_USE_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL
debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
             uint64_t object, size_t location, int32_t messageCode,
             const char *pLayerPrefix, const char *pMessage, void *pUserData) {
  (void)flags;
  (void)object;
  (void)location;
  (void)messageCode;
  (void)pUserData;
  (void)pLayerPrefix; // Unused arguments
  fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n",
          objectType, pMessage);
  return VK_FALSE;
}
#endif // APP_USE_VULKAN_DEBUG_REPORT

void VPrintDeviceProperty(uint32_t index, VkPhysicalDevice &p_device) {
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(p_device, &deviceProperties);

  spdlog::info("Physical Device [{}]", index);
  spdlog::info("\tName:", deviceProperties.deviceName);
  spdlog::info("\tAPI Version: {}.{}.{}",
               VK_API_VERSION_MAJOR(deviceProperties.apiVersion),
               VK_API_VERSION_MINOR(deviceProperties.apiVersion),
               VK_API_VERSION_PATCH(deviceProperties.apiVersion));

  spdlog::info("Physical Device Type");
  switch (deviceProperties.deviceType) {
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    spdlog::info("Integrated GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    spdlog::info("Discrete GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    spdlog::info("Virtual GPU");
    break;
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    spdlog::info("CPU / Software");
    break;
  default:
    spdlog::info("Other / Unknown");
    break;
  }
}

std::vector<const char *> VGetExtensions() {
  std::vector<const char *> r = std::vector<const char *>();
  uint32_t sdl_extensions_count = 0;
  const char *const *sdl_extensions =
      SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
  for (uint32_t n = 0; n < sdl_extensions_count; n++)
    r.push_back(sdl_extensions[n]);
  return r;
}

void VInit() {
  SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    // printf("Error: SDL_Init(): %s\n", SDL_GetError());
    throw std::runtime_error(
        std::format("Error: SDL_Init(): {}\n", SDL_GetError()));
  }
  spdlog::trace("SDL init successfully");
  spdlog::trace("\tTags:");
  spdlog::trace("\t\tSDL_INIT_VIDEO");
  spdlog::trace("\t\tSDL_INIT_GAMEPAD");
}

void VCreateInstance(std::vector<const char *> extensions, VkInstance &instance,
                     VkAllocationCallbacks *allocation) {
  spdlog::trace("Trying init vulkan instance...");
  VkInstanceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

  // Enumerate available extensions
  uint32_t properties_count;
  std::vector<VkExtensionProperties> properties;
  vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
  properties.resize(properties_count);
  VkResult err = vkEnumerateInstanceExtensionProperties(
      nullptr, &properties_count, properties.data());
  check_vk_result(err, 1);

  // Enable required extensions
  if (IsExtensionAvailable(
          properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    extensions.push_back(
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
  if (IsExtensionAvailable(properties,
                           VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    create_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
  }
#endif

  // Enabling validation layers
#ifdef APP_USE_VULKAN_DEBUG_REPORT
  const char *layers[] = {"VK_LAYER_KHRONOS_validation"};
  create_info.enabledLayerCount = 1;
  create_info.ppEnabledLayerNames = layers;
  extensions.push_back("VK_EXT_debug_report");
#endif

  // Create Vulkan Instance
  create_info.enabledExtensionCount = (uint32_t)extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();
  err = vkCreateInstance(&create_info, allocation, &instance);
  check_vk_result(err, 1);
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
  volkLoadInstance(instance);
#endif
  // Setup the debug report callback
#ifdef APP_USE_VULKAN_DEBUG_REPORT
  auto f_vkCreateDebugReportCallbackEXT =
      (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
          instance, "vkCreateDebugReportCallbackEXT");
  IM_ASSERT(f_vkCreateDebugReportCallbackEXT != nullptr);
  VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
  debug_report_ci.sType =
      VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
  debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
                          VK_DEBUG_REPORT_WARNING_BIT_EXT |
                          VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
  debug_report_ci.pfnCallback = debug_report;
  debug_report_ci.pUserData = nullptr;
  err = f_vkCreateDebugReportCallbackEXT(instance, &debug_report_ci, allocation,
                                         &g_DebugReport);
  check_vk_result(err, 1);
#endif
  spdlog::trace("Trying init vulkan instance...Finished");
}

void VGetPhysicalDeviceFront(VkInstance &instance, VkPhysicalDevice &p_device) {
  std::vector<VkPhysicalDevice> r = std::vector<VkPhysicalDevice>();
  VGetPhysicalDeviceAll(instance, r);
  p_device = r.front();
}

void VGetPhysicalDeviceAll(VkInstance &instance,
                           std::vector<VkPhysicalDevice> &arr) {
  uint32_t deviceCount = 0;
  VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (result != VK_SUCCESS || deviceCount == 0) {
    spdlog::critical("Failed to find GPUs with Vulkan support!");
    return;
  }
  arr.resize(deviceCount);

  // 3. Fetch the actual physical device handles
  result = vkEnumeratePhysicalDevices(instance, &deviceCount, arr.data());
  if (result != VK_SUCCESS) {
    spdlog::critical("Failed to enumerate physical devices!");
    return;
  }
}

uint32_t VGetQueueFamily(VkPhysicalDevice &p_device) {
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount,
                                           nullptr);

  if (queueFamilyCount == 0) {
    spdlog::critical("No queue families found for this device.");
    return (uint32_t)-1;
  }

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(p_device, &queueFamilyCount,
                                           queueFamilies.data());

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      return i;
    }
  }

  spdlog::critical(
      "No queue families found are VK_QUEUE_GRAPHICS_BIT for this device.");
  return (uint32_t)-1;
}

void VGetLogicalDevice(VkPhysicalDevice &p_device, uint32_t &queue_family_index,
                       VkAllocationCallbacks *allocation, VkQueue &queue,
                       VkDevice &device) {
  std::vector<const char *> device_extensions;
  device_extensions.push_back("VK_KHR_swapchain");

  // Enumerate physical device extension
  uint32_t properties_count;
  std::vector<VkExtensionProperties> properties;
  vkEnumerateDeviceExtensionProperties(p_device, nullptr, &properties_count,
                                       nullptr);
  properties.resize(properties_count);
  vkEnumerateDeviceExtensionProperties(p_device, nullptr, &properties_count,
                                       properties.data());
#ifdef VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME
  if (IsExtensionAvailable(properties,
                           VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
    device_extensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
#endif

  const float queue_priority[] = {1.0f};
  VkDeviceQueueCreateInfo queue_info[1] = {};
  queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queue_info[0].queueFamilyIndex = queue_family_index;
  queue_info[0].queueCount = 1;
  queue_info[0].pQueuePriorities = queue_priority;
  VkDeviceCreateInfo create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
  create_info.pQueueCreateInfos = queue_info;
  create_info.enabledExtensionCount = (uint32_t)device_extensions.size();
  create_info.ppEnabledExtensionNames = device_extensions.data();
  VkResult err = vkCreateDevice(p_device, &create_info, allocation, &device);
  check_vk_result(err);
  vkGetDeviceQueue(device, queue_family_index, 0, &queue);
}

void VGetDescriptionPool(VkDevice &device, VkAllocationCallbacks *allocation,
                         VkDescriptorPool &pool) {
  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
       IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE},
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 0;
  for (VkDescriptorPoolSize &pool_size : pool_sizes)
    pool_info.maxSets += pool_size.descriptorCount;
  pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
  pool_info.pPoolSizes = pool_sizes;
  VkResult err = vkCreateDescriptorPool(device, &pool_info, allocation, &pool);
  check_vk_result(err);
}

std::vector<uint32_t> VCompileGLSLToSPIRV(const std::string &sourceName,
                                          shaderc_shader_kind shaderKind,
                                          const std::string &glslSource,
                                          bool optimize) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  if (optimize) {
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
  } else {
    options.SetOptimizationLevel(shaderc_optimization_level_zero);
  }

  options.SetTargetEnvironment(shaderc_target_env_vulkan,
                               shaderc_env_version_vulkan_1_3);
  shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
      glslSource, shaderKind, sourceName.c_str(), options);
  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    spdlog::error("Shaderc Error in [{}]", result.GetErrorMessage());
    return {};
  }
  return std::vector<uint32_t>(result.cbegin(), result.cend());
}

VkShaderModule VCreateShaderModule(VkDevice &logicalDevice,
                                   const std::vector<uint32_t> &spirvCode) {
  if (spirvCode.empty()) {
    spdlog::error(
        "Cannot create shader module: SPIR-V binary vector is empty.");
    return VK_NULL_HANDLE;
  }
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
  createInfo.pCode = spirvCode.data();
  VkShaderModule shaderModule = VK_NULL_HANDLE;
  VkResult result =
      vkCreateShaderModule(logicalDevice, &createInfo, nullptr, &shaderModule);

  if (result != VK_SUCCESS) {
    spdlog::error("Failed to create Vulkan VkShaderModule! Error code: {}",
                  static_cast<int>(result));
    return VK_NULL_HANDLE;
  }

  return shaderModule;
}

VkPipelineLayout
VCreatePipelineLayout(VkDevice device,
                      VkDescriptorSetLayout descriptorSetLayout) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

  // 1. Descriptor Set Layouts (For Uniform Buffers, Samplers, Textures)
  if (descriptorSetLayout != VK_NULL_HANDLE) {
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
  } else {
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr; // No uniforms used
  }

  // 2. Push Constants (For small, fast-updating pieces of data, e.g., a single
  // matrix)
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Omitted for simplicity

  // 3. Creation
  VkPipelineLayout pipelineLayout;
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  return pipelineLayout;
}

VkPipeline VCreateGraphicsPipeline(VkDevice device,
                                   VkShaderModule vertShaderModule,
                                   VkShaderModule fragShaderModule,
                                   VkPipelineLayout pipelineLayout,
                                   VkRenderPass renderPass) {
  // 1. Shader Stages Configuration
  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main"; // Entry point function in the shader

  VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  // 2. Vertex Input State (Assuming hardcoded vertex data in shaders for
  // simplicity)
  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  // 3. Input Assembly State
  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // 4. Viewport and Scissor (Using Dynamic State for flexibility)
  // This allows you to change resize handling without recreating the whole
  // pipeline.
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT,
                                               VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  // 5. Rasterization State
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  // 6. Multisampling State (Anti-aliasing disabled for now)
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // 7. Color Blending State (Alpha blending configuration)
  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_TRUE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  colorBlendAttachment.dstColorBlendFactor =
      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;

  // 8. The Main Pipeline Uber-Struct
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr; // Omitted for basic 2D layout
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicState;

  // Bind layouts and render passes
  pipelineInfo.layout = pipelineLayout;
  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass =
      0; // Index of the subpass where this pipeline will be used

  // Base pipeline derivatives (not using them here)
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.basePipelineIndex = -1;

  // 9. Creation
  VkPipeline graphicsPipeline;
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  return graphicsPipeline;
}
} // namespace January
