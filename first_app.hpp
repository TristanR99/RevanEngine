#pragma once

#include "reng_device.hpp"
#include "reng_model.hpp"
#include "reng_pipeline.hpp"
#include "reng_swap_chain.hpp"
#include "reng_window.hpp"

// std
#include <memory>
#include <vector>

namespace reng {
class FirstApp {
 public:
  static constexpr int WIDTH = 800;
  static constexpr int HEIGHT = 600;

  FirstApp();
  ~FirstApp();

  FirstApp(const FirstApp &) = delete;
  FirstApp &operator=(const FirstApp &) = delete;

  void run();

 private:
  void loadModels();
  void createPipelineLayout();
  void createPipeline();
  void createCommandBuffers();
  void drawFrame();
  void recreateSwapChain();
  void recordCommandBuffer(int imageIndex);

  RengWindow rengWindow{WIDTH, HEIGHT, "Tutorial 07"};
  RengDevice rengDevice{rengWindow};
  std::unique_ptr<RengSwapChain> rengSwapChain;
  std::unique_ptr<RengPipeline> rengPipeline;
  VkPipelineLayout pipelineLayout;
  std::vector<VkCommandBuffer> commandBuffers;
  std::unique_ptr<RengModel> rengModel;
};
}  // namespace reng
