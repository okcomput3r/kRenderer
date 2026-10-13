#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory/memory.h>
#include <utils/logs.hpp>

#include <cstdint>

struct renderer_info{

  VkInstance   instance {nullptr};

  // surface
  GLFWWIndow*  window   {nullptr};
  VkSurfaceKHR surface  {nullptr}:

  VkPhysicalDevice physicalDevice {nullptr};

};
typedef renderer_info renderer_info;


struct viewPort {
  uint32_t width;
  uint32_t height;

};
