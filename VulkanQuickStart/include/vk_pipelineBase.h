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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan_core.h>

#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>
#include <vk_pipelineSceneNode.h>

namespace VK {

	class PipelineBase {
	public:
		static void addShaders(const VulkanAppPtr& app, const std::string& shaderId, const std::vector<std::string>& filenames);

		PipelineBase(const VulkanAppPtr& app);
		virtual ~PipelineBase();

		void setCullMode(VkCullModeFlagBits cullMode);
		void setPolygonMode(VkPolygonMode polygonMode);
		void setLineWidth(double width);

		virtual void cleanupSwapChain();
		void draw(VkCommandBuffer cmdBuff, size_t swapChainIndex);

		void setViewportRect(const VkRect2D& rect);
		void setScissorRect(const VkRect2D& rect);
		void build();

		const VulkanAppPtr& getApp() const;

		virtual size_t numSceneNodes() const;
		virtual void updateUniformBuffers(size_t swapChainIndex) = 0;

		virtual void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const = 0;
		virtual size_t getUboSize() const = 0;

		VkDescriptorSetLayout getDescriptorSetLayout() const;

	protected:
		virtual void createDescriptorSetLayout() = 0;
		virtual void buildSceneNodes() = 0;
		virtual std::string getShaderIdMethod() = 0;

		VulkanAppPtr _app;

		VkVertexInputBindingDescription _vertBindDesc;
		std::vector<VkVertexInputAttributeDescription> _vertAttribDesc;
		VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;

	private:
		void createPipelineLayout();

		void setShaderStages(std::vector<VkPipelineShaderStageCreateInfo>& shaderStages);
		void setVertexInputInfo(VkPipelineVertexInputStateCreateInfo& vertexInputInfo);
		void setInputAssembly(VkPipelineInputAssemblyStateCreateInfo& inputAssembly);
		void setViewport(VkViewport& viewPort);
		void setViewportState(VkPipelineViewportStateCreateInfo& viewportState, VkViewport* viewportPtr);
		void setRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer);
		void setMultisampling(VkPipelineMultisampleStateCreateInfo& multisampling);
		void setDepthStencil(VkPipelineDepthStencilStateCreateInfo& depthStencil);
		void setColorBlendAttachment(VkPipelineColorBlendAttachmentState& colorBlendAttachment);
		void setColorBlending(VkPipelineColorBlendStateCreateInfo& colorBlending, VkPipelineColorBlendAttachmentState* colorBlendAttachmentPtr);

		VkCullModeFlagBits _cullMode = VK_CULL_MODE_NONE;
		VkPolygonMode _polygonMode = VK_POLYGON_MODE_FILL;
		float _lineWidth = 1.0f;
		VkRect2D _viewportRect, _scissorRect;

	protected:
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		VkPipeline _graphicsPipeline = VK_NULL_HANDLE;
	};

	inline const VulkanAppPtr& PipelineBase::getApp() const {
		return _app;
	}

	inline VkDescriptorSetLayout PipelineBase::getDescriptorSetLayout() const {
		return _descriptorSetLayout;
	}

	inline void PipelineBase::setCullMode(VkCullModeFlagBits cullMode) {
		_cullMode = cullMode;
	}

	inline void PipelineBase::setPolygonMode(VkPolygonMode polygonMode) {
		_polygonMode = polygonMode;
	}

	inline void PipelineBase::setLineWidth(double width) {
		_lineWidth = (float)width;
	}

	inline void PipelineBase::setViewportRect(const VkRect2D& rect) {
		_viewportRect = rect;
	}

	inline void PipelineBase::setScissorRect(const VkRect2D& rect) {
		_scissorRect = rect;
	}

}
