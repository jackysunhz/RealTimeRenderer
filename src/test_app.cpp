#include "test_app.hpp"
#include <stdexcept>
#include <iostream>
#include <array>

namespace vkrtr {
TestApp::TestApp()
{
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
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
        drawFrame();
    }

    vkDeviceWaitIdle(vkrtrDevice.device());
}

void TestApp::loadModels()
{
    std::vector<VkrtrModel::Vertex> vertices{
        //position,      color
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f},  {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
    };

    vkrtrModel = std::make_unique<VkrtrModel>(vkrtrDevice, vertices);
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
    auto pipelineConfig = VkrtrPipeline::defaultPipelineConfigInfo(vkrtrSwapChain->width(), vkrtrSwapChain->height());
    pipelineConfig.renderPass = vkrtrSwapChain->getRenderPass();
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

void TestApp::createCommandBuffers(){
    commandBuffers.resize(vkrtrSwapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkrtrDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(vkrtrDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffers");
    }

}
void TestApp::drawFrame(){
    uint32_t imageIndex;
    auto result = vkrtrSwapChain->acquireNextImage(&imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR){
        recreateSwapChain();
        return;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    recordCommandBuffer(imageIndex);

    result = vkrtrSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vkrtrWindow.wasWindowResized()){
        vkrtrWindow.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    if(result != VK_SUCCESS){
        throw std::runtime_error("Failed to present swap chain image");
    }
}

void TestApp::recreateSwapChain()
{
    auto extent = vkrtrWindow.getExtent();
    while(extent.width == 0 || extent.height == 0){
        extent = vkrtrWindow.getExtent();
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(vkrtrDevice.device());
    vkrtrSwapChain = std::make_unique<VkrtrSwapChain>(vkrtrDevice, extent);
    createPipeline();
}

void TestApp::recordCommandBuffer(int imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("Failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = vkrtrSwapChain->getRenderPass();
        renderPassInfo.framebuffer = vkrtrSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = vkrtrSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkrtrPipeline->bind(commandBuffers[imageIndex]);
        vkrtrModel->bind(commandBuffers[imageIndex]);
        vkrtrModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
            throw std::runtime_error("Failed to record command buffer");
        }
}
}