#include <renderer/core.hpp>
#include <sys/types.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>

//Remove later
#include <vector>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <fstream>


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
// Logs dont print to screen, unlike warning and error, wich do both screen and file
// Logs::trace()   // activate under flag DEBUG
// Logs::Verbose() // activate under flag VERBOSE
//
// Assert::assert() // activate under flag DEBUG
// Assert::assert_paranoid() // activate under flag PARANOID
//
// Error::panic()
// Error::abort() // panic without condition statement, only msg



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





































/**
 * s_MachineContext_vk
 * 
 *
 */
struct s_MachineContext_vk{

  VkPhysicalDevice*           p_physicalDevices; // array of physical devices availables
  VkSurfaceFormatKHR*         p_surfaceFormats; // array of surface formats availables
  VkQueueFamilyProperties*    p_queueFamilies; // array of queue families availables
  VkSurfaceCapabilitiesKHR    surfaceCapabilities;

  VkApplicationInfo        appInfo;
  VkInstanceCreateInfo     instanceCreateInfo;
  VkDeviceCreateInfo       logicDeviceCreateInfo;
  VkDeviceQueueCreateInfo  queueCreateInfo;
  VkSwapchainCreateInfoKHR swapchainCreateInfo;
  VkImageViewCreateInfo    imageViewCreateInfo;

  // Features
  VkPhysicalDeviceFeatures2        features2;
  VkPhysicalDeviceVulkan11Features features11;
  VkPhysicalDeviceVulkan13Features features13;


  uint32_t physicalDevicesCount;
  uint32_t queueFamiliesCount;

  uint32_t surfaceFormatCount;
  uint32_t surfaceFormatIndex;

};
typedef s_MachineContext_vk MachineContext_vk;


/**
 *
 * s_RendererDescriptor
 *
 *
 */

struct s_RedererDescriptor{

};


/**
 * s_ViewPort_vk
 *
 *
 *
 */
struct s_ViewPort_vk {

  VkSurfaceFormatKHR* p_surfaceFormat;
  VkSurfaceKHR        surface;
  VkExtent2D          surfaceExtent;
  VkViewport          instance;

  uint32_t width;
  uint32_t height;

};
typedef s_ViewPort_vk ViewPort_vk;


/***
 *  s_Swapchain_vk
 *
 *
 *
 */
struct s_Swapchain_vk {

  VkSwapchainKHR swapchain;
  VkImage*       p_swapChainImages;
  VkImageView*   p_swapChainImageViews;
  uint32_t       imageCount;

};
typedef s_Swapchain_vk Swapchain_vk;


struct s_Pipeline_vk {

  VkPipelineLayout pipelineLayout;
  VkPipeline       graphicsPipeline;

  // TODO put shader Modules on other struct
  VkShaderModule   shaderModule;
  //VkPipelineShaderStageCreateInfo shaderModuleStages[]; descartar por ahora
  //VkPipelineShaderStageCreateInfo vertexShaderStageInfo; descartar por ahora
  //VkPipelineShaderStageCreateInfo fragmentShaderStageInfo; descartar por ahora

  // pipeline stages
  VkPipelineVertexInputStateCreateInfo   vertexInputState;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyState; // TODO descartar al pasar a mesh rendering
  VkPipelineDynamicStateCreateInfo       dynamicState;
  VkPipelineViewportStateCreateInfo      viewPortState;
  VkPipelineRasterizationStateCreateInfo rasterizerState;
  VkPipelineMultisampleStateCreateInfo   multisamplingState;
  VkPipelineColorBlendAttachmentState    colorBlendingAttachmentState;
  VkPipelineColorBlendStateCreateInfo    colorBlendindState;

  VkPipelineLayoutCreateInfo             pipelineLayoutInfo;
  VkPipelineRenderingCreateInfo          pipelineRenderingCreateInfo;
  VkGraphicsPipelineCreateInfo           graphicsPipelineCreateInfo;

};
typedef s_Pipeline_vk Pipeline_vk;


/**
 *  s_RendererContext_vk
 *
 *
 *
 */
struct s_RendererContext_vk{

  VkInstance       instance;
  VkPhysicalDevice physicalDevice;
  VkDevice         logicDevice;
  GLFWwindow*      window;
  ViewPort_vk      viewPort;
  uint32_t         graphicsFamilyIndex;
  VkQueue          graphicQueue;

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

  Concurrence_vk concurrence;


};
typedef s_kRendererCore_vk kRendererCore_vk;


