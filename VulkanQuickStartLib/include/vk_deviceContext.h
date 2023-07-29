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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <limits>
#include <set>
#include <memory>

#include <vk_forwardDeclarations.h>

namespace VK {

	struct DeviceContext {
		DeviceContext(size_t maxFramesInFlight);
		~DeviceContext();
		void destroy();

		void createSyncObjects();
		void waitForInFlightFence();
		VkSemaphore getImageAvailableSemaphore() const;
		VkSemaphore getRenderFinishedSemaphore() const;
		void submitGraphicsQueue(VkCommandBuffer cmdBuf);
		void nextFrame();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr);

		const ShaderPool& getShaderPool() const;
		ShaderPool& getShaderPool();

		VkDevice _device = VK_NULL_HANDLE;
		VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
		VkCommandPool _commandPool = VK_NULL_HANDLE;
		VkQueue _graphicsQueue = VK_NULL_HANDLE;
		VkPhysicalDeviceFeatures _features = {};
		VkPhysicalDeviceMemoryProperties _memoryProperties = {};

	private:
		size_t _maxFramesInFlight;
		std::vector<VkSemaphore> _imageAvailableSemaphores;
		std::vector<VkSemaphore> _renderFinishedSemaphores;
		std::vector<VkFence> _inFlightFences;
		size_t _currentFrame = 0;

		ShaderPoolPtr _shaderPool;
	};

	inline void DeviceContext::waitForInFlightFence() {
		vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	}

	inline VkSemaphore DeviceContext::getImageAvailableSemaphore() const {
		return _imageAvailableSemaphores[_currentFrame];
	}

	inline VkSemaphore DeviceContext::getRenderFinishedSemaphore() const {
		return _renderFinishedSemaphores[_currentFrame];
	}

	inline void DeviceContext::nextFrame() {
		_currentFrame = (_currentFrame + 1) % _maxFramesInFlight;
	}

	inline const ShaderPool& DeviceContext::getShaderPool() const {
		return *_shaderPool;
	}

	inline ShaderPool& DeviceContext::getShaderPool() {
		return *_shaderPool;
	}

}
