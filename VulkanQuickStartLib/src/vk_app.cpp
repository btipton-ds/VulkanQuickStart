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

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <set>
#include <unordered_map>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vk_logger.h>
#include "vk_deviceContext.h"
#include "vk_buffer.h"
#include "vk_image.h"
#include "vk_textureImage.h"
#include "vk_vertexTypes.h"
#include "vk_modelPNC3f.h"
#include <vk_modelPNCT3f.h>
#include <vk_sceneNode.h>
#include <vk_pipelinePNC3f.h>
#include <vk_pipelinePNCT3f.h>
#include <vk_pipelineUi.h>
#include <vk_pipelineUboGroup.h>
#include <vk_ui_window.h>
#include <vk_computeStep.h>
#include <vk_postDrawTask.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#define FORCE_VALIDATION 0

#if defined(VQS_RELEASE_BUILD) && !FORCE_VALIDATION
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

VulkanAppPtr VulkanApp::create(const VkRect2D& rect) {
	VulkanApp* ptr = new VulkanApp(rect);
	VulkanAppPtr result = shared_ptr<VulkanApp>(ptr);
	result->init();
	return result;
}

VulkanApp::VulkanApp(const VkRect2D& rect)
	: _frameRect(rect)
	, _deviceContext(make_shared<DeviceContext>(MAX_FRAMES_IN_FLIGHT))
	, _swapChain(_deviceContext)
{
	_modelToWorld = glm::identity<glm::mat4>();

}

void VulkanApp::init() {
	initWindow();
	initVulkan();
}

void VulkanApp::createPipelines() {
	_pipelines = make_shared<PipelineGroupType>(getAppPtr(), _swapChain._images.size());
	_pipelines->setAntiAliasSamples(_msaaSamples);
	if (_uiWindow)
		_uiWindow->getPipelines()->setAntiAliasSamples(_msaaSamples);
}

VulkanApp::~VulkanApp() {
	cleanup();
}

void VulkanApp::setUiWindow(const UI::WindowPtr& uiWindow) {
	if (uiWindow) {
		_uiWindow = uiWindow;
		_uiWindow->getPipelines()->setAntiAliasSamples(_msaaSamples);
	}
}

void VulkanApp::setAntiAliasSamples(VkSampleCountFlagBits samples) {
	if (samples < _maxMsaaSamples) {
		_msaaSamples = samples;
		_pipelines->setAntiAliasSamples(_msaaSamples);
		if (_uiWindow)
			_uiWindow->getPipelines()->setAntiAliasSamples(_msaaSamples);
		changed();
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

struct VulkanApp::QueueFamilyIndices {
	uint32_t graphicsFamily = 0xffffffff;
	uint32_t presentFamily = 0xffffffff;
	uint32_t computeFamily = 0xffffffff;

	bool isComplete() {
		return graphicsFamily != 0xffffffff && presentFamily != 0xffffffff && computeFamily != 0xffffffff;
	}
};

struct VulkanApp::SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

void VulkanApp::run() {
	mainLoop();
}

void VulkanApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	_window = glfwCreateWindow(_frameRect.extent.width, _frameRect.extent.height, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(_window, this);
	glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);

#ifdef _WIN32
	auto hwnd = glfwGetWin32Window(_window);
	_windowDpi = (unsigned int)GetDpiForWindow(hwnd);
#endif // _WIN32

}

void VulkanApp::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<VulkanApp*>(glfwGetWindowUserPointer(window));
	app->_frameRect.extent.width = width;
	app->_frameRect.extent.height = height;
	app->_framebufferResized = true;
}

void VulkanApp::initVulkan() {
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createPipelines();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createCommandPool();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createSyncObjects();
}

