#include <renderer/renderer.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <cstdio>

#define assert(COND,MSG) (void*)(0)
#define warning(COND,MSG) (void*)(0)
#define panic(COND,MSG) \
  do {  \
    if (COND) { \
      std::cout<<MSG<<std::endl;\
      exit(0);\
    }\
  } while (0)

#define trace(MSG) std::cout<<MSG<<std::endl;

// ARENA MOVE OTHER PLACE
static constexpr size_t s_maxArenaSize {UINT64_MAX};

struct s_StackArena{

  void* p_basePointer {nullptr};
  size_t offset   {0};
  size_t capacity {0};

  s_StackArena () = delete;
  s_StackArena (size_t _capacity){
    capacity = _capacity;
    p_basePointer = malloc(capacity);

    panic(p_basePointer == nullptr, "couldn't allocate memory");

  }
  ~s_StackArena() {
    destroy();
  }

  s_StackArena (s_StackArena& other)  = delete;
  s_StackArena (s_StackArena&& other) = delete;
  s_StackArena operator=(s_StackArena& other) = delete;

  void* push(size_t bytes){
    void* last_top = (void*)((size_t) p_basePointer + offset);

    panic(offset + bytes > capacity, "couldn't fit new item, arena full.");

    offset += bytes;

    return last_top;
  }

  void* clear(){

    warning(offset == 0, "nothing to clear");
    offset = 0;
    return p_basePointer;
  }

  void destroy(){
    if (p_basePointer!= nullptr)
      free(p_basePointer);
    p_basePointer = nullptr;
  }

};
typedef s_StackArena StackArena;





































// Static singleton renderer
static kRendererCore_vk* s_renderer = nullptr;
static StackArena s_rendererArena {100000}; // 100.000 bytes

/**
 * s_MachineContext_vk
 * 
 *
 */
struct s_MachineContext_vk{

  VkPhysicalDevice*           p_physicalDevices; // array of physical devices availables
  VkSurfaceFormatKHR*         p_surfaceFormats; // array of surface formats availables
  VkQueueFamilyProperties*    p_queueFamilies; // array of queue families availables

  VkApplicationInfo    appInfo;
  VkInstanceCreateInfo instanceCreateInfo;

};
typedef s_MachineContext_vk MachineContext_vk;


/**
 * s_ViewPort_vk
 *
 *
 *
 */
struct s_ViewPort_vk {

  uint32_t width;
  uint32_t height;


  VkSurfaceFormatKHR* p_surfaceFormat;
  VkSurfaceKHR        surface;
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

  VkImage*     p_swapChainImages;
  VkImageView* p_swapChainImageViews;

  VkSwapchainKHR swapchain;

  uint32_t imageCount;
};
typedef s_Swapchain_vk Swapchain_vk;


struct s_Pipeline_vk {

  VkPipelineLayout pipelineLayout;
  VkPipeline graphicsPipeline;

};
typedef s_Pipeline_vk Pipeline_vk;


/**
 *  s_RendererContext_vk
 *
 *
 *
 */
struct s_RendererContext_vk{

  VkInstance instance;

  GLFWwindow* window;
  ViewPort_vk viewPort;

  VkPhysicalDevice* p_physicalDevice;
  VkDevice logicalDevice;

  uint32_t graphicsFamilyIndex;
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

  VkCommandPool   commandPool;
  VkCommandBuffer commandBuffer;

  // TODO wrap semaphores into its own memory/concurrency header if needed
  VkSemaphore presentCompleteSemaphore, renderFinishedSemaphore;
  VkFence drawFence;

};
typedef s_concurrence_vk Concurrence_vk;



/**
 * kRendererCore_vk es el nucleo del renderizador, en esta estructura de
 * datos se almacenan los elementos necesarios para construir una pipeline
 * de renderizado.
 *
 *
 *
 *
 */
struct s_kRendererCore_vk{

  RendererContext_vk* p_context;
  MachineContext_vk*  p_machineContext;
  Swapchain_vk*       p_swapchain;
  Pipeline_vk*        p_pipeline;

  RendererDescriptor* p_descriptor;


  // TODO put shader Modules on other struct (ej.pipeline or new)
  VkShaderModule shaderModule;

};

// Definition of private functions

void instanciate_renderer_core_vk();
kRendererCore_vk* get_renderer_core_instance_vk();

void init_vk(kRendererCore_vk* renderer);

