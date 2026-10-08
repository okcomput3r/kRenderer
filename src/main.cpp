#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory.h>
#include <utils/logs.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>

// change later
#include <fstream>
#include <vector>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

enum RET_CODE {

  RETURN_SUCCES,
  RETURN_FAILED

};

const char *requiredDeviceExtension[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
uint32_t requiredDeviceExtensionCount = 1;

GLFWwindow *window = nullptr;
VkSurfaceKHR surface = nullptr;

VkInstance instance;

VkPhysicalDevice physicalDevice;
VkDevice device;

int graphicsFamilyIndex = -1;
VkQueue graphicsQueue;

VkApplicationInfo appInfo{};
VkInstanceCreateInfo createInfo{};

VkSwapchainKHR swapchain;
uint32_t imageCount;
VkImage *swapChainImages;
VkImageView *swapChainImageViews;

uint32_t desiredImageCount = 3;

// surface Extent
VkExtent2D surfaceExtent;

// surface formats
uint32_t surfaceFormatIndex = 0;
VkSurfaceFormatKHR *surfaceFormats;

VkShaderModule shaderModule;
VkPipelineLayout pipelineLayout = nullptr;

VkPipeline graphicsPipeline = nullptr;

VkCommandPool commandPool = nullptr;
VkCommandBuffer commandBuffer = nullptr;

VkSemaphore presentCompleteSemaphore = nullptr;
VkSemaphore renderFinishedSemaphore = nullptr;

VkFence drawFence = nullptr;

RET_CODE init_glfw_window() {

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

  if (window == nullptr) {
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE allocate_instance_vk() {

  // App info
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Kilby Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions =
      glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  uint32_t vulkanExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount,
                                         nullptr);

  VkExtensionProperties *vulkanExtensions =
      static_cast<VkExtensionProperties *>(
          malloc(sizeof(VkExtensionProperties) * vulkanExtensionCount));
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount,
                                         vulkanExtensions);

  uint8_t supported = 0;
  for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
    supported = 0;
    for (uint32_t j = 0; j < vulkanExtensionCount; ++j) {
      if (strcmp(glfwExtensions[i], vulkanExtensions[j].extensionName) == 0) {
        supported = 1;
        break;
      }
    }
    if (!supported) {
      free(vulkanExtensions);
      return RET_CODE::RETURN_FAILED;
    }
  }

  free(vulkanExtensions);

  // Create Info
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  // Instance

  if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE allocate_surface_vk() {

  // Surface

  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != 0) {
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_physical_device_vk() {

  // Physical Device

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    return RET_CODE::RETURN_FAILED;
  }

  VkPhysicalDevice *p_physicalDevices = nullptr;
  p_physicalDevices = static_cast<VkPhysicalDevice *>(
      malloc(sizeof(VkPhysicalDevice) * deviceCount));

  vkEnumeratePhysicalDevices(instance, &deviceCount, p_physicalDevices);

  uint8_t ext_supported = 0;
  uint32_t device_index = 0;
  for (int i = 0; i < deviceCount; ++i) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(p_physicalDevices[i], nullptr,
                                         &extensionCount, nullptr);

    VkExtensionProperties *aviableExtensions =
        static_cast<VkExtensionProperties *>(
            malloc(sizeof(VkExtensionProperties) * extensionCount));

    vkEnumerateDeviceExtensionProperties(p_physicalDevices[i], nullptr,
                                         &extensionCount, aviableExtensions);

    for (int j = 0; j < requiredDeviceExtensionCount; ++j) {
      ext_supported = 0;
      for (int w = 0; w < extensionCount; ++w) {
        if (strcmp(requiredDeviceExtension[j],
                   aviableExtensions[w].extensionName) == 0) {
          ext_supported = 1;
          break;
        }
      }
      if (!ext_supported) {
        break;
      }
    }

    free(aviableExtensions);

    if (ext_supported) {
      device_index = i;
      break;
    } else {
      return RET_CODE::RETURN_FAILED;
    }
  }

  physicalDevice = p_physicalDevices[device_index];

  free(p_physicalDevices);

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_logic_device_vk() {

  // Family Queues

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  VkQueueFamilyProperties *p_queueFamilies = nullptr;
  p_queueFamilies = static_cast<VkQueueFamilyProperties *>(
      malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount));

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           p_queueFamilies);

  VkBool32 result;
  for (int i = 0; i < queueFamilyCount; ++i) {
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &result);
    if (p_queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
        result == VK_TRUE) {
      graphicsFamilyIndex = i;
      break;
    }
  }

  free(p_queueFamilies);

  if (graphicsFamilyIndex == -1) {
    return RET_CODE::RETURN_FAILED;
  }

  // Logic Device

  float queuePriority = 0.5f;

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = graphicsFamilyIndex;
  queueCreateInfo.queueCount = 1;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures2 features2 = {};
  features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  features2.features.geometryShader = VK_TRUE;
  VkPhysicalDeviceVulkan11Features features11 = {};
  features11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  features11.shaderDrawParameters = VK_TRUE;
  features11.pNext = &features2;
  VkPhysicalDeviceVulkan13Features features13 = {};
  features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  features13.dynamicRendering = VK_TRUE;
  features13.pNext = &features11;

  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensionCount;
  deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtension;
  deviceCreateInfo.pNext = &features13;

  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) !=
      VK_SUCCESS) {
    return RET_CODE::RETURN_FAILED;
  }
  // Graphics Queue

  vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &graphicsQueue);

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_swapchain_vk() {

  // Query Basic capabilities (min/max number of images in swapchain, width
  // height of images)
  //
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                            &surfaceCapabilities);

  // Query and Choose surface formats (pixel format, color space )
  //
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                       &surfaceFormatCount, nullptr);
  surfaceFormats = static_cast<VkSurfaceFormatKHR *>(
      malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount));
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                       &surfaceFormatCount, surfaceFormats);

  for (int i = 0; i < surfaceFormatCount; ++i) {
    if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      surfaceFormatIndex = i;
      break;
    }
  }

  // Query and Choose Presentation modes (Fifo, mailBox)
  //
  uint32_t presentationModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                            &presentationModeCount, nullptr);
  VkPresentModeKHR *presentationModes = static_cast<VkPresentModeKHR *>(
      malloc(sizeof(VkPresentModeKHR) * presentationModeCount));
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      physicalDevice, surface, &presentationModeCount, presentationModes);

  uint32_t PresentIndex = 0;
  for (int i = 0; i < presentationModeCount; ++i) {
    if (presentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      PresentIndex = i;
      break;
    }
  }

  // choose swap extent
  //

  if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
    surfaceExtent = surfaceCapabilities.currentExtent;

  } else {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Clamp to [min image extent, max image extent]

    if (width <= surfaceCapabilities.minImageExtent.width)
      width = surfaceCapabilities.minImageExtent.width;
    else if (width >= surfaceCapabilities.maxImageExtent.width)
      width = surfaceCapabilities.maxImageExtent.width;

    if (height <= surfaceCapabilities.minImageExtent.height)
      height = surfaceCapabilities.minImageExtent.height;
    else if (height >= surfaceCapabilities.maxImageExtent.height)
      height = surfaceCapabilities.maxImageExtent.height;

    surfaceExtent.width = width;
    surfaceExtent.height = height;
  }

  uint32_t minImageCount =
      (desiredImageCount > surfaceCapabilities.minImageCount)
          ? desiredImageCount
          : surfaceCapabilities.minImageCount;

  if ((0 < surfaceCapabilities.maxImageCount) &&
      (surfaceCapabilities.maxImageCount < minImageCount)) {
    minImageCount = surfaceCapabilities.maxImageCount;
  }

  // Create the swapchain

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.surface = surface;
  swapchainCreateInfo.minImageCount = minImageCount;
  swapchainCreateInfo.imageFormat = surfaceFormats[surfaceFormatIndex].format;
  swapchainCreateInfo.imageColorSpace =
      surfaceFormats[surfaceFormatIndex].colorSpace;
  swapchainCreateInfo.imageExtent = surfaceExtent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = presentationModes[PresentIndex];
  swapchainCreateInfo.clipped = true;

  if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) !=
      VK_SUCCESS)
    return RET_CODE::RETURN_FAILED;

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  swapChainImages =
      static_cast<VkImage *>(malloc(sizeof(VkImage) * imageCount));
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages);

  // Create the image views

  swapChainImageViews =
      static_cast<VkImageView *>(malloc(sizeof(VkImageView) * imageCount));

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format = surfaceFormats[surfaceFormatIndex].format;
  imageViewCreateInfo.subresourceRange = {.aspectMask =
                                              VK_IMAGE_ASPECT_COLOR_BIT,
                                          .levelCount = 0,
                                          .layerCount = 1};
  imageViewCreateInfo.components = {
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

  for (int i = 0; i < imageCount; ++i) {
    imageViewCreateInfo.image = swapChainImages[i];
    vkCreateImageView(device, &imageViewCreateInfo, nullptr,
                      &swapChainImageViews[i]);
  }
  free(presentationModes);

  return RET_CODE::RETURN_SUCCES;
}

