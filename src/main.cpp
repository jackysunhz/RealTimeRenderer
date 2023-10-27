// RealTimeRenderer.cpp : Defines the entry point for the application.
//

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "test_app.hpp"

using namespace std;

int main()
{
    // auto config = vkrtr::PipelineConfigInfo{};
    // config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    // config.colorBlendInfo.attachmentCount = 1;
    // config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;

    // std::cout << config.colorBlendAttachment.srcColorBlendFactor << std::endl;
    // std::cout << config.colorBlendInfo.pAttachments->srcColorBlendFactor << std::endl;

	vkrtr::TestApp app;
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
