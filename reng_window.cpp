#include "reng_window.hpp"

// std
#include <stdexcept>

namespace reng {

RengWindow::RengWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
  initWindow();
}

RengWindow::~RengWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void RengWindow::initWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void RengWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to craete window surface");
  }
}

void RengWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height){
    auto lveWindow = reinterpret_cast<RengWindow *>(glfwGetWindowUserPointer(window));
    lveWindow->frameBufferResized = true;
    lveWindow->width = width;
    lveWindow->height = height;
  }
}  // namespace reng
