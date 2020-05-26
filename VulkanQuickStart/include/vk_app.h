#pragma once

/*

This file is part of the VulkanQuickStart Project.

	The VulkanQuickStart Project is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	The VulkanQuickStart Project is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	This link provides the exact terms of the GPL license <https://www.gnu.org/licenses/>.

	The author's interpretation of GPL 3 is that if you receive money for the use or distribution of the TriMesh Library or a derivative product, GPL 3 no longer applies.

	Under those circumstances, the author expects and may legally pursue a reasoble share of the income. To avoid the complexity of agreements and negotiation, the author makes
	no specific demands in this regard. Compensation of roughly 1% of net or $5 per user license seems appropriate, but is not legally binding.

	In lay terms, if you make a profit by using the VulkanQuickStart Project (violating the spirit of Open Source Software), I expect a reasonable share for my efforts.

	Robert R Tipton - Author

	Dark Sky Innovative Solutions http://darkskyinnovation.com/

*/

#include <defines.h>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <mutex>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "vk_device_context.h"
#include "vk_image.h"
#include <vk_sceneNode.h>
#include <vk_sceneNodeGroup.h>
#include <vk_sceneNode3D.h>
#include <vk_scene.h>
#include <vk_pipeline3D.h>
#include <vk_shaderPool.h>

namespace TriMesh {
	class CMesh;
	using CMeshPtr = std::shared_ptr<CMesh>;
}

namespace VK {
	namespace UI {
		class Window;
		using WindowPtr = std::shared_ptr<Window>;
	}

	struct SwapChain {
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		std::vector<VkFramebuffer> swapChainFramebuffers;
	};

	class Scene;
	using ScenePtr = std::shared_ptr<Scene>;

	class Pipeline;
	using PipelinePtr = std::shared_ptr<Pipeline>;

	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<class VulkanApp>;

	class VulkanApp {
	public:
		using UniformBufferObject = PipelineVertex3D::UniformBufferObject;

		using BoundingBox = CBoundingBox3D<float>;

		VulkanApp(int width, int height);
		~VulkanApp();

		void setUiWindow(UI::Window* uiWindow );

		SceneNode3DPtr addSceneNode3D(const std::string& modelFilename, const std::string& imageFilename);
		SceneNode3DPtr addSceneNode3D(const TriMesh::CMeshPtr& mesh);
		const PipelinePtr& addPipeline(const PipelinePtr& pipeline);

		const ScenePtr& getScene() const;
		void setScene(const ScenePtr& scene);

		SceneNodeGroupConstPtr getRoot3D() const;
		SceneNodeGroupPtr getRoot3D();

		const DeviceContext& getDeviceContext() const;
		DeviceContext& getDeviceContext();

		const SwapChain& getSwapChain() const;
		SwapChain& getSwapChain();

		VkRenderPass getRenderPass() const;
		VkSampleCountFlagBits getAntiAliasSamples() const;
		GLFWwindow* getWindow();
		UI::Window* getUiWindow();
		unsigned int getWindowDpi() const;
		const UI::Window* getUiWindow() const;

		void setModelToWorldTransform(const glm::mat4& xform);
		const glm::mat4& getModelToWorldTransform() const;


		void setModelScale(double scale);
		double getModelScale() const;

		const ShaderPool& getShaderPool() const;
		ShaderPool& getShaderPool();

		void run();

	private:
		struct SwapChainSupportDetails;
		struct QueueFamilyIndices;

		static std::vector<char> readFile(const std::string& filename);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void initWindow(int width, int height);
		void initVulkan();
		void recreateSwapChain();
		void mainLoop();
		void cleanupSwapChain();
		void cleanup();
		void createInstance();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void setupDebugMessenger();
		void createSurface();
		void pickPhysicalDevice();

		void createLogicalDevice();
		void createSwapChain();
		void createImageViews();
		void createRenderPass();
		void createGraphicsPipeline();
		void createFramebuffers();
		void createCommandPool();
		void createColorResources();
		void createDepthResources();
		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat findDepthFormat();
		bool hasStencilComponent(VkFormat format);
		VkSampleCountFlagBits getMaxUsableSampleCount();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createCommandBuffers();
		void drawCmdBufferLoop(size_t swapChainIndex,
			VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo& renderPassInfo);
		void drawPipeline(size_t swapChainIndex, const PipelinePtr& pipeline);
		void createSyncObjects();
		void updateUniformBuffer(uint32_t swapChainImageIndex);
		void reportFPS();
		void drawFrame();
		VkShaderModule createShaderModule(const std::vector<char>& code);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();

		GLFWwindow* _window;
		UI::Window* _uiWindow;
		unsigned int _windowDpi = 72;
		glm::mat4 _modelToWorld;
		double _modelScale = 1.0;
		VkSurfaceKHR surface;
		std::mutex _swapChainMutex;
		size_t _changeNumber = 0, _lastChangeNumber = 0, _uiWindowChangeNumber = 0;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;

		DeviceContext deviceContext;
		VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		VkQueue presentQueue;
		SwapChain _swapChain;

		VkRenderPass renderPass;
		size_t _pipelineVertIdx = stm1, 
			_pipelineSamplerIdx = stm1,
			_pipelineUiIdx = stm1;
		ShaderPoolPtr _shaderPool;
		std::vector<PipelinePtr> _pipelines;

		Image colorImage, depthImage;

		ScenePtr _scene;
		size_t _root3DNode = stm1;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		size_t currentFrame = 0;

		bool framebufferResized = false;
	};

	inline const ScenePtr& VulkanApp::getScene() const {
		return _scene;
	}

	inline void VulkanApp::setScene(const ScenePtr& scene) {
		_scene = scene;
	}

	inline const DeviceContext& VulkanApp::getDeviceContext() const {
		return deviceContext;
	}

	inline DeviceContext& VulkanApp::getDeviceContext() {
		return deviceContext;
	}

	inline const SwapChain& VulkanApp::getSwapChain() const {
		return _swapChain;
	}

	inline SwapChain& VulkanApp::getSwapChain() {
		return _swapChain;
	}

	inline VkRenderPass VulkanApp::getRenderPass() const {
		return renderPass;
	}

	inline VkSampleCountFlagBits VulkanApp::getAntiAliasSamples() const {
		return msaaSamples;
	}

	inline GLFWwindow* VulkanApp::getWindow() {
		return _window;
	}

	inline UI::Window* VulkanApp::getUiWindow() {
		return _uiWindow;
	}

	inline const UI::Window* VulkanApp::getUiWindow() const {
		return _uiWindow;
	}

	inline unsigned int VulkanApp::getWindowDpi() const {
		return _windowDpi;
	}

	inline void VulkanApp::setModelToWorldTransform(const glm::mat4& xform) {
		_modelToWorld = xform;
	}

	inline const glm::mat4& VulkanApp::getModelToWorldTransform() const {
		return _modelToWorld;
	}

	inline void VulkanApp::setModelScale(double scale) {
		_modelScale = scale;
	}

	inline double VulkanApp::getModelScale() const {
		return _modelScale;
	}

	inline const ShaderPool& VulkanApp::getShaderPool() const {
		return *_shaderPool;
	}

	inline ShaderPool& VulkanApp::getShaderPool() {
		return *_shaderPool;
	}

}