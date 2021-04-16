#include "first_app.hpp"

// std
#include <array>
#include <stdexcept>

namespace reng {

FirstApp::FirstApp() {
  loadModels();
  createPipelineLayout();
  recreateSwapChain();
  createCommandBuffers();
}

FirstApp::~FirstApp() { vkDestroyPipelineLayout(rengDevice.device(), pipelineLayout, nullptr); }

void FirstApp::run() {
  while (!rengWindow.shouldClose()) {
    glfwPollEvents();
    drawFrame();
  }

  vkDeviceWaitIdle(rengDevice.device());
}

void FirstApp::loadModels() {
  std::vector<RengModel::Vertex> vertices{
      {{0.0f,  0.0f}, {1.0f, 0.0f, 1.0f}},
      {{-0.5f,-1.0f}, {1.0f, 1.0f, 0.0f}},
      {{-1.0f, 0.0f}, {0.0f, 1.0f, 1.0f}},
      {{-0.5f, 1.0f}, {1.0f, 1.0f, 0.0f}},
      {{0.5f,  1.0f}, {0.0f, 1.0f, 1.0f}},
      {{1.0f,  0.0f}, {1.0f, 1.0f, 0.0f}},
      {{0.5f, -1.0f}, {0.0f, 1.0f, 1.0f}},
      {{-0.5f,-1.0f}, {1.0f, 1.0f, 0.0f}}
      };
  rengModel = std::make_unique<RengModel>(rengDevice, vertices);
}

void FirstApp::createPipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;
  if (vkCreatePipelineLayout(rengDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void FirstApp::createPipeline() {
  PipelineConfigInfo pipelineConfig{};
  RengPipeline::defaultPipelineConfigInfo(
      pipelineConfig,
      rengSwapChain->width(),
      rengSwapChain->height());
  pipelineConfig.renderPass = rengSwapChain->getRenderPass();
  pipelineConfig.pipelineLayout = pipelineLayout;
  rengPipeline = std::make_unique<RengPipeline>(
      rengDevice,
      "shaders/simple_shader.vert.spv",
      "shaders/simple_shader.frag.spv",
      pipelineConfig);
}

void FirstApp::recreateSwapChain(){
  auto extent = rengWindow.getExtent();
  while (extent.width == 0 || extent.height == 0) {
    extent = rengWindow.getExtent();
    glfwWaitEvents();
  }

  vkDeviceWaitIdle(rengDevice.device());
  rengSwapChain = std::make_unique<RengSwapChain>(rengDevice, extent);
  createPipeline();
}

void FirstApp::createCommandBuffers() {
  commandBuffers.resize(rengSwapChain->imageCount());

  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = rengDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  if (vkAllocateCommandBuffers(rengDevice.device(), &allocInfo, commandBuffers.data()) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

void FirstApp::recordCommandBuffer(int imageIndex){
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = rengSwapChain->getRenderPass();
  renderPassInfo.framebuffer = rengSwapChain->getFrameBuffer(imageIndex);

  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = rengSwapChain->getSwapChainExtent();

  std::array<VkClearValue, 2> clearValues{};
  clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
  clearValues[1].depthStencil = {1.0f, 0};
  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  rengPipeline->bind(commandBuffers[imageIndex]);
  rengModel->bind(commandBuffers[imageIndex]);
  rengModel->draw(commandBuffers[imageIndex]);

  vkCmdEndRenderPass(commandBuffers[imageIndex]);
  if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void FirstApp::drawFrame() {
  uint32_t imageIndex;
  auto result = rengSwapChain->acquireNextImage(&imageIndex);

  if(result == VK_ERROR_OUT_OF_DATE_KHR){
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  recordCommandBuffer(imageIndex);
  result = rengSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rengWindow.wasWindowResized()) {
    rengWindow.resetWindowResizedFlag();
    recreateSwapChain();
    return;
  }

  if (result != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }
}

}  // namespace reng