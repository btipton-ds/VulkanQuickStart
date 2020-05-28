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

#include <vk_pipeline.h>

#include <stdexcept>
#include <string>
#include <fstream>
#include <array>

#include <vk_deviceContext.h>
#include <vk_buffer.h>
#include <vk_vertexTypes.h>
#include <vk_app.h>

namespace VK {

	using namespace std;

	PipelineBase::PipelineBase(VulkanApp* app)
		: _app(app)
	{
		_viewportRect.offset = { 0,0 };
		_viewportRect.extent = _app->getSwapChain().swapChainExtent;
		_scissorRect = _viewportRect;
	}

	PipelineBase::~PipelineBase() {
		cleanupSwapChain();
	}

	void PipelineBase::cleanupSwapChain() {
		if (_uniformBuffers.empty())
			return;

		_uniformBuffers.clear();
		_descriptorSets.clear();

		auto devCon = _app->getDeviceContext().device_;
		vkDestroyDescriptorSetLayout(devCon, _descriptorSetLayout, nullptr);
		vkDestroyPipeline(devCon, _graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(devCon, _pipelineLayout, nullptr);
		vkDestroyDescriptorPool(devCon, _descriptorPool, nullptr);
	}

	void PipelineBase::build() {
		if (_sceneNodes.empty())
			return;

		createDescriptorPool();
		createUniformBuffers();
		createDescriptorSetLayout();
		createDescriptorSets();

		/*
			Caution!
			The following sequence uses pointers to temporary storage. If you declare viewPort or scissorRect (for example) in the
			setViewPortState method, their storage will roll up on the stack on function exit.

			The original code was a long, continuous stream and difficult for me to read.

			This is a reasonable compromise.

			All the storage structures are declared in this stack frame, or stored on the heap.
			The code which populates the members is broken out so that it's possible to see the code flow.

			It may be desireable to create composite structs such as 

			myViewportState {
				VkViewport						  viewport;
				VkRect2D						  scissorRect;
				VkPipelineViewportStateCreateInfo viewportState;
			}

			so that dependent sub structs are colocated with their owner so that stack coherance is guaranteed.

			I leave that decision to you.
		*/

		/*
		Note: 
		I chose to use the older style 'declare first, use after' pattern so the code flow is more visible.

		Note: It's tempting to make this a class, possibly a singleton, just to keep the code organized. If all the methods are inlined and the class is on the stack, 
		there will be no performance hit.
		However! There're a lot cross linking pointers and dependencies. Making this it's own class requires a bit too much untangling for now.
		*/

		vector<VkPipelineShaderStageCreateInfo> shaderStages;
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssembly;
		VkViewport viewport;
		VkPipelineViewportStateCreateInfo viewportState;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineDepthStencilStateCreateInfo depthStencil;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlending;

		setShaderStages(shaderStages);
		setVertexInputInfo(vertexInputInfo);
		setInputAssembly(inputAssembly);
		setViewport(viewport);
		setViewportState(viewportState, &viewport);
		setRasterizer(rasterizer);
		setMultisampling(multisampling);
		setDepthStencil(depthStencil);
		setColorBlendAttachment(colorBlendAttachment);
		setColorBlending(colorBlending, &colorBlendAttachment);

		createPipelineLayout();

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = _app->getRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		auto device = _app->getDeviceContext().device_;

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}
	}

	inline void PipelineBase::setShaderStages(vector<VkPipelineShaderStageCreateInfo>& shaderStages) {
		auto& shader = _app->getShaderPool().getShader(getShaderIdMethod());
		for (size_t i = 0; i < shader->_shaderModules.size(); i++) {
			const auto& shaderModule = shader->_shaderModules[i];
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = (i == 0) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT; // TODO, add a method which sets the shader type in the pool
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main"; // This is the name of the shader entry point. NOT a user provided name of the shader.
			shaderStages.push_back(shaderStageInfo);
		}
	}

	inline void PipelineBase::setVertexInputInfo(VkPipelineVertexInputStateCreateInfo& vertexInputInfo) {
		vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertAttribDesc.size());
		vertexInputInfo.pVertexBindingDescriptions = &_vertBindDesc;
		vertexInputInfo.pVertexAttributeDescriptions = _vertAttribDesc.data();
	}

	inline void PipelineBase::setInputAssembly(VkPipelineInputAssemblyStateCreateInfo& inputAssembly) {
		inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	}

	inline void PipelineBase::setViewport(VkViewport& viewport) {
		viewport.x = static_cast<float>(_viewportRect.offset.x);
		viewport.y = static_cast<float>(_viewportRect.offset.y);
		viewport.width = static_cast<float>(_viewportRect.extent.width);
		viewport.height = static_cast<float>(_viewportRect.extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	}

	inline void PipelineBase::setViewportState(VkPipelineViewportStateCreateInfo& viewportState, VkViewport* viewportPtr) {
		viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = viewportPtr;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &_scissorRect;
	}

	inline void PipelineBase::setRasterizer(VkPipelineRasterizationStateCreateInfo& rasterizer) {
		rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = _polygonMode;
		rasterizer.lineWidth = _lineWidth;
		rasterizer.cullMode = _cullMode;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
	}

	inline void PipelineBase::setMultisampling(VkPipelineMultisampleStateCreateInfo& multisampling) {
		multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = _app->getAntiAliasSamples();
	}

	inline void PipelineBase::setDepthStencil(VkPipelineDepthStencilStateCreateInfo& depthStencil) {
		depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
	}

	inline void PipelineBase::setColorBlendAttachment(VkPipelineColorBlendAttachmentState& colorBlendAttachment) {
		colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
	}

	inline void PipelineBase::setColorBlending(VkPipelineColorBlendStateCreateInfo& colorBlending, VkPipelineColorBlendAttachmentState* colorBlendAttachmentPtr) {
		colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = colorBlendAttachmentPtr;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
	}

	inline void PipelineBase::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};

		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

		if (vkCreatePipelineLayout(_app->getDeviceContext().device_, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void PipelineBase::createDescriptorPool() {
		const auto& swap = _app->getSwapChain();
		auto devCon = _app->getDeviceContext().device_;

		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(swap.swapChainImages.size());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(swap.swapChainImages.size());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(swap.swapChainImages.size());

		if (vkCreateDescriptorPool(devCon, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void PipelineBase::addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const {
		for (const auto& sceneNode : _sceneNodes)
			sceneNode->addCommands(cmdBuff, _pipelineLayout, _descriptorSets[swapChainIdx]);
	}

}