// Static singleton renderer
static kRendererCore_vk* s_renderer = nullptr;
static StackArena s_rendererArena {100000}; // 100.000 bytes

// Definition of private functions

void instanciate_renderer_core_vk();
kRendererCore_vk* get_renderer_core_instance_vk();
void init_vk(kRendererCore_vk* renderer);
void allocate_instance_vk(kRendererCore_vk* renderer);
void allocate_surface_vk(kRendererCore_vk* renderer);
void setup_physical_device_vk(kRendererCore_vk* renderer);
uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices, uint32_t deviceCount);
void setup_logic_device_vk(kRendererCore_vk* renderer);
void setup_swapchain_vk(kRendererCore_vk* renderer);
void setup_graphic_pipeline_vk(kRendererCore_vk* renderer);
void setup_default_graphic_pipeline_vk(kRendererCore_vk* renderer); // without configutation
void setup_command_pool_vk(kRendererCore_vk* renderer);
void record_command_buffer(uint32_t imageIndex);
void create_sync_objects(kRendererCore_vk* renderer);
GLFWwindow* get_GLFW_window();

// implementation




void init_module_renderer(GLFWwindow* window){

  //Logs::start_log_session("Renderer");
  trace("Initiating kRenderer module. ");

  panic(window == nullptr, "pointer to glfw window is null");

  kRendererCore_vk* renderer = get_renderer_core_instance_vk();
  panic(renderer == nullptr, "couln't instantiate renderer!");
 
  renderer->p_context->window = window;

  trace("Initialicing vulkan");
  init_vk(renderer);

}

void init_module_renderer(){
  //Logs::start_log_session("Renderer");
  trace("Initiating kRenderer module. ");

  kRendererCore_vk* renderer = get_renderer_core_instance_vk();
  panic(renderer == nullptr, "couln't instantiate renderer!");
 
  renderer->p_context->window = get_GLFW_window();

  trace("Initialicing vulkan");
  init_vk(renderer);

}

GLFWwindow* retrieve_renderer_window(){

  return get_renderer_core_instance_vk()->p_context->window;
}

kRendererCore_vk* get_renderer_core_instance_vk(){

  if (s_renderer == nullptr)
    instanciate_renderer_core_vk();

  return s_renderer;
}

void instanciate_renderer_core_vk() {
    s_renderer =  static_cast<kRendererCore_vk*>(malloc(sizeof(kRendererCore_vk)));// static_cast<kRendererCore_vk*>(s_rendererArena.push(sizeof(kRendererCore_vk)));
    panic(s_renderer == nullptr, "WTH");

    // Renderer Context allocation

    s_renderer->p_context = static_cast<RendererContext_vk*>(malloc(sizeof(RendererContext_vk)));

    s_renderer->p_context->window                   = nullptr;
    s_renderer->p_context->instance                 = nullptr;
    s_renderer->p_context->physicalDevice           = nullptr;
    s_renderer->p_context->logicDevice              = nullptr;
    s_renderer->p_context->viewPort.width           = s_defaultWindowSize_x;
    s_renderer->p_context->viewPort.height          = s_defaultWindowSize_y;
    s_renderer->p_context->viewPort.surface         = nullptr;
    s_renderer->p_context->viewPort.p_surfaceFormat = nullptr;
    s_renderer->p_context->graphicsFamilyIndex      = UINT32_INVALID_VALUE;
    s_renderer->p_context->graphicQueue             = nullptr;

    // Machine Context allocation

    s_renderer->p_machineContext = static_cast<MachineContext_vk*>(malloc(sizeof(MachineContext_vk)));

    s_renderer->p_machineContext->p_physicalDevices  = nullptr;
    s_renderer->p_machineContext->p_surfaceFormats   = nullptr;
    s_renderer->p_machineContext->p_queueFamilies    = nullptr;

    s_renderer->p_machineContext->physicalDevicesCount = UINT32_INVALID_VALUE;
    s_renderer->p_machineContext->queueFamiliesCount   = UINT32_INVALID_VALUE;
    s_renderer->p_machineContext->surfaceFormatCount   = UINT32_INVALID_VALUE;
    s_renderer->p_machineContext->surfaceFormatIndex   = UINT32_INVALID_VALUE;

  // Swapchain allocation

    s_renderer->p_swapchain = static_cast<Swapchain_vk*>(malloc(sizeof(Swapchain_vk)));

    s_renderer->p_swapchain->p_swapChainImages     = nullptr;
    s_renderer->p_swapchain->p_swapChainImageViews = nullptr;
    s_renderer->p_swapchain->swapchain             = nullptr;
    s_renderer->p_swapchain->imageCount            = UINT32_INVALID_VALUE;

    // Pipeline allocation

    s_renderer->p_pipeline = static_cast<Pipeline_vk*>(malloc(sizeof(Pipeline_vk)));

    s_renderer->p_pipeline->pipelineLayout   = nullptr;
    s_renderer->p_pipeline->graphicsPipeline = nullptr;

    // Renderer Descriptor allocation

    s_renderer->p_descriptor = static_cast<RendererDescriptor*>(malloc(sizeof(RendererDescriptor)));
    // TODO
    // initilice descriptor params

}

