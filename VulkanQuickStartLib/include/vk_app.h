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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vk_exports.h>
#include <vk_forwardDeclarations.h>
#include "vk_deviceContext.h"
#include "vk_image.h"
#include <vk_sceneNode.h>
#include <vk_sceneNodePNC3f.h>
#include <vk_sceneNodePNCT3f.h>
#include <vk_pipelinePNC3f.h>
#include <vk_shaderPool.h>
#include <vk_swapChain.h>
#include <vk_offscreenPass.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS VulkanApp : public std::enable_shared_from_this<VulkanApp> {
	public:
		using BoundingBox = CBoundingBox3D<float>;
		using UboType = UniformBufferObject3D;
		using PipelineGroupType = PipelineUboGroup<UniformBufferObject3D>;
		using PipelineGroupTypePtr = PipelineUboGroupPtr<UniformBufferObject3D>;
		using PipelinePtr = PipelineGroupType::PipelinePtr;

		class Updater {
			// TODO
			// Very ugly workaround. After days of trying to get another thread to update the pipelines, I was not successful.
			// So, I'm using an old 'pass the batton' approach. The other thread set's a flag, the app calls a mediator which checks the flag,
			// if something needs to be done, it is done as a call from within mainloop.
		public:
			virtual void updateVkApp() = 0;
		};
		using UpdaterPtr = std::shared_ptr<Updater>;

		static VulkanAppPtr create(const VkRect2D& rect);

		void setClearColor(float red, float green, float blue, float alpha = 1.0f);

	protected:
		VulkanApp(const VkRect2D& rect);
		void init();

	public:
		~VulkanApp();

		VulkanAppPtr getAppPtr();

		void setUiWindow(const UI::WindowPtr& uiWindow );

		VkFormat findDepthFormat();

		void changed();
		size_t getRuntimeMillis() const;

		template<class PIPELINE_TYPE>
		VK::PipelinePtr<PIPELINE_TYPE> addPipelineWithSource(const std::string& shaderId, const std::vector<std::string>& filenames);
		size_t addOffscreen(const OffscreenPassBasePtr& osp);
		size_t addComputeStep(const ComputeStepBasePtr& step);
		size_t addPostDrawTask(const PostDrawTaskPtr& task);

		const DeviceContextPtr& getDeviceContext() const;

		const SwapChain& getSwapChain() const;
		SwapChain& getSwapChain();

		void setTargetFrameDurationMillis(double duration);
		void setAntiAliasSamples(VkSampleCountFlagBits samples);
		GLFWwindow* getWindow();
		const UI::WindowPtr& getUiWindow() const;
		unsigned int getWindowDpi() const;

		void setModelToWorldTransform(const glm::mat4& xform);
		const glm::mat4& getModelToWorldTransform() const;


		void setModelScale(double scale);
		double getModelScale() const;

		uint32_t getSwapChainIndex() const;

		void run();
		void stop();

		void setUpdater(const UpdaterPtr& updater);
		ImagePtr getOffscreenImage(size_t index) const;

		const VkRect2D& getFrameRect() const;

		VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		struct FormatPairRec {
			VkFormat _format;
			VkImageTiling _tiling;
		};
		std::vector<FormatPairRec> findSupportedFormats(const std::vector<VkFormat>& candidates, VkFormatFeatureFlags features);

		template<typename FUNC_TYPE>
		void setUboUpdateFunction(FUNC_TYPE f) {
			_uboUpdater = f;
		}

	private:
		struct SwapChainSupportDetails;
		struct QueueFamilyIndices;

		static std::vector<char> readFile(const std::string& filename);
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

		void initWindow();
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
		void createPipelines();
		void createImageViews();
		void createRenderPass();
		void createGraphicsPipeline();
		void createFramebuffers();
		void createCommandPool();
		void createColorResources();
		void createDepthResources();
		bool hasStencilComponent(VkFormat format);
		VkSampleCountFlagBits getMaxUsableSampleCount();

		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void createCommandBuffers();
		void drawCmdBufferLoop(VkCommandBuffer cmdBuff, size_t swapChainIndex, VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo renderPassInfo);
		void drawCmdBufferLoop(const OffscreenPassBasePtr& osp, VkCommandBuffer cmdBuff, VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo renderPassInfo);
		void createSyncObjects();
		void updateUniformBuffer(uint32_t swapChainImageIndex);
		void updateUBO(const VkExtent2D& extent, const BoundingBox& modelBounds, UboType& ubo) const;
		void reportFPS();
		void drawFrame();
		void submitGraphicsQueue();
		void submitComputeCommands();
		void presentQueueKHR();
		bool recreateSwapChainIfNeeded(VkResult result) ;
		void doPostDrawTasks() const;

		VkShaderModule createShaderModule(const std::vector<char>& code);
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D getWindowExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
		bool isDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures& features);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		std::vector<const char*> getRequiredExtensions();
		bool checkValidationLayerSupport();

		GLFWwindow* _window;
		VkRect2D _frameRect;
		bool _isRunning = true;
		size_t _runtimeMillis = 0;
		UI::WindowPtr _uiWindow;
		unsigned int _windowDpi = 72;
		VkFormat _requestedFormat = VK_FORMAT_B8G8R8A8_UNORM;
		VkColorSpaceKHR _requestedColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		UpdaterPtr _updater;
		UpdateUboFunctionType<UniformBufferObject3D> _uboUpdater;
		
		glm::mat4 _modelToWorld;
		double _modelScale = 1.0;
		VkSurfaceKHR _surface;
		size_t _changeNumber = 0, _lastChangeNumber = 0, _uiWindowChangeNumber = 0;
		uint32_t _swapChainIndex;

		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debugMessenger;

		DeviceContextPtr _deviceContext;

		VkClearColorValue _clearColor = { { 0, 0, 0, 1 } };
		VkClearDepthStencilValue _depthStencil = { 1, 0 };
		VkSampleCountFlagBits _maxMsaaSamples = VK_SAMPLE_COUNT_1_BIT;
		VkSampleCountFlagBits _msaaSamples = VK_SAMPLE_COUNT_1_BIT;

		VkQueue _presentQueue;
		SwapChain _swapChain;

		double _targetFrameDurationMillis = -1;
		std::vector<OffscreenPassBasePtr> _offscreenPasses;
		std::vector<PostDrawTaskPtr> _postDrawTasks;
		PipelineGroupTypePtr _pipelines;
		std::vector<ComputeStepBasePtr> _computeSteps;

		std::vector<VkCommandBuffer> _commandBuffers;

		bool _framebufferResized = false;
	};

	inline void VulkanApp::setClearColor(float red, float green, float blue, float alpha) {
		_clearColor = { {red, green, blue, alpha} };
	}

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

	inline void VulkanApp::setTargetFrameDurationMillis(double duration) {
		_targetFrameDurationMillis = duration;
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

	inline uint32_t VulkanApp::getSwapChainIndex() const {
		return _swapChainIndex;
	}

	inline void VulkanApp::setUpdater(const UpdaterPtr& updater) {
		_updater = updater;
	}

	template<class PIPELINE_TYPE>
	inline VK::PipelinePtr<PIPELINE_TYPE> VulkanApp::addPipelineWithSource(const std::string& shaderId, const std::vector<std::string>& filenames) {
		VK::PipelinePtr<PIPELINE_TYPE> pipeline = _pipelines->addPipelineWithSource<PIPELINE_TYPE>(shaderId, _frameRect, filenames);
		return pipeline;
	}

	inline void VulkanApp::stop() {
		_isRunning = false;
	}

	inline size_t VulkanApp::getRuntimeMillis() const {
		return _runtimeMillis;
	}


	inline ImagePtr VulkanApp::getOffscreenImage(size_t index) const {
		if (index < _offscreenPasses.size())
			return _offscreenPasses[index]->getColorImage();
		return nullptr;
	}

	inline const VkRect2D& VulkanApp::getFrameRect() const {
		return _frameRect;
	}

}

#pragma warning( pop )