void get_GLFW_window(kRendererCore_vk* renderer);
void allocate_instance_vk(kRendererCore_vk* renderer);
void allocate_surface_vk(kRendererCore_vk* renderer);
void setup_physical_device_vk(kRendererCore_vk* renderer);
uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices, uint32_t deviceCount);
void setup_logic_device_vk(kRendererCore_vk* renderer);
void setup_swapchain_vk(kRendererCore_vk* renderer);
void setup_graphic_pipeline_vk(kRendererCore_vk* renderer);
void setup_default_graphic_pipeline_vk(kRendererCore_vk* renderer); // without configutation

void start_log_entry(char* path);




// implementation


void instanciate_renderer_core_vk()
{
    s_renderer =  static_cast<kRendererCore_vk*>(malloc(sizeof(kRendererCore_vk)));// static_cast<kRendererCore_vk*>(s_rendererArena.push(sizeof(kRendererCore_vk)));
    panic(s_renderer == nullptr, "WTH");

    // instanciar todos los punteros necesarios
    // TODO
 

    // Renderer Context allocation

    s_renderer->p_context = static_cast<RendererContext_vk*>(malloc(sizeof(RendererContext_vk)));

    s_renderer->p_context->window   = nullptr;
    s_renderer->p_context->instance = nullptr;
    s_renderer->p_context->p_physicalDevice = nullptr;
    s_renderer->p_context->logicalDevice    = nullptr;
    s_renderer->p_context->viewPort.width   = s_defaultWindowSize_x;
    s_renderer->p_context->viewPort.height  = s_defaultWindowSize_y;
    s_renderer->p_context->viewPort.surface         = nullptr;
    s_renderer->p_context->viewPort.p_surfaceFormat = nullptr;
    s_renderer->p_context->graphicsFamilyIndex      = UINT32_INVALID_VALUE;
    s_renderer->p_context->graphicQueue = nullptr;

    // Machine Context allocation

    s_renderer->p_machineContext = static_cast<MachineContext_vk*>(malloc(sizeof(MachineContext_vk)));

    //s_renderer->p_machineContext->appInfo;
    //s_renderer->p_machineContext->instanceCreateInfo;
    s_renderer->p_machineContext->p_physicalDevices  = nullptr;
    s_renderer->p_machineContext->p_surfaceFormats   = nullptr;
    s_renderer->p_machineContext->p_queueFamilies    = nullptr;

    // Swapchain allocation

    s_renderer->p_swapchain = static_cast<Swapchain_vk*>(malloc(sizeof(Swapchain_vk)));

    s_renderer->p_swapchain->p_swapChainImages     = nullptr;
    s_renderer->p_swapchain->p_swapChainImageViews = nullptr;
    s_renderer->p_swapchain->swapchain  = nullptr;
    s_renderer->p_swapchain->imageCount = UINT32_INVALID_VALUE;

    // Pipeline allocation

    s_renderer->p_pipeline = static_cast<Pipeline_vk*>(malloc(sizeof(Pipeline_vk)));

    s_renderer->p_pipeline->pipelineLayout   = nullptr;
    s_renderer->p_pipeline->graphicsPipeline = nullptr;

    // Renderer Descriptor allocation

    s_renderer->p_descriptor = static_cast<RendererDescriptor*>(malloc(sizeof(RendererDescriptor)));
    // TODO
    // initilice descriptor params

}

kRendererCore_vk* get_renderer_core_instance_vk(){

  if (s_renderer == nullptr){
    instanciate_renderer_core_vk();
  }
  return s_renderer;
}

void init_module_renderer(){

  //Logs::start_log_session("Renderer");
  trace("Initiating kRenderer module. ");

  kRendererCore_vk* renderer = get_renderer_core_instance_vk();
  panic(renderer == nullptr, "couln't instantiate renderer");

  trace("Getting GLFW window.");
  get_GLFW_window(s_renderer);

  trace("GLFW window obtained.");
  trace("Initialicing vulkan");
  init_vk(s_renderer);

}

void init_vk(kRendererCore_vk* renderer){

  trace("Allocating vulkan instance.");
  allocate_instance_vk(renderer);

  trace("Vulkan instance successfully allocated.");
  trace("Allocating rendering surface.");
  allocate_surface_vk(renderer);

  trace("Rendering surface successfully allocated.");
  trace("Setting up physical device.");
  setup_physical_device_vk(renderer);

  trace("physical device successfully configured.");
  trace("Setting up logic device.");
  setup_logic_device_vk(renderer);

  trace("Logic device setup complete.");
  trace("Settign up pipeline wapchain.");
  setup_swapchain_vk(renderer);

  trace("Pipeline setup completed.");
  trace("setting up graphics pipeline.");
  setup_graphic_pipeline_vk(renderer);
  trace("Pipeline setup completed.");

}