void init_vk(kRendererCore_vk* renderer){

  trace("Allocating vulkan instance.");
  allocate_instance_vk(renderer);

  trace("Allocating rendering surface.");
  allocate_surface_vk(renderer);

  trace("Setting up physical device.");
  setup_physical_device_vk(renderer);

  trace("Setting up logic device.");
  setup_logic_device_vk(renderer);

  trace("Settign up pipeline wapchain.");
  setup_swapchain_vk(renderer);

  trace("setting up graphics pipeline.");
  setup_graphic_pipeline_vk(renderer);

  trace("Setting up command pool");
  setup_command_pool_vk(renderer);

  trace("Creating sync objects");
  create_sync_objects(renderer);

  trace("VULKAN SETUP DONE");
}

GLFWwindow* get_GLFW_window(){

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


  GLFWwindow* window = nullptr;
  window = glfwCreateWindow(s_defaultWindowSize_x,s_defaultWindowSize_y,
                            "kEngine",
                            nullptr,
                            nullptr);
  
  panic(window == nullptr, "null window");
  return window;
}
void allocate_instance_vk(kRendererCore_vk* renderer){

  // App Info
  VkApplicationInfo& appInfo_ref  = renderer->p_machineContext->appInfo;
  appInfo_ref.sType               = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo_ref.pApplicationName    = "kEngine";
  appInfo_ref.applicationVersion  = VK_MAKE_VERSION(1, 0, 0);
  appInfo_ref.pEngineName         = "Kilby Engine";
  appInfo_ref.engineVersion       = VK_MAKE_VERSION(1, 0, 0);
  appInfo_ref.apiVersion          = VK_API_VERSION_1_4;

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


  // Check for validation layers

  if (enableValidationLayers){

    uint32_t vk_validationLayerCount = UINT32_INVALID_VALUE;
    vkEnumerateInstanceLayerProperties(&vk_validationLayerCount, nullptr);
    VkLayerProperties* p_layerProperties = static_cast<VkLayerProperties*>(malloc(sizeof(VkLayerProperties)*vk_validationLayerCount));
    vkEnumerateInstanceLayerProperties(&vk_validationLayerCount, p_layerProperties);

    // Checking for layer(s) support

    for (uint32_t i = 0; i < s_validationLayersCount; ++i) {
      supported = 0;
      for (uint32_t j = 0; j < vk_validationLayerCount; ++j) {
        if (strcmp(s_validationLayers[i], p_layerProperties[j].layerName) == 0) {
          supported = 1;
          break;
        }
      }
      if (!supported) {
        free(p_layerProperties);
        panic(!supported, "Vulkan Extensions not supported");
      }
    }
 
  }


  // Create Instance Info
  VkInstanceCreateInfo& createInfo_ref   = renderer->p_machineContext->instanceCreateInfo;
  createInfo_ref.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo_ref.pApplicationInfo        = &appInfo_ref;
  createInfo_ref.enabledExtensionCount   = glfwExtensionCount;
  createInfo_ref.ppEnabledExtensionNames = glfwExtensions;
  createInfo_ref.enabledLayerCount       = s_validationLayersCount;
  createInfo_ref.ppEnabledLayerNames     = s_validationLayers;

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

  vkEnumeratePhysicalDevices(renderer->p_context->instance, &renderer->p_machineContext->physicalDevicesCount, nullptr);

  panic(renderer->p_machineContext->physicalDevicesCount == 0, "No Physical devices found!");

  renderer->p_machineContext->p_physicalDevices = static_cast<VkPhysicalDevice *>(
      malloc(sizeof(VkPhysicalDevice) * renderer->p_machineContext->physicalDevicesCount));

  vkEnumeratePhysicalDevices(renderer->p_context->instance,
                             &renderer->p_machineContext->physicalDevicesCount,
                             renderer->p_machineContext->p_physicalDevices);


  uint32_t device_index = choose_physical_device_vk(renderer->p_machineContext->p_physicalDevices, renderer->p_machineContext->physicalDevicesCount);

  renderer->p_context->physicalDevice = renderer->p_machineContext->p_physicalDevices[device_index];
}

