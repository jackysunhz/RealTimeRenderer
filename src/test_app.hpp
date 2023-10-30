#pragma once

#include "vkrtr_window.hpp"
#include "vkrtr_pipeline.hpp"
#include "vkrtr_device.hpp"
#include "vkrtr_swap_chain.hpp"
#include "vkrtr_model.hpp"

#include <memory>

namespace vkrtr {

class TestApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    TestApp();
    ~TestApp();

    TestApp(const TestApp&) = delete;
    TestApp& operator=(const TestApp&) = delete;

    void run();
private:
    void loadModels();
    VkrtrWindow vkrtrWindow{WIDTH, HEIGHT, "TestApp"};
    VkrtrDevice vkrtrDevice{vkrtrWindow};
    VkrtrSwapChain vkrtrSwapChain{vkrtrDevice, vkrtrWindow.getExtent()};
    std::unique_ptr<VkrtrPipeline> vkrtrPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<VkrtrModel> vkrtrModel;

    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();
};

}