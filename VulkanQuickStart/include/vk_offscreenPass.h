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

#include <vk_forwardDeclarations.h>

#include <vulkan/vulkan_core.h>
#include <vk_pipelineUboGroup.h>
#include <vk_uniformBuffers.h>

namespace VK {

	class OffscreenPass {
	public:
		using UboType = UniformBufferObject3D;
		using PipelineGroupType = PipelineUboGroup<UboType>;

		OffscreenPass(const DeviceContextPtr& deviceContext, VkFormat colorFormat, VkFormat depthFormat);
		~OffscreenPass();

		void init(const VkExtent2D& extent);
		void cleanup();

		const ImagePtr& getColorImage() const;
		VkRenderPass getRenderPass() const;
		VkFramebuffer getFrameBuffer() const;
		const VkExtent2D& getExtent() const;

		const PipelineGroupType& getPipelines() const;
		PipelineGroupType& getPipelines();

		void setUbo(const UboType& ubo);

	private:
		VkExtent2D _extent = { 0, 0 };
		VkFramebuffer _frameBuffer = VK_NULL_HANDLE;
		ImagePtr _color = VK_NULL_HANDLE, _depth = VK_NULL_HANDLE;
		VkRenderPass _renderPass = VK_NULL_HANDLE;
		VkSampler _sampler = VK_NULL_HANDLE;
		VkDescriptorImageInfo _descriptor{};
		DeviceContextPtr _deviceContext;
		VkFormat _colorFormat, _depthFormat;

		UboType _ubo;
		PipelineGroupType _pipelines;
	};

	inline const ImagePtr& OffscreenPass::getColorImage() const {
		return _color;
	}

	inline VkRenderPass OffscreenPass::getRenderPass() const {
		return _renderPass;
	}

	inline VkFramebuffer OffscreenPass::getFrameBuffer() const {
		return _frameBuffer;
	}

	inline const VkExtent2D& OffscreenPass::getExtent() const {
		return _extent;
	}

	inline const typename OffscreenPass::PipelineGroupType& OffscreenPass::getPipelines() const {
		return _pipelines;
	}

	inline typename OffscreenPass::PipelineGroupType& OffscreenPass::getPipelines() {
		return _pipelines;
	}

}