// Checking for support on extensions
//    - for each device, get supported extensions for that device
//    - then compare them to the requierd extension
//    - if all extensions are supported, then select that device

uint32_t choose_physical_device_vk(VkPhysicalDevice* physicalDevices, uint32_t deviceCount){

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
    if (ext_supported){
      device_index = i;
      break;
    }
  }

  panic(!ext_supported, "No support for extensions with any physicalDevice!");

  return device_index;
}

void setup_logic_device_vk(kRendererCore_vk* renderer) {

  // Get Family Queues

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(renderer->p_context->physicalDevice,
                                           &queueFamilyCount,
                                           nullptr);
  renderer->p_machineContext->p_queueFamilies = static_cast<VkQueueFamilyProperties *>(malloc(sizeof(VkQueueFamilyProperties) * queueFamilyCount));
  vkGetPhysicalDeviceQueueFamilyProperties(renderer->p_context->physicalDevice,
                                           &queueFamilyCount,
                                           renderer->p_machineContext->p_queueFamilies);

  VkBool32 result;
  for (int i = 0; i < queueFamilyCount; ++i) {
    vkGetPhysicalDeviceSurfaceSupportKHR(renderer->p_context->physicalDevice, i, renderer->p_context->viewPort.surface, &result);

    VkQueueFamilyProperties& r_queueFamilie = renderer->p_machineContext->p_queueFamilies[i];
    if (r_queueFamilie.queueFlags & VK_QUEUE_GRAPHICS_BIT && result == VK_TRUE) {
      renderer->p_context->graphicsFamilyIndex = i;
      break;
    }
  }

  panic(renderer->p_context->graphicsFamilyIndex == -1, "No graphic family Found!");

  // Configuration of Logic Device

  float queuePriority = 0.5f;

  VkDeviceQueueCreateInfo& queueCreateInfo_ref = renderer->p_machineContext->queueCreateInfo;
  queueCreateInfo_ref.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo_ref.queueFamilyIndex = renderer->p_context->graphicsFamilyIndex;
  queueCreateInfo_ref.queueCount       = 1;
  queueCreateInfo_ref.pQueuePriorities = &queuePriority;

  // Setting up features

  VkPhysicalDeviceFeatures2& features2_ref = renderer->p_machineContext->features2;
  features2_ref.sType                      = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  features2_ref.features.geometryShader    = VK_TRUE;

  VkPhysicalDeviceVulkan11Features& features11_ref = renderer->p_machineContext->features11;
  features11_ref.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
  features11_ref.shaderDrawParameters = VK_TRUE;
  features11_ref.pNext                = &features2_ref;

  VkPhysicalDeviceVulkan13Features& r_features13 = renderer->p_machineContext->features13;
  r_features13.sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
  r_features13.dynamicRendering = VK_TRUE;
  r_features13.pNext            = &features11_ref;

  VkDeviceCreateInfo& r_deviceCreateInfo      = renderer->p_machineContext->logicDeviceCreateInfo;
  r_deviceCreateInfo.sType                    = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  r_deviceCreateInfo.pQueueCreateInfos        = &queueCreateInfo_ref;
  r_deviceCreateInfo.queueCreateInfoCount     = 1;
  r_deviceCreateInfo.enabledExtensionCount    = s_requiredDeviceExtensionCount;
  r_deviceCreateInfo.ppEnabledExtensionNames  = s_requiredDeviceExtensions;
  r_deviceCreateInfo.pNext = &r_features13;

  if (vkCreateDevice(renderer->p_context->physicalDevice,
                     &r_deviceCreateInfo,
                     nullptr,
                     &renderer->p_context->logicDevice) != VK_SUCCESS)
    panic(1==1,"Culdn't create logical device!");

  // Setup of Graphics Queue

  vkGetDeviceQueue(renderer->p_context->logicDevice,
                   renderer->p_context->graphicsFamilyIndex,
                   0,
                   &renderer->p_context->graphicQueue);

}

