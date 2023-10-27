#include "test_app.hpp"
#include <stdexcept>
#include <iostream>

namespace vkrtr {
TestApp::TestApp()
{
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
}

TestApp::~TestApp()
{
    vkDestroyPipelineLayout(vkrtrDevice.device(), pipelineLayout, nullptr);
}

void TestApp::run()
{
    while(!vkrtrWindow.shouldClose()){
        glfwPollEvents();
    }
}

void TestApp::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;             
    pipelineLayoutInfo.pSetLayouts = nullptr;           
    pipelineLayoutInfo.pushConstantRangeCount = 0;      
    pipelineLayoutInfo.pPushConstantRanges = nullptr;   

    if (vkCreatePipelineLayout(vkrtrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void TestApp::createPipeline()
{
    auto pipelineConfig = VkrtrPipeline::defaultPipelineConfigInfo(vkrtrSwapChain.width(), vkrtrSwapChain.height());
    pipelineConfig.renderPass = vkrtrSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;

    //This is just an easy fix to the problem when creating default config info
    pipelineConfig.colorBlendInfo.pAttachments = &pipelineConfig.colorBlendAttachment;

    vkrtrPipeline = std::make_unique<VkrtrPipeline>(
        vkrtrDevice, 
        "../../shaders/simple_shader.vert.spv", 
        "../../shaders/simple_shader.frag.spv", 
        pipelineConfig
    );
}

void TestApp::createCommandBuffers(){}
void TestApp::drawFrame(){}

}