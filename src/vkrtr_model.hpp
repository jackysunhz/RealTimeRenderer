#pragma once

#include "vkrtr_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vkrtr {

class VkrtrModel {
public:

    struct Vertex
    {
        glm::vec2 position;
        static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        /* data */
    };
    

    VkrtrModel(VkrtrDevice& device, const std::vector<Vertex>& vertices);
    ~VkrtrModel();

    VkrtrModel(const VkrtrModel &) = delete;
    void operator=(const VkrtrModel &) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    VkrtrDevice &vkrtrDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;

    void createVertexBuffers(const std::vector<Vertex> &vertices);
    



};

} // namespace vkrtr