void VulkanApp::recreateSwapChain() {
	if (_uiWindow)
		_uiWindowChangeNumber = _uiWindow->getChangeNumber();
	_lastChangeNumber = _changeNumber;
	_framebufferResized = false;
	_pipelines->resized(_frameRect);

	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(_window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(_deviceContext->_device);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void VulkanApp::mainLoop() {
	static chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
	if (_targetFrameDurationMillis > 0) {
		while (!glfwWindowShouldClose(_window) && _isRunning) {
			chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			_runtimeMillis = chrono::duration_cast<std::chrono::milliseconds>(start - t0).count();

			glfwPollEvents();
			drawFrame();
			if (_updater)
				_updater->updateVkApp();
			chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


			int64_t timeMs = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			int64_t target = (int64_t)_targetFrameDurationMillis;

			if (target > timeMs) {
				std::this_thread::sleep_for(chrono::milliseconds(target - timeMs));
			}

		}
	}
	else {
		while (!glfwWindowShouldClose(_window) && _isRunning) {
			chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
			_runtimeMillis = chrono::duration_cast<std::chrono::milliseconds>(start - t0).count();

			glfwPollEvents();
			drawFrame();
			if (_updater)
				_updater->updateVkApp();
		}
	}

	vkDeviceWaitIdle(_deviceContext->_device);
}

void VulkanApp::cleanupSwapChain() {
	for (auto framebuffer : _swapChain._vkFrameBuffers) {
		vkDestroyFramebuffer(_deviceContext->_device, framebuffer, nullptr);
}

	_pipelines->iterate([](const PipelinePtr& pl) {
		pl->cleanupSwapChain();
	});

	for (const auto& osp : _offscreenPasses) {
		osp->cleanupSwapChain();
	}

	if (_uiWindow)
		_uiWindow->getPipelines()->cleanupSwapChain();

	if (!_commandBuffers.empty())
		vkFreeCommandBuffers(_deviceContext->_device, _deviceContext->_commandPool, static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());

	for (auto imageView : _swapChain._vkImageViews) {
		vkDestroyImageView(_deviceContext->_device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(_deviceContext->_device, _swapChain._vkSwapChain, nullptr);
}

size_t VulkanApp::addOffscreen(const OffscreenPassBasePtr& osp) {
	size_t result = _offscreenPasses.size();
	_offscreenPasses.push_back(osp);
	changed();
	return result;
}

size_t VulkanApp::addComputeStep(const ComputeStepBasePtr& step) {
	size_t result = _computeSteps.size();
	_computeSteps.push_back(step);
	changed();
	return result;
}

size_t VulkanApp::addPostDrawTask(const PostDrawTaskPtr& task) {
	size_t result = _postDrawTasks.size();
	_postDrawTasks.push_back(task);
	changed();
	return result;
}

void VulkanApp::cleanup() {
	for (const auto& osp : _offscreenPasses)
		osp->cleanup();

	cleanupSwapChain();

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(_instance, _debugMessenger, nullptr);
	}

	_deviceContext->destroy();

	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);

	glfwDestroyWindow(_window);

	glfwTerminate();
}

void VulkanApp::createInstance() {
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		THROW("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS) {
		THROW("failed to create instance!");
	}
}

void VulkanApp::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

void VulkanApp::setupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS) {
		THROW("failed to set up debug messenger!");
	}
}

void VulkanApp::createSurface() {
	if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
		THROW("failed to create window surface!");
	}
}

void VulkanApp::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		THROW("failed to find GPUs with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		VkPhysicalDeviceFeatures features = {};
		if (isDeviceSuitable(device, features)) {
			_deviceContext->_physicalDevice = device;
			_deviceContext->_features = features;
			_maxMsaaSamples = getMaxUsableSampleCount();
			break;
		}
	}

	if (_deviceContext->_physicalDevice == VK_NULL_HANDLE) {
		THROW("failed to find a suitable GPU!");
	}
}

void VulkanApp::createLogicalDevice() {
	// TODO, move this bulk of this method into DeviceContext as appropriate.
	QueueFamilyIndices indices = findQueueFamilies(_deviceContext->_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily, indices.computeFamily };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

// do this to turn on everything	VkPhysicalDeviceFeatures deviceFeatures = _deviceContext->_features;
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.fillModeNonSolid = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_deviceContext->_physicalDevice, &createInfo, nullptr, &_deviceContext->_device) != VK_SUCCESS) {
		THROW("failed to create logical device!");
	}

	vkGetDeviceQueue(_deviceContext->_device, indices.graphicsFamily, 0, &_deviceContext->_graphicsQueue);
	vkGetDeviceQueue(_deviceContext->_device, indices.presentFamily, 0, &_presentQueue);

	vkGetPhysicalDeviceMemoryProperties(_deviceContext->_physicalDevice, &_deviceContext->_memoryProperties);
}

