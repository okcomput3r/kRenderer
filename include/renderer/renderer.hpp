#ifndef K_RENDERER_HEADER
#define K_RENDERER_HEADER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <cstdint>

// define max value of uint32_t as an unreachable/invalid value for debug checking
#define UINT32_INVALID_VALUE UINT32_MAX

// default values for window dimensions
static constexpr uint32_t s_defaultWindowSize_x {1920};
static constexpr uint32_t s_defaultWindowSize_y {1080};

static constexpr uint32_t s_maxImageCount {3};

static constexpr uint32_t s_requiredDeviceExtensionCount = 1;
static constexpr char*    s_requiredDeviceExtensions[s_requiredDeviceExtensionCount]   = {
                                                            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                            };

static constexpr uint32_t s_validationLayersCount = 1;
static const     char*    s_validationLayers[s_validationLayersCount]    = { "VK_LAYER_KHRONOS_validation" };

#ifdef __DEBUG__
constexpr uint8_t enableValidationLayers = 1;
#else
constexpr uint8_t enableValidationLayers = 0;
#endif

//#include <memory/memory.hpp>
//#include <utils/logs.hpp>

/**
 *  TODO
 *  render config es una estructura de datos que se crea
 *  a base de un documento .json que especifica la configuración de 
 *  el renderizador
 *
 *  privado
 *
 *
 */
struct s_RedererDescriptor;
typedef s_RedererDescriptor RendererDescriptor;

// Functions

void init_module_renderer(); // TODO remove later
GLFWwindow* retrieve_renderer_window();
void init_module_renderer(GLFWwindow* window);
void init_module_renderer(GLFWwindow* window, char*  renderConfig_path); // TODO implementation of rendererDescriptor
void cleanup_module_renderer();



void entrypoint_render_frame();

// igual pasar a la interfaz privada
void load_renderer_configuration(char* configPath, RendererDescriptor& renderConfig); // TODO
void parse_renderer_configuration(char* rawText, RendererDescriptor& renderConfig);   // TODO
RendererDescriptor& get_renderer_configuration(); // TODO



#endif /* END of K_RENDERER_HEADER */
