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

#include <memory>

#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>

namespace VK {

	class ComputeStep {
		// Uses a compute shader t process one image to another
	public:
		static ComputeStepPtr newPtr(const DeviceContextPtr& dc, const TextureImagePtr& srcImage, const TextureImagePtr& dstImage, const std::string& shaderId) {
			ComputeStepPtr p = std::shared_ptr<ComputeStep>(new ComputeStep(dc, srcImage, dstImage, shaderId));
			return p;
		}
		void submitCommands();

		const TextureImagePtr& getResultImage() const;

	private:
		ComputeStep(const DeviceContextPtr& dc, const TextureImagePtr& srcImage, const TextureImagePtr& dstImage, const std::string& shaderId);

		void build();
		void createComputeQueue();
		void createDescriptorPool();
		void createUniformBuffers();
		void buildComputeCommandBuffer();
		void createCompute();

		DeviceContextPtr _dc;

		TextureImagePtr _srcImage, _dstImage;
		std::string _shaderId;

		VkQueue _queue = VK_NULL_HANDLE;
		VkCommandBuffer _cmdBuf = VK_NULL_HANDLE;
		VkFence _fence = VK_NULL_HANDLE;

		VkCommandPool _commandPool = VK_NULL_HANDLE;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet _descriptorSet = VK_NULL_HANDLE;
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		VkPipeline _pipeline = VK_NULL_HANDLE;
		int32_t _pipelineIndex = 0;
		uint32_t _queueFamilyIndex = 0;
	};

	inline const TextureImagePtr& ComputeStep::getResultImage() const {
		return _dstImage;
	}
}