#include "vkrtr_window.hpp"

namespace vkrtr {

VkrtrWindow::VkrtrWindow(int width, int height, std::string name): width(width), height(height), windowName(name) {
    initWindow();
}

VkrtrWindow::~VkrtrWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VkrtrWindow::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

}