void setup_swapchain_vk(kRendererCore_vk* renderer) {

  VkPhysicalDevice& r_physicalDevice = renderer->p_context->physicalDevice;
  VkSurfaceKHR&     r_surface = renderer->p_context->viewPort.surface;

  // Query Basic capabilities (min/max number of images in swapchain, width
  // height of images)
  //
  VkSurfaceCapabilitiesKHR& surfaceCapabilities = renderer->p_machineContext->surfaceCapabilities;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_physicalDevice,
                                            r_surface,
                                            &surfaceCapabilities);

  // Query and Choose surface formats (pixel format, color space )
  //
  uint32_t& surfaceFormatCount = renderer->p_machineContext->surfaceFormatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(r_physicalDevice,
                                       r_surface,
                                       &surfaceFormatCount,
                                       nullptr);
  renderer->p_machineContext->p_surfaceFormats = static_cast<VkSurfaceFormatKHR *>(
      malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount));

  vkGetPhysicalDeviceSurfaceFormatsKHR(r_physicalDevice,
                                       r_surface,
                                       &surfaceFormatCount,
                                       renderer->p_machineContext->p_surfaceFormats);

  for (int i = 0; i < surfaceFormatCount; ++i) {
    if (renderer->p_machineContext->p_surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
        renderer->p_machineContext->p_surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      renderer->p_machineContext->surfaceFormatIndex = i;
      break;
    }
  }
  panic(renderer->p_machineContext->surfaceFormatIndex == UINT32_INVALID_VALUE, "Couldn't find surface with correct capabilities");

  // Query and Choose Presentation modes (Fifo, mailBox)
  //
  uint32_t presentationModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(r_physicalDevice,
                                            r_surface,
                                            &presentationModeCount,
                                            nullptr);

  VkPresentModeKHR *presentationModes = static_cast<VkPresentModeKHR *>(
      malloc(sizeof(VkPresentModeKHR) * presentationModeCount));
  vkGetPhysicalDeviceSurfacePresentModesKHR(r_physicalDevice,
                                            r_surface,
                                            &presentationModeCount,
                                            presentationModes);

  uint32_t PresentIndex = 0;
  for (int i = 0; i < presentationModeCount; ++i) {
    if (presentationModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      PresentIndex = i;
      break;
    }
  }

  // choose swap extent
  //
  VkExtent2D& surfaceExtent = renderer->p_context->viewPort.surfaceExtent;
  GLFWwindow* window = renderer->p_context->window;

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

  uint32_t minImageCount = (s_maxImageCount > surfaceCapabilities.minImageCount) ? s_maxImageCount :
                                                                                   surfaceCapabilities.minImageCount;

  if ((0 < surfaceCapabilities.maxImageCount) &&
      (surfaceCapabilities.maxImageCount < minImageCount)) {
    minImageCount = surfaceCapabilities.maxImageCount;
  }

  // Create the swapchain

  uint32_t& surfaceFormatIndex       = renderer->p_machineContext->surfaceFormatIndex;
  VkSurfaceFormatKHR* surfaceFormats = renderer->p_machineContext->p_surfaceFormats;

  VkSwapchainCreateInfoKHR& swapchainCreateInfo = renderer->p_machineContext->swapchainCreateInfo;
  swapchainCreateInfo.sType              = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.surface           = r_surface;
  swapchainCreateInfo.minImageCount     = minImageCount;
  swapchainCreateInfo.imageFormat       = surfaceFormats[surfaceFormatIndex].format;
  swapchainCreateInfo.imageColorSpace   = surfaceFormats[surfaceFormatIndex].colorSpace;
  swapchainCreateInfo.imageExtent       = surfaceExtent;
  swapchainCreateInfo.imageArrayLayers  = 1;
  swapchainCreateInfo.imageUsage        = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainCreateInfo.imageSharingMode  = VK_SHARING_MODE_EXCLUSIVE;
  swapchainCreateInfo.preTransform      = surfaceCapabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha    = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode       = presentationModes[PresentIndex];
  swapchainCreateInfo.clipped           = true;


  VkDevice& device = renderer->p_context->logicDevice;

  VkSwapchainKHR& swapchain            = renderer->p_swapchain->swapchain;
  uint32_t&       imageCount           = renderer->p_swapchain->imageCount;

  if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
    panic(1, "Unable to create swapchain!!!");

  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
  renderer->p_swapchain->p_swapChainImages = static_cast<VkImage *>(malloc(sizeof(VkImage) * imageCount));
  vkGetSwapchainImagesKHR(device, swapchain, &imageCount, renderer->p_swapchain->p_swapChainImages);

  // Create the image views

  renderer->p_swapchain->p_swapChainImageViews = static_cast<VkImageView*>(malloc(sizeof(VkImageView) * imageCount));

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.viewType         = VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.format           = surfaceFormats[surfaceFormatIndex].format;
  imageViewCreateInfo.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                          .levelCount = 1,
                                          .layerCount = 1};
  imageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY,
                                    VK_COMPONENT_SWIZZLE_IDENTITY};

  VkImage*     swapChainImages     = renderer->p_swapchain->p_swapChainImages;
  VkImageView* swapChainImageViews = renderer->p_swapchain->p_swapChainImageViews;

  for (int i = 0; i < imageCount; ++i) {
    imageViewCreateInfo.image = swapChainImages[i];
    vkCreateImageView(device, &imageViewCreateInfo, nullptr,
                      &swapChainImageViews[i]);
  }
  free(presentationModes);

}

