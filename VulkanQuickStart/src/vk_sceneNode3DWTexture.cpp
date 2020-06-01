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

#include <vk_pipeline3DWithTexture.h>
#include <vk_sceneNode3DWTexture.h>
#include <vk_pipeline3D.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

SceneNode3DWTexture::SceneNode3DWTexture()
	: _modelXForm(glm::mat4(1.0f))
{
}

SceneNode3DWTexture::~SceneNode3DWTexture() {
}


void SceneNode3DWTexture::updateUniformBuffer(PipelineBase* pipeline, size_t swapChainIndex) {
	auto pipeline3D = dynamic_cast<PipelineVertex3DWSampler*>(pipeline);
	auto ubo = pipeline3D->getUniformBuffer();
	ubo.modelView *= _modelXForm;
	ubo.draw = isDrawn() ? 1 : 0;
	pipeline->updateUniformBufferTempl(swapChainIndex, ubo);
}


void SceneNode3DWTexture::cleanupSwapChain(PipelineVertex3DWSampler* ownerPipeline) {
	_descriptorSets.clear();

	auto devCon = ownerPipeline->getApp()->getDeviceContext().device_;
	if (_descriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(devCon, _descriptorPool, nullptr);
}

void SceneNode3DWTexture::addCommandsIdx(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, size_t swapChainIdx) {
	addCommands(cmdBuff, pipelineLayout, _descriptorSets[swapChainIdx]);
}


void SceneNode3DWTexture::createDescriptorPool(PipelineVertex3DWSampler* ownerPipeline) {
	auto app = ownerPipeline->getApp();
	const auto& swap = app->getSwapChain();
	auto devCon = app->getDeviceContext().device_;

	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swap._vkImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swap._vkImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swap._vkImages.size());

	if (vkCreateDescriptorPool(devCon, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void SceneNode3DWTexture::createDescriptorSets(PipelineVertex3DWSampler* ownerPipeline) {
	auto app = ownerPipeline->getApp();
	auto dc = app->getDeviceContext().device_;

	const auto& swap = app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap._vkImages.size();

	std::vector<VkDescriptorSetLayout> layouts(swapChainSize, ownerPipeline->getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool; // TODO I haven't figured out yet if there should be one pool for the entire pipeline or not. Attempts to do that all crashed.
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainSize);
	allocInfo.pSetLayouts = layouts.data();

	_descriptorSets.resize(swapChainSize);

	if (vkAllocateDescriptorSets(dc, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainSize; i++) {
		VkDescriptorBufferInfo bufferInfo = {};

		bufferInfo.buffer = ownerPipeline->getUniformBuffers()[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(ownerPipeline->getUniformBuffers()[i]);

		vector<VkDescriptorImageInfo> imageInfoList;
		buildImageInfoList(imageInfoList);

		std::vector<VkWriteDescriptorSet> descriptorWrites;

		VkWriteDescriptorSet descUniform = {};
		descUniform.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descUniform.dstSet = _descriptorSets[i];
		descUniform.dstBinding = 0;
		descUniform.dstArrayElement = 0;
		descUniform.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descUniform.descriptorCount = 1;
		descUniform.pBufferInfo = &bufferInfo;
		descriptorWrites.push_back(descUniform);

		uint32_t imageCount = min(PipelineVertex3DWSampler::getMaxSamplers(), static_cast<uint32_t> (imageInfoList.size()));

		VkWriteDescriptorSet descSampler = {};
		descSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descSampler.dstSet = _descriptorSets[i];
		descSampler.dstBinding = 1;
		descSampler.dstArrayElement = 0;
		descSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSampler.descriptorCount = imageCount;
		descSampler.pImageInfo = imageInfoList.data();
		descriptorWrites.push_back(descSampler);

		vkUpdateDescriptorSets(dc, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
