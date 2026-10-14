#include <renderer/renderer.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// Static singleton renderer
static kRendererCore_vk* s_renderer = nullptr;


/**
 * s_MachineContext_vk
 * 
 *
 */
struct s_MachineContext_vk{

  VkPhysicalDevice*   p_physicalDevices {nullptr}; // array of physical devices availables
  VkSurfaceFormatKHR* p_surfaceFormats  {nullptr}; // array of surface formats availables
  VkQueueFamilyProperties*    p_queueFamilies {nullptr}; // array of queue families availables

};
typedef s_MachineContext_vk MachineContext_vk;



/**
 * s_ViewPort_vk
 *
 *
 *
 */
struct s_ViewPort_vk {

  uint32_t width  {s_defaultWindowSize_x};
  uint32_t height {s_defaultWindowSize_y};


  VkSurfaceFormatKHR* p_surfaceFormat {nullptr};
  VkSurfaceKHR        p_surface       {nullptr};
  VkExtent2D surfaceExtent;

};
typedef s_ViewPort_vk ViewPort_vk;


/***
 *  s_Swapchain_vk
 *
 *
 *
 */
struct s_Swapchain_vk {

  VkImage*     p_swapChainImages     {nullptr};
  VkImageView* p_swapChainImageViews {nullptr};

  VkSwapchainKHR swapchain {nullptr};

  uint32_t imageCount {UINT32_INVALID_VALUE};
};
typedef s_Swapchain_vk Swapchain_vk;


struct s_Pipeline_vk {

  VkPipelineLayout pipelineLayout {nullptr};
  VkPipeline graphicsPipeline     {nullptr};

};
typedef s_Pipeline_vk Pipeline_vk;


/**
 *  s_RendererContext_vk
 *
 *
 *
 */
struct s_RendererContext_vk{

  VkApplicationInfo appInfo {};

  GLFWwindow* window  {nullptr};
  ViewPort_vk viewPort;

  VkInstanceCreateInfo instanceCreateInfo {};
  VkInstance instance {nullptr};

  VkPhysicalDevice* p_physicalDevice {nullptr};
  VkDevice logicalDevice {};

  uint32_t graphicsFamilyIndex {UINT32_INVALID_VALUE};
  VkQueue graphicQueue;

};
typedef s_RendererContext_vk RendererContext_vk;



/**
 *  s_concurrence_vk
 *
 *
 *
 * TODO
 *  - move to its own header file
 */
struct s_concurrence_vk{

  VkCommandPool   commandPool {nullptr};
  VkCommandBuffer commandBuffer {nullptr};

  // TODO wrap semaphores into its own memory/concurrency header if needed
  VkSemaphore presentCompleteSemaphore {nullptr}, renderFinishedSemaphore {nullptr};
  VkFence drawFence {nullptr};

};
typedef s_concurrence_vk Concurrence_vk;



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

  RendererContext_vk context;
 
  Swapchain_vk swapchain;

  Pipeline_vk pipeline;

  // TODO put shader Modules on other struct (ej.pipeline or new)
  VkShaderModule shaderModule;

  RendererDescriptor descriptor;
};

// Definition of private functions

void instanciate_renderer_core_vk(kRendererCore_vk* renderer);
kRendererCore_vk* get_renderer_core_instance_vk();

void get_GLFW_window(kRendererCore_vk* renderer);
void allocate_instance_vk(kRendererCore_vk* renderer);
void setup_pyshical_device_vk(kRendererCore_vk* renderer);
uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices);
void setup_logic_device_vk(kRendererCore_vk* renderer);
void setup_swapchain_vk(kRendererCore_vk* renderer);
void setup_graphic_pipeline_vk(kRendererCore_vk* renderer);
void setup_default_graphic_pipeline_vk(kRendererCore_vk* renderer); // without configutation

void start_log_entry(char* path);




// implementation


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

void allocate_instance_vk(kRendererCore_vk* renderer){

}

void setup_pyshical_device_vk(kRendererCore_vk* renderer){

}

uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices){ return -1;}

void setup_logic_device_vk(kRendererCore_vk* renderer) {

}

void setup_swapchain_vk(kRendererCore_vk* renderer) {

}

void setup_graphic_pipeline_vk(kRendererCore_vk* renderer) {

}

void start_log_entry(char* path){

}