void VulkanApp::createSwapChain() {
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_deviceContext->_physicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = getWindowExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = _surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT; // VK_IMAGE_USAGE_TRANSFER_SRC_BIT is required to read back the image.

	QueueFamilyIndices indices = findQueueFamilies(_deviceContext->_physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	if (vkCreateSwapchainKHR(_deviceContext->_device, &createInfo, nullptr, &_swapChain._vkSwapChain) != VK_SUCCESS) {
		THROW("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(_deviceContext->_device, _swapChain._vkSwapChain, &imageCount, nullptr);
	_swapChain._vkImages.resize(imageCount);
	vkGetSwapchainImagesKHR(_deviceContext->_device, _swapChain._vkSwapChain, &imageCount, _swapChain._vkImages.data());

	_swapChain._images.clear();
	_swapChain._images.reserve(imageCount);
	for (auto vkImage : _swapChain._vkImages) {
		_swapChain._images.push_back(make_shared<Image>(_deviceContext, createInfo, vkImage));
	}
	_swapChain._imageFormat = surfaceFormat.format;
	_swapChain._extent = extent;
}

void VulkanApp::createImageViews() {
	_swapChain._vkImageViews.resize(_swapChain._vkImages.size());

	for (uint32_t i = 0; i < _swapChain._vkImages.size(); i++) {
		_swapChain._vkImageViews[i] = Image::createImageView(_deviceContext, _swapChain._vkImages[i], 
			_swapChain._imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void VulkanApp::createRenderPass() {
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = _swapChain._imageFormat;
	colorAttachment.samples = _msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = _msaaSamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve = {};
	colorAttachmentResolve.format = _swapChain._imageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef = {};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	subpass.pResolveAttachments = (_msaaSamples == VK_SAMPLE_COUNT_1_BIT) ? nullptr : &colorAttachmentResolveRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkRenderPass renderPass;
	if (vkCreateRenderPass(_deviceContext->_device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		THROW("failed to create render pass!");
	}
	_pipelines->setRenderPass(renderPass);
	if (_uiWindow)
		_uiWindow->getPipelines()->setRenderPass(renderPass);
}

void VulkanApp::createGraphicsPipeline() {
	_pipelines->iterate([](const PipelinePtr& pl) {
		if (pl->isVisible())
			pl->build();
	});

	for (const auto& osp : _offscreenPasses) {
		osp->build();
	}

	if (_uiWindow)
		_uiWindow->getPipelines()->iterate([](const UI::Window::PipelinePtr& pl) {
		if (pl->isVisible())
			pl->build();
	});
}

void VulkanApp::createFramebuffers() {
	_swapChain._vkFrameBuffers.resize(_swapChain._vkImageViews.size());

	for (size_t i = 0; i < _swapChain._vkImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			_swapChain._colorImage.getImageView(),
			_swapChain._depthImage.getImageView(),
			_swapChain._vkImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = _pipelines->getRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = _swapChain._extent.width;
		framebufferInfo.height = _swapChain._extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(_deviceContext->_device, &framebufferInfo, nullptr, &_swapChain._vkFrameBuffers[i]) != VK_SUCCESS) {
			THROW("failed to create framebuffer!");
		}
	}
}

void VulkanApp::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(_deviceContext->_physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

	if (vkCreateCommandPool(_deviceContext->_device, &poolInfo, nullptr, &_deviceContext->_commandPool) != VK_SUCCESS) {
		THROW("failed to create graphics command pool!");
	}
}

void VulkanApp::createColorResources() {
	VkFormat colorFormat = _swapChain._imageFormat;
	VkImageUsageFlags flagBits = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	_swapChain._colorImage.create(colorFormat, flagBits, _swapChain._extent.width, _swapChain._extent.height, _msaaSamples);
}

void VulkanApp::createDepthResources() {
	VkFormat depthFormat = findDepthFormat();
	VkImageUsageFlags flagBits = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	_swapChain._depthImage.create(depthFormat, flagBits, _swapChain._extent.width, _swapChain._extent.height, _msaaSamples);
}

VkFormat VulkanApp::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
	auto pDevice = _deviceContext->_physicalDevice;
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(pDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	THROW("failed to find supported format!");
}

vector<VulkanApp::FormatPairRec> VulkanApp::findSupportedFormats(const vector<VkFormat>& candidates, VkFormatFeatureFlags features) {
	auto pDevice = _deviceContext->_physicalDevice;
	vector<FormatPairRec> result;
	for (VkFormat format : candidates) {
		FormatPairRec pair;
		pair._format = format;
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(pDevice, format, &props);

		if ((props.linearTilingFeatures & features) == features) {
			pair._tiling = VK_IMAGE_TILING_LINEAR;
			result.push_back(pair);
		}
		else if ((props.optimalTilingFeatures & features) == features) {
			pair._tiling = VK_IMAGE_TILING_OPTIMAL;
			result.push_back(pair);
		}
	}
	return result;
}

VkFormat VulkanApp::findDepthFormat() {
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool VulkanApp::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkSampleCountFlagBits VulkanApp::getMaxUsableSampleCount() {
	VkPhysicalDeviceProperties physicalDeviceProperties;
	vkGetPhysicalDeviceProperties(_deviceContext->_physicalDevice, &physicalDeviceProperties);

	VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

void VulkanApp::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_deviceContext->_device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		THROW("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_deviceContext->_device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(_deviceContext->_device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		THROW("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(_deviceContext->_device, buffer, bufferMemory, 0);
}

uint32_t VulkanApp::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_deviceContext->_physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	THROW("failed to find suitable memory type!");
}

void VulkanApp::createCommandBuffers() {
	_commandBuffers.resize(_swapChain._vkFrameBuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = _deviceContext->_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

	if (vkAllocateCommandBuffers(_deviceContext->_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
		THROW("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	for (size_t swapChainIndex = 0; swapChainIndex < _commandBuffers.size(); swapChainIndex++) {
		VkCommandBuffer& cmdBuff = _commandBuffers[swapChainIndex];

		if (vkBeginCommandBuffer(cmdBuff, &beginInfo) != VK_SUCCESS) {
			THROW("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = _clearColor;
		clearValues[1].depthStencil = _depthStencil;
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		for (const OffscreenPassBasePtr& osp : _offscreenPasses) {
			clearValues[0].color = osp->getClearColor();
			clearValues[1].depthStencil = osp->getDepthStencil();;
			drawCmdBufferLoop(osp, cmdBuff, beginInfo, renderPassInfo);
		}

		clearValues[0].color = _clearColor;
		clearValues[1].depthStencil = _depthStencil;
		drawCmdBufferLoop(cmdBuff, swapChainIndex, beginInfo, renderPassInfo);

		if (vkEndCommandBuffer(cmdBuff) != VK_SUCCESS) {
			THROW("failed to record command buffer!");
		}
	}

}

void VulkanApp::drawCmdBufferLoop(VkCommandBuffer cmdBuff, size_t swapChainIndex, VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo renderPassInfo) {
	renderPassInfo.renderPass = _pipelines->getRenderPass();
	renderPassInfo.framebuffer = _swapChain._vkFrameBuffers[swapChainIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = _swapChain._extent;

	vkCmdBeginRenderPass(cmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	_pipelines->iterate([&](const PipelinePtr& pipeline) {
		if (pipeline->isVisible())
			pipeline->draw(cmdBuff, swapChainIndex);
	});

	if (_uiWindow)
		_uiWindow->getPipelines()->iterate([&](const UI::Window::PipelinePtr& pipeline) {
		if (pipeline->isVisible())
			pipeline->draw(cmdBuff, swapChainIndex);
	});

	vkCmdEndRenderPass(cmdBuff);
}

void VulkanApp::drawCmdBufferLoop(const OffscreenPassBasePtr& osp, VkCommandBuffer cmdBuff, VkCommandBufferBeginInfo& beginInfo, VkRenderPassBeginInfo renderPassInfo) {
	renderPassInfo.renderPass = osp->getRenderPass();
	renderPassInfo.framebuffer = osp->getFrameBuffer();
	renderPassInfo.renderArea.extent = osp->getRect().extent;

	vkCmdBeginRenderPass(cmdBuff, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	osp->draw(cmdBuff);

	vkCmdEndRenderPass(cmdBuff);
}

void VulkanApp::createSyncObjects() {
	_deviceContext->createSyncObjects();
}

void VulkanApp::reportFPS() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	static size_t count = 0;
	static float lastReportTime = 0;
	count++;
	if (time - lastReportTime > 2.5f) {
		float deltaT = time - lastReportTime;
		lastReportTime = time;
		std::cout << "FPS: " << (count / deltaT) << ", time/frame: " << (1000.0f * deltaT / (float)count) << " ms\n";
		count = 0;
	}
}

namespace {
	inline glm::vec3 conv(const Vector3f& pt) {
		return glm::vec3(pt[0], pt[1], pt[2]);
	}

}

void VulkanApp::updateUBO(const VkExtent2D& extent, const BoundingBox& modelBounds, UboType& ubo) const {

	ubo = {};
	ubo.ambient = 0.10f;
	ubo.numLights = 2;
	ubo.lightDir[0] = glm::normalize(glm::vec3(1, -0.5, 1));
	ubo.lightDir[1] = glm::normalize(glm::vec3(-1, -0.5, 3));

	float w = (float)extent.width;
	float h = (float)extent.height;
	float maxDim = std::max(w, h);
	w /= maxDim;
	h /= maxDim;

	auto range = modelBounds.range();
	float maxModelDim = max(max(range[0], range[1]), range[2]);
	float scale = 2.0f * 1.0f / maxModelDim;
	scale *= (float)getModelScale();

	auto ctr = (modelBounds.getMin() + modelBounds.getMax()) / 2;
	glm::mat4 model = getModelToWorldTransform();
	model *= glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
	model *= glm::translate(glm::mat4(1.0f), -conv(ctr));

	glm::mat4 view = glm::lookAt(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.modelView = view * model;

	//		auto proj = glm::perspective(glm::radians(45.0f), _extent.width / (float)_extent.height, 0.1f, 10.0f);
	ubo.proj = glm::ortho(-w, w, -h, h, -10.0f, 10.0f);
	ubo.proj[1][1] *= -1;
}

void VulkanApp::updateUniformBuffer(uint32_t swapChainImageIndex) {
	reportFPS();

	BoundingBox modelBounds;

	_pipelines->iterate([&](const PipelinePtr& pipeline) {
		auto ptr3D = dynamic_pointer_cast<PipelinePNC3f>(pipeline);
		if (ptr3D)
			modelBounds.merge(ptr3D->getBounds());

		auto ptr3DWTex = dynamic_pointer_cast<PipelinePNCT3f>(pipeline);
		if (ptr3DWTex)
			modelBounds.merge(ptr3DWTex->getBounds());
	});

	UniformBufferObject3D ubo;
	if (!_uboUpdater || !_uboUpdater(_swapChain._extent.width, _swapChain._extent.height, ubo)) {
		updateUBO(_swapChain._extent, modelBounds, ubo);
	}
	_pipelines->setUbo(ubo, swapChainImageIndex);

	if (_uiWindow) {
		_uiWindow->updateUniformBuffer(swapChainImageIndex);
	}

	for (const OffscreenPassBasePtr& osp : _offscreenPasses) {
		if (!osp->updateUbo()) {
			const auto& rect = osp->getRect();
			if (!_uboUpdater || !_uboUpdater(rect.extent.width, rect.extent.height, ubo)) {
				updateUBO(rect.extent, modelBounds, ubo);
			}
			OffscreenPass3D::PointerType ospSpecific = dynamic_pointer_cast<OffscreenPass3D>(osp);
			if (ospSpecific)
				ospSpecific->setUbo(ubo);
		}
	}

	for (const auto& step : _computeSteps) {
		step->updateUbo();
	}
}

inline bool VulkanApp::recreateSwapChainIfNeeded(VkResult result) {
	// TODO, per scene node change numbers.
	bool needToRecreate = false;

	if (_uiWindow)
		needToRecreate = _uiWindow->getChangeNumber() != _uiWindowChangeNumber || needToRecreate;

	needToRecreate = (result == VK_ERROR_OUT_OF_DATE_KHR || _changeNumber > _lastChangeNumber) || _framebufferResized;

	if (needToRecreate) {
		recreateSwapChain();
	}
	return needToRecreate;
}

void VulkanApp::drawFrame() {
	// TODO Cache this state on _pipelines so we stop doing the loop
	size_t numSceneNodes = 0;
	_pipelines->iterate([&](const PipelinePtr& pipeline) {
		// TODO, add update buffers call here.
		numSceneNodes += pipeline->numSceneNodes();
	});

	if (numSceneNodes <= 0)
		return;

	_deviceContext->waitForInFlightFence();

	VkSemaphore semaphore = _deviceContext->getImageAvailableSemaphore();
	VkResult result = vkAcquireNextImageKHR(_deviceContext->_device, _swapChain._vkSwapChain, std::numeric_limits<uint64_t>::max(),
		semaphore, VK_NULL_HANDLE, &_swapChainIndex);

	if (recreateSwapChainIfNeeded(result)) {
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		THROW("failed to acquire swap chain image!");
	}

	updateUniformBuffer(_swapChainIndex);
	submitGraphicsQueue();
	submitComputeCommands();
	presentQueueKHR();

	doPostDrawTasks();

	_deviceContext->nextFrame();
}

void VulkanApp::submitGraphicsQueue() {
	_deviceContext->submitGraphicsQueue(_commandBuffers[_swapChainIndex]);
}

void VulkanApp::presentQueueKHR() {
	VkSemaphore semaphore = _deviceContext->getImageAvailableSemaphore();
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { semaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_commandBuffers[_swapChainIndex];

	VkSemaphore signalSemaphores = _deviceContext->getRenderFinishedSemaphore();
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphores;

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &signalSemaphores;

	VkSwapchainKHR swapChains[] = { _swapChain._vkSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &_swapChainIndex;

	VkResult result = vkQueuePresentKHR(_presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _changeNumber > _lastChangeNumber) {
		_lastChangeNumber = _changeNumber;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		THROW("failed to present swap chain image!");
	}
}

void VulkanApp::doPostDrawTasks() const {
	for (const auto& task : _postDrawTasks)
		task->doPostDrawTask(); // Has an overly specific name because it may be used as a mix in base class
}

void VulkanApp::submitComputeCommands() {
	for (const auto& step : _computeSteps)
		step->submitCommands();
}

VkShaderModule VulkanApp::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(_deviceContext->_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		THROW("failed to create shader module!");
	}

	return shaderModule;
}

VkSurfaceFormatKHR VulkanApp::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == _requestedFormat && availableFormat.colorSpace == _requestedColorSpace) {
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR VulkanApp::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

VkExtent2D VulkanApp::getWindowExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
		throw runtime_error("capabilities.currentExtent.width == FLT_MAX");
	}
	else {
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VulkanApp::SwapChainSupportDetails VulkanApp::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool VulkanApp::isDeviceSuitable(VkPhysicalDevice device, VkPhysicalDeviceFeatures& features) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	vkGetPhysicalDeviceFeatures(device, &features);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && features.samplerAnisotropy;
}

bool VulkanApp::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

VulkanApp::QueueFamilyIndices VulkanApp::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}

		if (queueFamily.queueCount > 0 && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
		{
			indices.computeFamily = i;
		}


		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

std::vector<const char*> VulkanApp::getRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool VulkanApp::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<char> VulkanApp::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		THROW("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApp::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

	// Format the message
	string tmp, msg;
	msg = pCallbackData->pMessage;

	auto pos = msg.find("]");
	if (pos > -1) {
		tmp = msg.substr(0, pos + 2);
		msg = msg.substr(tmp.length());
		std::cerr << "validation layer message: " << tmp << std::endl;

		tmp = msg.substr(0, msg.find("|"));
		msg = msg.substr(tmp.length() + 2);
		std::cerr << "  " << tmp << std::endl;

		tmp = "| " + msg.substr(0, msg.find("|"));
		msg = msg.substr(tmp.length() + 2);
		std::cerr << "  " << tmp << std::endl;

		tmp = "| " + msg;
		std::cerr << "  " << tmp << std::endl;
	} else {
		std::cerr << "  " << msg << std::endl;
	}
	return VK_FALSE;
}
