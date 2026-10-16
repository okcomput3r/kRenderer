#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <iostream>

//#include <memory/memory.hpp>

//#define _OK_LOGS_IMPL
//#include <utils/logs.hpp>

#include <renderer/renderer.hpp>

GLFWwindow* window = nullptr;

void entrypoint_mainloop_Vk() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
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

void get_GLFW_window(){

  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(1920,
                            1080,
                            "kEngine",
                            nullptr,
                            nullptr);

}


int main(int argc, char **argv) {

  get_GLFW_window();

  init_module_renderer(window);
  entrypoint_mainloop_Vk();

  return 0;
}
