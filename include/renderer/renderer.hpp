#ifndef K_RENDERER_HEADER
#define K_RENDERER_HEADER

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

// define max value of uint32_t as an unreachable/invalid value for debug checking
#define UINT32_INVALID_VALUE UINT32_MAX

// default values for window dimensions
static constexpr uint32_t s_defaultWindowSize_x {1920};
static constexpr uint32_t s_defaultWindowSize_y {1080};

static constexpr uint32_t s_maxImageCount {3};

static const char* s_requiredDeviceExtensions[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
static const uint32_t s_requiredDeviceExtensionCount = 1;

#include <vector>
//#include <memory/memory.hpp>
//#include <utils/logs.hpp>

#include <cstdint>


/**
 *  TODO
 *  render config es una estructura de datos que se crea
 *  a base de un documento .json que especifica la configuración de 
 *  el renderizador
 *
 *
 *
 */
struct s_RedererDescriptor{


};
typedef s_RedererDescriptor RendererDescriptor;

/**
 * TODO
 * El core del renderizador contiene todas las estructuras de datos necesarias
 * para que funcione (?)
 *
 * obscure implementation
 * declaration on renderer.cpp
 */
struct s_kRendererCore_vk;
typedef s_kRendererCore_vk kRendererCore_vk;

// Functions

void init_module_renderer();
void init_module_renderer(RendererDescriptor& renderConfig); // TODO implementation of rendererDescriptor
void cleanup_module_renderer();

void entrypoint_render_frame();

// igual pasar a la interfaz privada 
void load_renderer_configuration(char* configPath, RendererDescriptor& renderConfig); // TODO
void parse_renderer_configuration(char* rawText, RendererDescriptor& renderConfig);   // TODO
RendererDescriptor& get_renderer_configuration(); // TODO



#endif /* END of K_RENDERER_HEADER */
