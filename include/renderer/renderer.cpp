#include <renderer/renderer.hpp>





/**
 * kRendererCore_vk es el nucleo del renderizador, en esta estructura de
 * datos se almacenan los elementos necesarios para construir una pipeline
 * de renderizado.
 *
 *
 *
 * TODO
 * - hacer wrappers para elementos parecidos (ej. window, surface, surfaceExtent...)
 *
 */
struct s_kRendererCore_vk{

  VkInstance   instance {nullptr};

  // window management ( surface, surface extent, surface format )
  GLFWwindow*  window   {nullptr};
  VkSurfaceKHR surface  {nullptr};

  VkExtent2D surfaceExtent;
  uint32_t surfaceFormatIndex {UINT32_MAX};
  std::vector<VkSurfaceFormatKHR> surfaceFormats {};

  // physical & logicalDevices

  std::vector<VkPhysicalDevice> physicalDevices {};
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

  std::vector<VkImage>     swapChainImages {};
  std::vector<VkImageView> swapChainImageViews {};

  // shader Modules
  VkShaderModule shaderModule;

  // TODO in the future, have multiple pipelines (if posible)
  VkPipelineLayout pipelineLayout {nullptr};
  VkPipeline graphicsPipeline     {nullptr};

  VkCommandPool   commandPool {nullptr};
  VkCommandBuffer commandBuffer {nullptr};

  // TODO wrap semaphores into its own memory/concurrency header if needed
  VkSemaphore presentCompleteSemaphore {nullptr}, renderFinishedSemaphore {nullptr};
  VkFence drawFence {nullptr};

};

// definition of functions
void instanciate_renderer_core_vk(kRendererCore_vk* renderer);
kRendererCore_vk* get_renderer_core_instance_vk();



static kRendererCore_vk* s_renderer = nullptr;


void instanciate_renderer_core_vk(kRendererCore_vk *renderer)
{

}

kRendererCore_vk* get_renderer_core_instance_vk(){

  if (s_renderer == nullptr)
    instanciate_renderer_core_vk(s_renderer);
  return s_renderer;
}





void init_module_renderer(){

  kRendererCore_vk* renderer = get_renderer_core_instance_vk();

}


void get_GLFW_window(kRendererCore_vk* renderer){

}

void allocate_instance_vk(kRendererCore_vk* renderer){}

void setup_pyshical_device_vk(kRendererCore_vk* renderer){}

uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices){ return -1;}

void setup_logic_device_vk(kRendererCore_vk* renderer) {}

void setup_swapchain_vk(kRendererCore_vk* renderer) {}

void setup_graphic_pipeline_vk(kRendererCore_vk* renderer) {}

void start_log_entry(char* path);