void get_GLFW_window(kRendererCore_vk* renderer){

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  renderer->p_context->window = glfwCreateWindow(renderer->p_context->viewPort.width,
                                              renderer->p_context->viewPort.height,
                                              "kEngine",
                                              nullptr,
                                              nullptr);

  panic(renderer->p_context->window == nullptr, "Couldn't get GLFW Window!");

}

void allocate_instance_vk(kRendererCore_vk* renderer){

  // App Info
  VkApplicationInfo& appInfo_ref = renderer->p_machineContext->appInfo;
  appInfo_ref.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo_ref.pApplicationName = "kEngine";
  appInfo_ref.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo_ref.pEngineName = "Kilby Engine";
  appInfo_ref.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo_ref.apiVersion = VK_API_VERSION_1_0;

  // get required extensions
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  // get vulkan supported extensions
  uint32_t vulkanExtensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, nullptr);
  VkExtensionProperties *vulkanExtensions = static_cast<VkExtensionProperties *>( malloc(sizeof(VkExtensionProperties) * vulkanExtensionCount));
  vkEnumerateInstanceExtensionProperties(nullptr, &vulkanExtensionCount, vulkanExtensions);

  // Checking for extension(s) support
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
      panic(!supported, "Vulkan Extensions not supported");
    }
  }
  free(vulkanExtensions);

  // Create Instance Info
  VkInstanceCreateInfo& createInfo_ref   = renderer->p_machineContext->instanceCreateInfo;
  createInfo_ref.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo_ref.pApplicationInfo        = &appInfo_ref;
  createInfo_ref.enabledExtensionCount   = glfwExtensionCount;
  createInfo_ref.ppEnabledExtensionNames = glfwExtensions;
  createInfo_ref.enabledLayerCount       = 0;
  createInfo_ref.ppEnabledLayerNames     = nullptr;

  // Instance

  if (vkCreateInstance(&createInfo_ref, nullptr, &renderer->p_context->instance) != VK_SUCCESS)
    panic(1==1,"Unable to create vulkan instance");

}

void allocate_surface_vk(kRendererCore_vk* renderer){

  trace("allocating surface for rendering");

  if (glfwCreateWindowSurface(renderer->p_context->instance, renderer->p_context->window, nullptr, &renderer->p_context->viewPort.surface) != 0)
    panic(1==1, "unable to allocate surface for rendering.");

}


void setup_physical_device_vk(kRendererCore_vk* renderer){

// Physical Device

  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(renderer->p_context->instance, &deviceCount, nullptr);

  panic(deviceCount == 0, "No Physical devices found!");

  renderer->p_machineContext->p_physicalDevices = static_cast<VkPhysicalDevice *>(
      malloc(sizeof(VkPhysicalDevice) * deviceCount));

  vkEnumeratePhysicalDevices(renderer->p_context->instance,
                             &deviceCount,
                             renderer->p_machineContext->p_physicalDevices);


  uint32_t device_index = choose_physical_device_vk(renderer->p_machineContext->p_physicalDevices, deviceCount);

  renderer->p_context->p_physicalDevice = &renderer->p_machineContext->p_physicalDevices[device_index];
}

uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices, uint32_t deviceCount){

  // Checking for support on extensions
  //    - for each device, get supported extensions for that device
  //    - then compare them to the requierd extension
  //    - if all extensions are supported, then select that device

  uint8_t ext_supported = 0;
  uint32_t device_index = 0;
  for (int i = 0; i < deviceCount; ++i) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevices[i], nullptr, &extensionCount, nullptr);
    VkExtensionProperties *aviableExtensions = static_cast<VkExtensionProperties *>(malloc(sizeof(VkExtensionProperties) * extensionCount));
    vkEnumerateDeviceExtensionProperties(physicalDevices[i], nullptr, &extensionCount, aviableExtensions);

    for (int j = 0; j < s_requiredDeviceExtensionCount; ++j) {
      ext_supported = 0;
      for (int w = 0; w < extensionCount; ++w) {
        if (strcmp(s_requiredDeviceExtensions[j],
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
    if (ext_supported)
      device_index = i;
      break;

  }

  panic(!ext_supported, "No support for extensions with any physicalDevice!");

  return device_index;
}

void setup_logic_device_vk(kRendererCore_vk* renderer) {

}

void setup_swapchain_vk(kRendererCore_vk* renderer) {

}

void setup_graphic_pipeline_vk(kRendererCore_vk* renderer) {

}


