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

    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    void initWindow();

private:
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    int width;
    int height;

    std::string windowName;
    GLFWwindow* window;

    bool framebufferResized = false;
};

}