void setup_graphic_pipeline_vk(kRendererCore_vk* renderer) {

  VkDevice&       device       = renderer->p_context->logicDevice;
  VkShaderModule& shaderModule = renderer->p_pipeline->shaderModule;

  //TODO crashout in windows rn
  std::vector<char> shaderCode = readFile(SHADERS_SOURCE_DIR + std::string("/slang.spv"));

  // Create shader module

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType     = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize  = shaderCode.size() * sizeof(char);
  createInfo.pCode     = reinterpret_cast<const uint32_t *>(shaderCode.data());

  vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

  // Instanciate shader stages

  VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
  vertexShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStageInfo.module = shaderModule;
  vertexShaderStageInfo.pName  = "vertMain";

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
  fragmentShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = shaderModule;
  fragmentShaderStageInfo.pName  = "fragMain";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo,
                                                    fragmentShaderStageInfo};

  // [Construct pipeline]

  // Pipeline Stages
  //  -- vertexInput -- ignored
  //  -- inputAssembly
  //  -- dynamicStates [viewPort & scissors]
  //  -- rasterizer
  //  -- multisampling -- ignored
  //  -- colorBlending [Attatchment & Blending]

  VkPipelineVertexInputStateCreateInfo& vertexInputState = renderer->p_pipeline->vertexInputState;
  vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkPipelineInputAssemblyStateCreateInfo& inputAssemblyState = renderer->p_pipeline->inputAssemblyState; // descartar al pasar a mesh rendering
  inputAssemblyState.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

  VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                    VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo& dynamicState = renderer->p_pipeline->dynamicState;
  dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = 2; // hardcode TODO
  dynamicState.pDynamicStates    = dynamicStates;

  VkPipelineViewportStateCreateInfo& viewportState = renderer->p_pipeline->viewPortState;
  viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount  = 1;

  VkPipelineRasterizationStateCreateInfo& rasterizerState = renderer->p_pipeline->rasterizerState;
  rasterizerState.sType            = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerState.depthClampEnable = VK_FALSE;
  rasterizerState.polygonMode      = VK_POLYGON_MODE_FILL;
  rasterizerState.cullMode         = VK_CULL_MODE_BACK_BIT;
  rasterizerState.frontFace        = VK_FRONT_FACE_CLOCKWISE;
  rasterizerState.depthBiasEnable  = VK_FALSE;
  rasterizerState.lineWidth        = 1.0f;

  VkPipelineMultisampleStateCreateInfo& multisamplingState = renderer->p_pipeline->multisamplingState;
  multisamplingState.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisamplingState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisamplingState.sampleShadingEnable  = VK_FALSE;

  VkPipelineColorBlendAttachmentState& colorBlendingAttachmentState = renderer->p_pipeline->colorBlendingAttachmentState;
  //colorBlendingAttachmentState.sType          = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_ATTACHMENT_STATE_CREATE_INFO;
  colorBlendingAttachmentState.blendEnable    = VK_FALSE;
  colorBlendingAttachmentState.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

  VkPipelineColorBlendStateCreateInfo& colorBlendingState = renderer->p_pipeline->colorBlendindState;
  colorBlendingState.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlendingState.logicOpEnable   = VK_FALSE;
  colorBlendingState.logicOp         = VK_LOGIC_OP_COPY;
  colorBlendingState.attachmentCount = 1;
  colorBlendingState.pAttachments = &colorBlendingAttachmentState;

  // Pipeline layout 

  VkPipelineLayoutCreateInfo& pipelineLayoutInfo = renderer->p_pipeline->pipelineLayoutInfo;
  pipelineLayoutInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount         = 0;
  pipelineLayoutInfo.pushConstantRangeCount = 0;

  vkCreatePipelineLayout(device,
                         &pipelineLayoutInfo,
                         nullptr,
                         &renderer->p_pipeline->pipelineLayout);

  VkPipelineRenderingCreateInfo& pipelineRenderingCreateInfo = renderer->p_pipeline->pipelineRenderingCreateInfo;
  pipelineRenderingCreateInfo.sType                    = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  pipelineRenderingCreateInfo.colorAttachmentCount    = 1;
  pipelineRenderingCreateInfo.pColorAttachmentFormats = &renderer->p_machineContext->p_surfaceFormats[renderer->p_machineContext->surfaceFormatIndex].format;

  VkGraphicsPipelineCreateInfo& graphicsPipelineCreateInfo = renderer->p_pipeline->graphicsPipelineCreateInfo;
  graphicsPipelineCreateInfo.sType                 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicsPipelineCreateInfo.stageCount           = 2;
  graphicsPipelineCreateInfo.pStages              = shaderStages;
  graphicsPipelineCreateInfo.pVertexInputState    = &vertexInputState;
  graphicsPipelineCreateInfo.pInputAssemblyState  = &inputAssemblyState;
  graphicsPipelineCreateInfo.pViewportState       = &viewportState;
  graphicsPipelineCreateInfo.pRasterizationState  = &rasterizerState;
  graphicsPipelineCreateInfo.pMultisampleState    = &multisamplingState;
  graphicsPipelineCreateInfo.pColorBlendState     = &colorBlendingState;
  graphicsPipelineCreateInfo.pDynamicState        = &dynamicState;
  graphicsPipelineCreateInfo.layout               = renderer->p_pipeline->pipelineLayout;
  graphicsPipelineCreateInfo.renderPass           = nullptr;
  graphicsPipelineCreateInfo.pNext                = &pipelineRenderingCreateInfo;

  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &renderer->p_pipeline->graphicsPipeline) != VK_SUCCESS)
    panic(1,"Unable to create graphics pipeline");
}


