#pragma once

#include "vkrtr_window.hpp"
#include "vkrtr_pipeline.hpp"

namespace vkrtr {

class TestApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();
private:
    VkrtrWindow vkrtrWindow{WIDTH, HEIGHT, "TestApp"};
    VkrtrPipeline vkrtrPipeline{"../../shaders/simple_shader.vert", "../../shaders/simple_shader.frag"};
};

}