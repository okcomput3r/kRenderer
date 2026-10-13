#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory/memory.h>
#include <utils/logs.hpp>

#include <cstdint>

struct renderer_info{

  VkInstance   instance {nullptr};

  // window management ( surface, surface extent, surface format )
  GLFWwindow*  window   {nullptr};
  VkSurfaceKHR surface  {nullptr};

  VkExtent2D surfaceExtent;

  uint32_t surfaceFormatIndex {UINT32_MAX};
  Memory::Containers::vector<VkSurfaceFormatKHR> surfaceFormats {};

  // physical & logicalDevices

  Memory::Containers::vector<VkPhysicalDevice> physicalDevices {};
  VkPhysicalDevice* p_selectedPhysicalDevice;
  VkDevice          logicalDevice  {nullptr};

  // TODO Wrap into struct like "renderer_graphic_queue"
  uint32_t graphicsFamilyIndex {UINT32_MAX};
  VkQueue graphicQueue;

  VkApplicationInfo appInfo {};
  VkInstanceCreateInfo instanceCreateInfo {};

  VkSwapchainKHR swapchain {nullptr};

  // vk Images
  uint32_t imageCount {UINT32_MAX};
  uint32_t desiredImageCount {};

  Memory::Containers::vector<VkImage>     swapChainImages { };
  Memory::Containers::vector<VkImageView> swapChainImageViews { };

  renderer_info(const renderer_info&)  = delete;
  renderer_info(const renderer_info&&) = delete;
  renderer_info& operator=(const renderer_info&) = delete;

  static renderer_info& getInstance(){
    static renderer_info obj;
    return obj;
  }



  private:

    renderer_info() {}

};
typedef renderer_info renderer_info;


struct viewPort {
  uint32_t width;
  uint32_t height;

};