void setup_command_pool_vk(kRendererCore_vk* renderer){

  VkDevice& device               = renderer->p_context->logicDevice;

  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = renderer->p_context->graphicsFamilyIndex;

  vkCreateCommandPool(device, &poolInfo, nullptr, &renderer->concurrence.commandPool);

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool        = renderer->concurrence.commandPool;
  allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  vkAllocateCommandBuffers(device, &allocInfo, &renderer->concurrence.commandBuffer);

}

void record_command_buffer(kRendererCore_vk* renderer, uint32_t imageIndex){

  VkCommandBuffer& commandBuffer = renderer->concurrence.commandBuffer;

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vkBeginCommandBuffer(commandBuffer, &beginInfo);

  // Transition imagen layout from undefined to eColorAttatchmentOptimal

  VkImageMemoryBarrier2 barrier{};
  barrier.srcStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.srcAccessMask       = {};
  barrier.dstStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.dstAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image               = renderer->p_swapchain->p_swapChainImages[imageIndex];
  barrier.subresourceRange = {.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel   = 0,
                              .levelCount     = 1,
                              .baseArrayLayer = 0,
                              .layerCount     = 1};

  VkDependencyInfo dependency_info        = {};
  dependency_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dependency_info.dependencyFlags         = {};
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers    = &barrier;

  vkCmdPipelineBarrier2(renderer->concurrence.commandBuffer, &dependency_info);

  // Setup clear color (black backgroundn)

  VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

  VkRenderingAttachmentInfo attachmentInfo{};
  attachmentInfo.imageView   = renderer->p_swapchain->p_swapChainImageViews[imageIndex];
  attachmentInfo.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
  attachmentInfo.loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR;
  attachmentInfo.storeOp     = VK_ATTACHMENT_STORE_OP_STORE;
  attachmentInfo.clearValue  = clearColor;

  // Set rendering info & start recording

  VkRenderingInfo renderingInfo = {};
  renderingInfo.layerCount           = 1;
  renderingInfo.colorAttachmentCount = 1;
  renderingInfo.pColorAttachments    = &attachmentInfo;
  renderingInfo.renderArea           = {.offset = {0, 0},
                                        .extent = renderer->p_context->viewPort.surfaceExtent};

  vkCmdBeginRendering(commandBuffer, &renderingInfo);

  // Bind GRAPHIC pipeline and set ViewPort and Scissors
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    renderer->p_pipeline->graphicsPipeline);

  VkViewport viewPort = {0.0f,
                         0.0f,
                         static_cast<float>(renderer->p_context->viewPort.surfaceExtent.width),
                         static_cast<float>(renderer->p_context->viewPort.surfaceExtent.height),
                         0.0f,
                         1.0f};

  renderer->p_context->viewPort.instance = viewPort;

  vkCmdSetViewport(commandBuffer, 0, 1, &renderer->p_context->viewPort.instance);

  VkRect2D scissors = {VkOffset2D{0, 0}, renderer->p_context->viewPort.surfaceExtent};
  vkCmdSetScissor(commandBuffer, 0, 1, &scissors);

  // DRAW THE TRIANGLE GAD DAMN

  vkCmdDraw(commandBuffer, 3, 1, 0, 0);

  // End rendering and transition to PresentSrcKHR
  vkCmdEndRendering(commandBuffer);

  barrier.srcStageMask        = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
  barrier.srcAccessMask       = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstStageMask        = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
  barrier.dstAccessMask       = {};
  barrier.oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  barrier.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image               = renderer->p_swapchain->p_swapChainImages[imageIndex];
  barrier.subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = 0,
                              .levelCount = 1,
                              .baseArrayLayer = 0,
                              .layerCount = 1};

  dependency_info.dependencyFlags         = {};
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers    = &barrier;

  vkCmdPipelineBarrier2(commandBuffer, &dependency_info);

  // End commandBuffer
  vkEndCommandBuffer(commandBuffer);
}

