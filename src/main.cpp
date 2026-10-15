#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>


//#include <memory/memory.hpp>

//#define _OK_LOGS_IMPL
//#include <utils/logs.hpp>

#include <renderer/renderer.hpp>

void entrypoint_mainloop_Vk() {

  while (!glfwWindowShouldClose(retrieve_renderer_window())) {
    glfwPollEvents();
    //draw_frame_vk();
    entrypoint_render_frame();
  }
}
/*
void cleanup_Vk() {

  // cleanup swapchain info
  free(swapChainImages);
  free(swapChainImageViews);
  free(surfaceFormats);

  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);

  glfwDestroyWindow(window);
  glfwTerminate();
}
*/

int main(int argc, char **argv) {

  init_module_renderer();
  entrypoint_mainloop_Vk();

  return 0;
}
