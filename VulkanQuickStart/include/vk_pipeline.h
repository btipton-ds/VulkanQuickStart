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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan_core.h>
#include <vk_buffer.h>
#include <vk_sceneNode.h>

namespace VK {

	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<class VulkanApp>;

	class Pipeline;
	using PipelinePtr = std::shared_ptr<Pipeline>;

	class Pipeline {
	public:
		template<class PIPELINE_TYPE>
		static PipelinePtr createWithSource(VulkanApp* app, const std::string& vertShaderFilename, const std::string& fragShaderFilename);

		template<class PIPELINE_TYPE>
		static PipelinePtr create(VulkanApp* app);

		Pipeline(VulkanApp* app);
		virtual ~Pipeline();

		void setCullMode(VkCullModeFlagBits cullMode);
		void setPolygonMode(VkPolygonMode polygonMode);
		void setLineWidth(double width);

		void addSceneNode(const SceneNodePtr& node);

		void cleanupSwapChain();

		void setViewportRect(const VkRect2D& rect);
		void setScissorRect(const VkRect2D& rect);
		void build();

		VkPipeline getVKPipeline() const;

		virtual void updateUniformBuffer(size_t swapChainIndex) = 0;

		template<class BUF_TYPE>
		void updateUniformBuffer(size_t swapChainIndex, const BUF_TYPE& ubo);

		void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const;
		size_t numSceneNodes() const;

	protected:
		void createDescriptorPool();
		virtual void createDescriptorSetLayout() = 0;
		virtual void createDescriptorSets() = 0;
		virtual void createUniformBuffers() = 0;
		virtual std::string getShaderIdMethod() = 0;

		VulkanApp* _app;
		SceneNodeList _sceneNodes;

		VkVertexInputBindingDescription _vertBindDesc;
		std::vector<VkVertexInputAttributeDescription> _vertAttribDesc;
		std::vector<Buffer> _uniformBuffers;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorSetLayout;
		std::vector<VkDescriptorSet> _descriptorSets;

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

		VkPipelineLayout _pipelineLayout;
		VkPipeline _graphicsPipeline;
	};

	inline void Pipeline::setCullMode(VkCullModeFlagBits cullMode) {
		_cullMode = cullMode;
	}

	inline void Pipeline::setPolygonMode(VkPolygonMode polygonMode) {
		_polygonMode = polygonMode;
	}

	inline void Pipeline::setLineWidth(double width) {
		_lineWidth = (float)width;
	}

	inline void Pipeline::setViewportRect(const VkRect2D& rect) {
		_viewportRect = rect;
	}

	inline void Pipeline::setScissorRect(const VkRect2D& rect) {
		_scissorRect = rect;
	}

	inline void Pipeline::addSceneNode(const SceneNodePtr& node) {
		_sceneNodes.push_back(node);
	}

	inline VkPipeline Pipeline::getVKPipeline() const {
		return _graphicsPipeline;
	}

	inline size_t Pipeline::numSceneNodes() const {
		return _sceneNodes.size();
	}

	template<class PIPELINE_TYPE>
	inline PipelinePtr Pipeline::createWithSource(VulkanApp* app, const std::string& vertShaderFilename, const std::string& fragShaderFilename) {
		auto& shaders = app->getShaderPool();
		const std::string shaderId = PIPELINE_TYPE::getShaderId();
		if (!shaders.getShader(shaderId))
			shaders.addShader(shaderId, { vertShaderFilename , fragShaderFilename });
		return create<PIPELINE_TYPE>(app);
	}

	template<class PIPELINE_TYPE>
	inline PipelinePtr Pipeline::create(VulkanApp* app) {
		PIPELINE_TYPE* ptr = new PIPELINE_TYPE(app);
		return std::shared_ptr<Pipeline>(ptr);
	}

	template<class BUF_TYPE>
	inline void Pipeline::updateUniformBuffer(size_t swapChainIndex, const BUF_TYPE& ubo) {
		_uniformBuffers[swapChainIndex].update(ubo);
	}


}
