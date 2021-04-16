#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
namespace reng {

class RengWindow {
 public:
  RengWindow(int w, int h, std::string name);
  ~RengWindow();

  RengWindow(const RengWindow &) = delete;
  RengWindow &operator=(const RengWindow &) = delete;

  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }
  bool wasWindowResized() {return frameBufferResized; }
  void resetWindowResizedFlag() { frameBufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

 private:
  static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
  void initWindow();

  int width;
  int height;
  bool frameBufferResized;

  std::string windowName;
  GLFWwindow *window;
};
}  // namespace reng
