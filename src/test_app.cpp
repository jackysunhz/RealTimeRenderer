#include "test_app.hpp"

namespace vkrtr {

    void TestApp::run()
    {
        while(!vkrtrWindow.shouldClose()){
            glfwPollEvents();
        }
    }
}