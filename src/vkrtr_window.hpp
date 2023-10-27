#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>


namespace vkrtr {
   
class VkrtrWindow {
public:
    VkrtrWindow(int width, int height, std::string name);
    ~VkrtrWindow();

    //We do not want multiple pointers pointing to the underlying GLFWwindow
    VkrtrWindow(const VkrtrWindow&) = delete;
    VkrtrWindow& operator=(const VkrtrWindow&) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    void initWindow();

private:
    const int width;
    const int height;

    std::string windowName;
    GLFWwindow* window;
};

}