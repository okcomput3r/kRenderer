#include <stdio.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

const char* requiredDeviceExtension[] = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
} ;

uint32_t requiredDeviceExtensionCount = 1;

GLFWwindow *window = nullptr;
VkSurfaceKHR surface = nullptr;

VkInstance instance;

VkPhysicalDevice physicalDevice;
VkDevice device;

VkQueue graphicsQueue;

VkApplicationInfo appInfo {};
VkInstanceCreateInfo createInfo {};

enum RET_CODE {

  RETURN_SUCCES,
  RETURN_FAILED

};

RET_CODE init_glfw_window(){

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow( WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

  if(window == nullptr){
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE allocate_instance_vk(){

  // App info
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Engine";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Kilby Engine";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;


  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  uint32_t vulkanExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);

  VkExtensionProperties *vulkanExtensions = static_cast<VkExtensionProperties*>(malloc(sizeof(VkExtensionProperties) * vulkanExtensionCount));
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, vulkanExtensions);

  uint8_t supported = 0;
  for (uint32_t i = 0; i < glfwExtensionCount; ++i){
    supported = 0;
    for (uint32_t j = 0; j < vulkanExtensionCount; ++j)
    {
      if(strcmp(glfwExtensions[i], vulkanExtensions[j].extensionName) == 0){
        supported = 1;
        break;
      }
    }
    if (!supported){
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

RET_CODE allocate_surface_vk(){

  // Surface

  if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != 0){
    return RET_CODE::RETURN_FAILED;
  }

  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_physical_device_vk(){

  // Physical Device

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    return RET_CODE::RETURN_FAILED;
  }

  VkPhysicalDevice *p_physicalDevices = nullptr;
  p_physicalDevices = static_cast<VkPhysicalDevice*>(malloc(sizeof(VkPhysicalDevice) * deviceCount));

  vkEnumeratePhysicalDevices(instance, &deviceCount, p_physicalDevices);

  uint8_t ext_supported = 0;
  uint32_t device_index = 0;
  for (int i = 0; i < deviceCount; ++i){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(p_physicalDevices[i], nullptr, &extensionCount, nullptr);

    VkExtensionProperties *aviableExtensions = static_cast<VkExtensionProperties*>(malloc(sizeof(VkExtensionProperties)*extensionCount));

    vkEnumerateDeviceExtensionProperties(p_physicalDevices[i], nullptr, &extensionCount, aviableExtensions);

    for (int j = 0; j < requiredDeviceExtensionCount; ++j)
    {
      ext_supported = 0;
      for (int w = 0; w < extensionCount; ++w){
        if (strcmp(requiredDeviceExtension[j], aviableExtensions[w].extensionName) == 0)
        {
          ext_supported = 1;
          break;
        }
      }
      if (!ext_supported) {
        break;
      }

    }

    free(aviableExtensions);
 
    if (ext_supported){
      device_index = i;
      break;
    }
    else {
      return RET_CODE::RETURN_FAILED;
    }
  }

  physicalDevice = p_physicalDevices[device_index];

  free(p_physicalDevices);

  return RET_CODE::RETURN_SUCCES;
}


RET_CODE setup_logic_device_vk(){

  // Family Queues

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  VkQueueFamilyProperties *p_queueFamilies = nullptr;
  p_queueFamilies = static_cast<VkQueueFamilyProperties*>(malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount));

  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, p_queueFamilies);

  VkBool32 result;
  int graphicsFamilyIndex = -1;
  for(int i = 0; i < queueFamilyCount; ++i)
  {
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice,i, surface, &result);
    if (p_queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && result == VK_TRUE) {
      graphicsFamilyIndex = i;
      break;
    }
  }

  free(p_queueFamilies);

  if (graphicsFamilyIndex == -1){
    return RET_CODE::RETURN_FAILED;
  }

  // Logic Device

  float queuePriority = 0.5f;

  VkDeviceQueueCreateInfo queueCreateInfo {};
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

  VkDeviceCreateInfo deviceCreateInfo {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensionCount;
  deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtension;
  deviceCreateInfo.pNext = &features13;

  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
    return RET_CODE::RETURN_FAILED;
  }
  // Graphics Queue

  vkGetDeviceQueue(device, graphicsFamilyIndex, 0, &graphicsQueue);


  return RET_CODE::RETURN_SUCCES;
}

RET_CODE setup_swapchain_vk(){

  // Query Basic capabilities (min/max number of images in swapchain, width height of images)
  //
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);


  // Query and Choose surface formats (pixel format, color space )
  //
  uint32_t surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
  VkSurfaceFormatKHR *surfaceFormats = static_cast<VkSurfaceFormatKHR*>(malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount));
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

  uint32_t surfaceFormatIndex = 0;
  for (int i = 0; i < surfaceFormatCount; ++i){
    if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
      surfaceFormatIndex = i;
      break;
    }
  }

  std::cout <<surfaceFormatIndex;

  // Query and Choose Presentation modes (Fifo, mailBox)
  //
  uint32_t presentationModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, nullptr);
  VkPresentModeKHR *presentationModes = static_cast<VkPresentModeKHR*>(malloc(sizeof(VkPresentModeKHR) * presentationModeCount));
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentationModeCount, presentationModes);

  uint32_t PresentIndex = 0;
  for (int i = 0; i < presentationModeCount; ++i){
    if (presentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){
      PresentIndex = i;
      break;
    }
  }

  std::cout <<PresentIndex;


  // choose swap extent TODO


  free(surfaceFormats);
  free(presentationModes);
  return RET_CODE::RETURN_SUCCES; 
}

RET_CODE init_Vk(){

  if (allocate_instance_vk() != RET_CODE::RETURN_SUCCES){
    return RET_CODE::RETURN_FAILED;
  }

  if (allocate_surface_vk() != RET_CODE::RETURN_SUCCES){
    return RET_CODE::RETURN_FAILED;
  }

  if (setup_physical_device_vk() != RET_CODE::RETURN_SUCCES){
    return RET_CODE::RETURN_FAILED;
  }
  if (setup_logic_device_vk() != RET_CODE::RETURN_SUCCES){
    return RET_CODE::RETURN_FAILED;
  }

  setup_swapchain_vk();

  return RET_CODE:: RETURN_SUCCES;
}

void entrypoint_mainloop_Vk(){

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

}

void cleanup_Vk(){

  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();

}



int main(int argc, char** argv){

  init_glfw_window();
  
  std::cout << "starting vulkan" << std::endl;

  if (init_Vk() == RET_CODE:: RETURN_FAILED)
  {
    std::cout << "failed to init vulkan\n" << std::endl;
  }
  std::cout << "vulkan setup is done" << std::endl; 

  entrypoint_mainloop_Vk();
  cleanup_Vk();


  return 0;
}
