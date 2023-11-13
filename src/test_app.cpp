#include "test_app.hpp"
#include <stdexcept>
#include <iostream>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace vkrtr {

struct SimplePushConstantData {
    glm::vec2 offset;//offset of the triangle
    alignas(16) glm::vec3 color;//color of the triangle
};

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
    std::cout << "maxPushConstantSize: " << vkrtrDevice.properties.limits.maxPushConstantsSize << "\n";
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
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;//this push constant range is shared by both vertex and fragment shader
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;             
    pipelineLayoutInfo.pSetLayouts = nullptr;           
    pipelineLayoutInfo.pushConstantRangeCount = 1;      
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;   

    if (vkCreatePipelineLayout(vkrtrDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void TestApp::createPipeline()
{
    assert(vkrtrSwapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

    PipelineConfigInfo pipelineConfig{};
    VkrtrPipeline::defaultPipelineConfigInfo(pipelineConfig);

    pipelineConfig.renderPass = vkrtrSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    //This is just an easy fix to the problem when creating default config info
    //pipelineConfig.colorBlendInfo.pAttachments = &pipelineConfig.colorBlendAttachment;
    vkrtrPipeline = std::make_unique<VkrtrPipeline>(
        vkrtrDevice, 
        "../../shaders/simple_shader.vert.spv", 
        "../../shaders/simple_shader.frag.spv", 
        pipelineConfig
    );
}

void TestApp::createCommandBuffers(){
    commandBuffers.resize(vkrtrSwapChain->imageCount());//we are using a one to one relationship between swap chain images and command buffers

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkrtrDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if(vkAllocateCommandBuffers(vkrtrDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS){
        throw std::runtime_error("Failed to allocate command buffers");
    }

}
void TestApp::freeCommandBuffers()
{
    vkFreeCommandBuffers(
        vkrtrDevice.device(),
        vkrtrDevice.getCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data()
    );

    commandBuffers.clear();
}
void TestApp::drawFrame()
{
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
    if(vkrtrSwapChain == nullptr){
        vkrtrSwapChain = std::make_unique<VkrtrSwapChain>(vkrtrDevice, extent);
    }else{
        vkrtrSwapChain = std::make_unique<VkrtrSwapChain>(vkrtrDevice, extent, std::move(vkrtrSwapChain));
        if(vkrtrSwapChain->imageCount() != commandBuffers.size()){
            freeCommandBuffers();
            createCommandBuffers();
        }
    }
    
    //if render pass compatible no need to create new pipeline
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

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(vkrtrSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(vkrtrSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, vkrtrSwapChain->getSwapChainExtent()};

    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    vkrtrPipeline->bind(commandBuffers[imageIndex]);
    vkrtrModel->bind(commandBuffers[imageIndex]);

    for (size_t i = 0; i < 10; i++)
    {
        SimplePushConstantData push{};
        push.color = {0.0f, 0.0f, 0.1f + 0.09f * i};
        push.offset = {0.0f, -0.5f + i * 0.09f};
        vkCmdPushConstants(
            commandBuffers[imageIndex],
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push
        );
        vkrtrModel->draw(commandBuffers[imageIndex]);
    }

    //vkrtrModel->draw(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);
    if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
        throw std::runtime_error("Failed to record command buffer");
    }
}
}