// CHANGE LATER TODO

static std::vector<char> readFile(const std::string &filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    std::cout << "failed to open file!" << std::endl;
  }

  std::vector<char> buffer(file.tellg());
  file.seekg(0, std::ios::beg);
  file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

  file.close();

  return buffer;
}

RET_CODE setup_graphic_pipeline_vk() {

  std::vector<char> shaderCode = readFile("./slang.spv");

  // Create shader module

  VkShaderModuleCreateInfo createInfo{};
  createInfo.codeSize = shaderCode.size() * sizeof(char);
  createInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());

  vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

  // Instanciate shader stages

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
  vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = shaderModule;
  vertexShaderStageInfo.pName = "vertMain";

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = shaderModule;
  fragmentShaderStageInfo.pName = "fragMain";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo,
                                                    fragmentShaderStageInfo};

  // Construct pipeline

  VkPipelineVertexInputStateCreateInfo
      vertexInputInfo{}; // empty for now (dynamic rendering)

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.dynamicStateCount = 2; // hardcode TODO
  dynamicState.pDynamicStates = dynamicStates;

  // Will be used later at draw time TODO

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;

  VkPipelineMultisampleStateCreateInfo multisampling{}; // disabled for now
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.sampleShadingEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachement{};
  colorBlendAttachement.blendEnable = VK_FALSE;
  colorBlendAttachement.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachement;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);

  VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo{};
  pipelineRenderingCreateInfo.colorAttachmentCount = 1;
  pipelineRenderingCreateInfo.pColorAttachmentFormats =
      &surfaceFormats[surfaceFormatIndex].format;

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStages;
  pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  pipelineCreateInfo.pViewportState = &viewportState;
  pipelineCreateInfo.pRasterizationState = &rasterizer;
  pipelineCreateInfo.pMultisampleState = &multisampling;
  pipelineCreateInfo.pColorBlendState = &colorBlending;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.layout = pipelineLayout;
  pipelineCreateInfo.renderPass = nullptr;
  pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo,
                                nullptr, &graphicsPipeline) != VK_SUCCESS) {
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_command_pool_vk() {

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = graphicsFamilyIndex;

  vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.commandPool = commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE record_command_buffer(uint32_t imageIndex) {

  VkCommandBufferBeginInfo beginInfo{};
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  // Transition imagen layout from undefined to eColorAttatchmentOptimal

  VkImageMemoryBarrier2 barrier{};
  barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.srcAccessMask = {};
  barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = swapChainImages[imageIndex];
  barrier.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = 0,
                              .levelCount = 1,
                              .baseArrayLayer = 0,
                              .layerCount = 1};

  VkDependencyInfo dependency_info = {};
  dependency_info.dependencyFlags = {};
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &barrier;

  vkCmdPipelineBarrier2(commandBuffer, &dependency_info);

  // Setup clear color (black background)

  VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

  VkRenderingAttachmentInfo attachmentInfo{};
  attachmentInfo.imageView = swapChainImageViews[imageIndex];
  attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentInfo.clearValue = clearColor;

  // Set rendering info & start recording

  VkRenderingInfo renderingInfo = {};
  renderingInfo.renderArea = {.offset = {0, 0}, .extent = surfaceExtent};
  renderingInfo.layerCount = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments = &attachmentInfo;

  vkCmdBeginRendering(commandBuffer, &renderingInfo);

  // Bind GRAPHIC pipeline and set ViewPort and Scissors
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    graphicsPipeline);

  VkViewport viewPort = {0.0f,
                         0.0f,
                         static_cast<float>(surfaceExtent.width),
                         static_cast<float>(surfaceExtent.height),
                         0.0f,
                         1.0f};
  vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

  VkRect2D scissors = {VkOffset2D{0, 0}, surfaceExtent};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissors);

  // DRAW THE TRIANGLE GAD DAMN

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  // End rendering and transition to PresentSrcKHR
  vkCmdEndRendering(commandBuffer);

  barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
  barrier.dstAccessMask = {};
  barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = swapChainImages[imageIndex];
  barrier.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = 0,
                              .levelCount = 1,
                              .baseArrayLayer = 0,
                              .layerCount = 1};

  dependency_info.dependencyFlags = {};
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers = &barrier;

  vkCmdPipelineBarrier2(commandBuffer, &dependency_info);

  // End commandBuffer
  vkEndCommandBuffer(commandBuffer);

  return RET_CODE::RETURN_SUCCES;
}