void create_sync_objects(kRendererCore_vk* renderer){

  VkDevice& device = renderer->p_context->logicDevice;


  VkSemaphoreCreateInfo semaphoreCreateInfo{};
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                    &renderer->concurrence.presentCompleteSemaphore);
  vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                    &renderer->concurrence.renderFinishedSemaphore);

  VkFenceCreateInfo fenceCreateInfo {};
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(device, &fenceCreateInfo, nullptr, &renderer->concurrence.drawFence);
}

void entrypoint_render_frame(){
  kRendererCore_vk* renderer = get_renderer_core_instance_vk();

  VkDevice&        device                   = renderer->p_context->logicDevice;
  VkSwapchainKHR&  swapchain                = renderer->p_swapchain->swapchain;
  uint32_t&        imageCount               = renderer->p_swapchain->imageCount;
  VkCommandBuffer& commandBuffer            = renderer->concurrence.commandBuffer;
  VkFence&         drawFence                = renderer->concurrence.drawFence;
  VkSemaphore&     presentCompleteSemaphore = renderer->concurrence.presentCompleteSemaphore;
  VkSemaphore&     renderFinishedSemaphore  = renderer->concurrence.renderFinishedSemaphore;

  VkResult result = vkWaitForFences(device, 1, &drawFence, VK_TRUE, UINT64_MAX);
  if (result != VK_SUCCESS)
    panic(1, "Fences failed");

  uint32_t image_index = -1;

  vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, presentCompleteSemaphore,
                        nullptr, &image_index);

  panic(image_index == -1 , "Unable to acquire next image at rendering");

  record_command_buffer(renderer, image_index);

  VkPipelineStageFlags waitDestinationStageMask = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

  VkSubmitInfo submitInfo{};
  submitInfo.waitSemaphoreCount   = 1;
  submitInfo.pWaitSemaphores      = &presentCompleteSemaphore;
  submitInfo.pWaitDstStageMask    = &waitDestinationStageMask;
  submitInfo.commandBufferCount   = 1;
  submitInfo.pCommandBuffers      = &commandBuffer;
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores    = &renderFinishedSemaphore;

  vkQueueSubmit(renderer->p_context->graphicQueue, 1, &submitInfo, drawFence);

  VkPresentInfoKHR presentInfoKHR{};
  presentInfoKHR.waitSemaphoreCount = 1;
  presentInfoKHR.pWaitSemaphores    = &renderFinishedSemaphore;
  presentInfoKHR.swapchainCount     = 1;
  presentInfoKHR.pSwapchains        = &swapchain;
  presentInfoKHR.pImageIndices      = &image_index;

  presentInfoKHR.pResults = nullptr;

  vkQueuePresentKHR(renderer->p_context->graphicQueue, &presentInfoKHR);
 
}
