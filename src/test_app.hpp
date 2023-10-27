#pragma once

#include "vkrtr_window.hpp"

namespace vkrtr {

class TestApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();
private:
    VkrtrWindow vkrtrWindow{WIDTH, HEIGHT, "TestApp"};
};

}