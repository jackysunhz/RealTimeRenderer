#pragma once

#include <string>
#include <vector>

namespace vkrtr {

class VkrtrPipeline {
public:
    VkrtrPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
    //~VkrtrPipeline();
private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
};

}