void create_sync_objects() {

  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                    &presentCompleteSemaphore);
  vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                    &renderFinishedSemaphore);

  VkFenceCreateInfo fenceCreateInfo = {.flags = VK_FENCE_CREATE_SIGNALED_BIT};
  vkCreateFence(device, &fenceCreateInfo, nullptr, &drawFence);
}

RET_CODE init_Vk() {

  if (allocate_instance_vk() != RET_CODE::RETURN_SUCCES)
    return RET_CODE::RETURN_FAILED;

  if (allocate_surface_vk() != RET_CODE::RETURN_SUCCES)
    return RET_CODE::RETURN_FAILED;

  if (setup_physical_device_vk() != RET_CODE::RETURN_SUCCES)
    return RET_CODE::RETURN_FAILED;

  if (setup_logic_device_vk() != RET_CODE::RETURN_SUCCES)
    return RET_CODE::RETURN_FAILED;

  if (setup_swapchain_vk() != RET_CODE::RETURN_SUCCES)
    return RET_CODE::RETURN_FAILED;

  if (setup_graphic_pipeline_vk() != RET_CODE::RETURN_SUCCES) {
    return RET_CODE::RETURN_FAILED;
  }

  setup_command_pool_vk();
  create_sync_objects();

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE draw_frame_vk() {

  VkResult result = vkWaitForFences(device, 1, &drawFence, VK_TRUE, UINT64_MAX);
  if (result != VK_SUCCESS) {
    return RET_CODE::RETURN_FAILED;
  }
  vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, presentCompleteSemaphore,
                        nullptr, &imageCount);

  record_command_buffer(imageCount);

  VkPipelineStageFlags waitDestinationStageMask = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo submitInfo{};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
  submitInfo.pWaitDstStageMask = &waitDestinationStageMask;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

  vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFence);

  VkPresentInfoKHR presentInfoKHR{};
  presentInfoKHR.waitSemaphoreCount = 1;
  presentInfoKHR.pWaitSemaphores = &renderFinishedSemaphore;
  presentInfoKHR.swapchainCount = 1;
  presentInfoKHR.pSwapchains = &swapchain;
  presentInfoKHR.pImageIndices = &imageCount;

  presentInfoKHR.pResults = nullptr;

  vkQueuePresentKHR(graphicsQueue, &presentInfoKHR);

  return RET_CODE::RETURN_SUCCES;
}

void entrypoint_mainloop_Vk() {

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    draw_frame_vk();
  }
}

void cleanup_Vk() {

  // cleanup swapchain info
  free(swapChainImages);
  free(swapChainImageViews);
  free(surfaceFormats);

  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}

int main(int argc, char **argv) {

  init_glfw_window();

  std::cout << "starting vulkan" << std::endl;

  if (init_Vk() == RET_CODE::RETURN_FAILED) {
    std::cout << "failed to init vulkan\n" << std::endl;
  }
  std::cout << "vulkan setup is done" << std::endl;

  entrypoint_mainloop_Vk();
  cleanup_Vk();

  return 0;
}
