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

#include <vk_defines.h>

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

#include <vk_forwardDeclarations.h>
#include "vk_deviceContext.h"
#include "vk_image.h"
#include <vk_pipelineSceneNode.h>
#include <vk_pipelineSceneNode3D.h>
#include <vk_pipelineSceneNode3DWSampler.h>
#include <vk_pipeline3D.h>
#include <vk_shaderPool.h>

namespace TriMesh {
	class CMesh;
	using CMeshPtr = std::shared_ptr<CMesh>;
}

namespace VK {
	struct SwapChain {
		inline SwapChain(const DeviceContextPtr& context)
		: _colorImage(context)
		, _depthImage(context)
		{}

		VkSwapchainKHR _vkSwapChain;
		VkFormat _imageFormat;
		VkExtent2D _extent;
		std::vector<ImagePtr> _images;
		std::vector<VkImage> _vkImages;
		std::vector<VkImageView> _vkImageViews;
		std::vector<VkFramebuffer> _vkFrameBuffers;
		Image _colorImage, _depthImage;
	};

	class VulkanApp : public std::enable_shared_from_this<VulkanApp> {
	public:
		using UniformBufferObject = Pipeline3D::UniformBufferObject;

		using BoundingBox = CBoundingBox3D<float>;

		VulkanApp(int width, int height);
		~VulkanApp();

		VulkanAppPtr getAppPtr();

		void setUiWindow(const UI::WindowPtr& uiWindow );

		void changed();

		PipelineBasePtr addPipeline(const PipelineBasePtr& pipeline);

		template<class PIPELINE_TYPE>
		PipelinePtr<PIPELINE_TYPE> addPipelineWithSource(const std::string& shaderId, const std::string& vertShaderFilename, const std::string& fragShaderFilename);

		const DeviceContextPtr& getDeviceContext() const;

		const SwapChain& getSwapChain() const;
		SwapChain& getSwapChain();

		VkRenderPass getRenderPass() const;
		void setAntiAliasSamples(VkSampleCountFlagBits samples);
		VkSampleCountFlagBits getAntiAliasSamples() const;
		GLFWwindow* getWindow();
		const UI::WindowPtr& getUiWindow() const;
		unsigned int getWindowDpi() const;

		void setModelToWorldTransform(const glm::mat4& xform);
		const glm::mat4& getModelToWorldTransform() const;


		void setModelScale(double scale);
		double getModelScale() const;

		const ShaderPool& getShaderPool() const;
		ShaderPool& getShaderPool();
		uint32_t getSwapChainIndex() const;

		void run();
		void stop();

		template<typename FUNC_TYPE>
		inline void safeUpdate(bool needToRecreateSwapChain, FUNC_TYPE func) {
			_isReady = false;
			vkDeviceWaitIdle(_deviceContext->device_);
			func();
			if (needToRecreateSwapChain)
				recreateSwapChain();
			_isReady = true;
		}
	private:
		struct SwapChainSupportDetails;
		struct QueueFamilyIndices;

		class PipelineRec {
		public:
			void add(const PipelineBasePtr& pl);

			template<typename FUNC_TYPE>
			inline void iterate(FUNC_TYPE func) {
				std::lock_guard lock(_mutex);
				for (auto& pl : _pipelines) {
					func(pl);
				}
			}

		private:
			std::mutex _mutex;
			std::vector<PipelineBasePtr> _pipelines;
		};

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
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createCommandBuffers();
		void drawCmdBufferLoop(size_t swapChainIndex,
			VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo& renderPassInfo);
		void createSyncObjects();
		void updateUniformBuffer(uint32_t swapChainImageIndex);
		void reportFPS();
		void drawFrame();
		VkShaderModule createShaderModule(const std::vector<char>& code);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D getWindowExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();

		GLFWwindow* _window;
		bool _isRunning = true, _isReady = true;
		UI::WindowPtr _uiWindow;
		unsigned int _windowDpi = 72;
		VkFormat _requestedFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkColorSpaceKHR _requestedColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		
		glm::mat4 _modelToWorld;
		double _modelScale = 1.0;
		VkSurfaceKHR surface;
		std::mutex _swapChainMutex;
		size_t _changeNumber = 0, _lastChangeNumber = 0, _uiWindowChangeNumber = 0;
		uint32_t _swapChainIndex;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;

		DeviceContextPtr _deviceContext;
		VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		VkQueue presentQueue;
		SwapChain _swapChain;

		VkRenderPass renderPass;
		size_t _pipelineVertIdx = stm1, 
			_pipelineSamplerIdx = stm1,
			_pipelineUiIdx = stm1;
		ShaderPoolPtr _shaderPool;

		UniformBufferObject3D _ubo;
		PipelineRec _pipelines;

		std::vector<VkCommandBuffer> _commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		size_t currentFrame = 0;

		bool _framebufferResized = false;
	};

	inline VulkanAppPtr VulkanApp::getAppPtr() {
		return shared_from_this();
	}

	inline void VulkanApp::changed() {
		_changeNumber++;
	}

	inline const DeviceContextPtr& VulkanApp::getDeviceContext() const {
		return _deviceContext;
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

	inline void VulkanApp::setAntiAliasSamples(VkSampleCountFlagBits samples) {
		_msaaSamples = samples;
	}

	inline VkSampleCountFlagBits VulkanApp::getAntiAliasSamples() const {
		return _msaaSamples;
	}

	inline GLFWwindow* VulkanApp::getWindow() {
		return _window;
	}

	inline const UI::WindowPtr& VulkanApp::getUiWindow() const {
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

	inline uint32_t VulkanApp::getSwapChainIndex() const {
		return _swapChainIndex;
	}

	template<class PIPELINE_TYPE>
	inline PipelinePtr<PIPELINE_TYPE> VulkanApp::addPipelineWithSource(const std::string& shaderId, const std::string& vertShaderFilename, const std::string& fragShaderFilename) {
		auto pipeline = createPipelineWithSource<PIPELINE_TYPE>(getAppPtr(), shaderId, vertShaderFilename, fragShaderFilename);
		pipeline->setUniformBufferPtr(&_ubo);
		addPipeline(pipeline);
		return pipeline;
	}

	inline void VulkanApp::stop() {
		_isRunning = false;
	}

}