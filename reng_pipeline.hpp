  
#pragma once

#include "reng_device.hpp"

// std
#include <string>
#include <vector>

namespace reng {

struct PipelineConfigInfo {

  VkPipelineViewportStateCreateInfo viewportInfo;
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
  std::vector<VkDynamicState> dynamicStateEnables;
  VkPipelineDynamicStateCreateInfo dynamicStateInfo;
  VkPipelineLayout pipelineLayout = (uint64_t)nullptr;
  VkRenderPass renderPass = (uint64_t)nullptr;
  uint32_t subpass = 0;
};

class RengPipeline {
 public:
  RengPipeline(
      RengDevice& device,
      const std::string& vertFilepath,
      const std::string& fragFilepath,
      const PipelineConfigInfo& configInfo);
  ~RengPipeline();

  RengPipeline(const RengPipeline&) = delete;
  RengPipeline& operator=(const RengPipeline&) = delete;

  void bind(VkCommandBuffer commandBuffer);

  static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

 private:
  static std::vector<char> readFile(const std::string& filepath);

  void createGraphicsPipeline(
      const std::string& vertFilepath,
      const std::string& fragFilepath,
      const PipelineConfigInfo& configInfo);

  void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

  RengDevice& rengDevice;
  VkPipeline graphicsPipeline;
  VkShaderModule vertShaderModule;
  VkShaderModule fragShaderModule;
};
}  // namespace reng