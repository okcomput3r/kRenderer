#ifndef K_RENDERER_HEADER
#define K_RENDERER_HEADER

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>


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
typedef s_RedererDescriptor RendererDescriptor_vk;


struct s_ViewPort {
  uint32_t width;
  uint32_t height;

};
typedef s_ViewPort ViewPort;


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
void init_module_renderer(RendererDescriptor_vk& renderConfig); // TODO implementation of rendererDescriptor
void cleanup_module_renderer();

void entrypoint_render_frame();

void load_renderer_configuration(char* configPath, RendererDescriptor_vk& renderConfig); // TODO
RendererDescriptor_vk& get_renderer_configuration(); // TODO



#endif /* END of K_RENDERER_HEADER */
