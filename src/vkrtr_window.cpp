#include "vkrtr_window.hpp"
#include <stdexcept>

namespace vkrtr {

VkrtrWindow::VkrtrWindow(int width, int height, std::string name): width(width), height(height), windowName(name) {
    initWindow();
}

VkrtrWindow::~VkrtrWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VkrtrWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface");
    }
}

void VkrtrWindow::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void VkrtrWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<VkrtrWindow*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
    app->width = width;
    app->height = height;
}
}
