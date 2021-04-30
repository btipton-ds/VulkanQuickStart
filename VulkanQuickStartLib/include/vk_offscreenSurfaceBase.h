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

#include <functional>

#include <vk_forwardDeclarations.h>

#include <vulkan/vulkan_core.h>
#include <vk_pipelineUboGroup.h>
#include <vk_uniformBuffers.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	/*
	OffscreenSurface and OffscreenSurfaceBase are NOT surfaces. They are collection of images, frame buffers, renderers etc. that fulfill the role of a surface
	for offscreen rendering.
	*/
	class EXPORT_VQS OffscreenSurfaceBase {
	public:
		OffscreenSurfaceBase(const VulkanAppPtr& app, VkFormat colorFormat);
		virtual ~OffscreenSurfaceBase();

		void init(const VkExtent2D& extent, size_t numFrames = 1);
		void cleanup();

		const TextureImagePtr& getColorImage() const;
		VkFramebuffer getFrameBuffer() const;
		const VkRect2D& getRect() const;

		const VkClearColorValue& getClearColor() const;
		void setClearColor(float red, float green, float blue, float alpha = 1.0f);
		void setClearColor(const VkClearColorValue& color);

		const VkClearDepthStencilValue& getDepthStencil() const;
		void setDepthStencil(const VkClearDepthStencilValue&);

		const VkDescriptorImageInfo& getDescriptorInfo() const;

		virtual VkRenderPass getRenderPass() const = 0;
		virtual bool updateUbo() = 0;
		virtual void cleanupSwapChain() = 0;
		virtual void build() = 0;
		virtual void draw(VkCommandBuffer cmdBuff) = 0;

	protected:
		virtual VkSampleCountFlagBits getAntiAliasSamples() const = 0;
		virtual void setRenderPass(VkRenderPass renderPass) = 0;

		VkRect2D _rect = { { 0, 0 }, {0, 0 } };
		VkClearColorValue _clearColor = { { 0, 0, 0, 1 } };
		VkClearDepthStencilValue _depthStencil = { 1, 0 };
		DeviceContextPtr _deviceContext;
		VkFormat _colorFormat, _depthFormat;

		struct FrameBuffers {
			VkFramebuffer _frameBuffer = VK_NULL_HANDLE;
			TextureImagePtr _color = VK_NULL_HANDLE;
			ImagePtr _depth = VK_NULL_HANDLE;
			VkSampler _sampler = VK_NULL_HANDLE;
			VkDescriptorImageInfo _descriptor = {};
		};

		size_t _currentBufferIdx = 0;
		size_t _drawBufferIdx = 0;
		std::vector<FrameBuffers> _frameBuffers;
	};

	inline const TextureImagePtr& OffscreenSurfaceBase::getColorImage() const {
		return _frameBuffers[_currentBufferIdx]._color;
	}

	inline VkFramebuffer OffscreenSurfaceBase::getFrameBuffer() const {
		return _frameBuffers[_currentBufferIdx]._frameBuffer;
	}

	inline const VkRect2D& OffscreenSurfaceBase::getRect() const {
		return _rect;
	}

	inline const VkClearColorValue& OffscreenSurfaceBase::getClearColor() const {
		return _clearColor;
	}

	inline const VkClearDepthStencilValue& OffscreenSurfaceBase::getDepthStencil() const {
		return _depthStencil;
	}

	inline void OffscreenSurfaceBase::setClearColor(float red, float green, float blue, float alpha) {
		_clearColor = { { red, green, blue, alpha } };
	}

	inline void OffscreenSurfaceBase::setClearColor(const VkClearColorValue& color) {
		_clearColor = color;
	}

	inline void OffscreenSurfaceBase::setDepthStencil(const VkClearDepthStencilValue& value) {
		_depthStencil = value;
	}

	inline const VkDescriptorImageInfo& OffscreenSurfaceBase::getDescriptorInfo() const {
		return _frameBuffers[_currentBufferIdx]._descriptor;
	}
}

#pragma warning (pop)
