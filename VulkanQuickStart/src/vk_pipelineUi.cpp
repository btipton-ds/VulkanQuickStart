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

#include <array>
#include <vk_pipelineUi.h>
#include <vk_shaderPool.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

string PipelineUi::getShaderId() {
	return "PipelineUi";
}

string PipelineUi::getShaderIdMethod() {
	return getShaderId();
}

PipelineUi::PipelineUi(VulkanApp* app)
	: Pipeline(app)
{
	_ubo._color = glm::vec4(1, 0, 0, 1);
	_vertBindDesc = Vertex2D::getBindingDescription();
	_vertAttribDesc = Vertex2D::getAttributeDescriptions();
	auto& shaders = app->getShaderPool();
	if (!shaders.getShader(getShaderId()))
		shaders.addShader(getShaderId(), { "shaders/shader_ui_vert.spv", "shaders/shader_ui_frag.spv" });
}

void PipelineUi::updateUniformBuffer(size_t swapChainIndex) {
	int widthPx, heightPx;

	auto win = _app->getWindow();
	glfwGetWindowSize(win, &widthPx, &heightPx);

	int dpi = _app->getWindowDpi();
	float widthIn = widthPx / (float)dpi;
	float heightIn = heightPx / (float)dpi;
	float widthPts = widthIn * 72;
	float heightPts = heightIn * 72;

	_ubo._scale = glm::vec2(1.0f / widthPts, 1.0f / heightPts);
	_ubo._offset = glm::vec2(-1.0f, -1.0f);
	_ubo._color = glm::vec4(1, 0, 0, 1);

	for (auto& sceneNode : _sceneNodes) {
		sceneNode->updateUniformBuffer(this, swapChainIndex);
	}
}

void PipelineUi::createUniformBuffers() {
	auto& device = _app->getDeviceContext();
	size_t bufferSize = sizeof(UniformBufferObject);
	const auto& swap = _app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap.swapChainImages.size();

	_uniformBuffers.resize(swapChainSize);

	for (size_t i = 0; i < swapChainSize; i++) {
		_uniformBuffers[i].create(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void PipelineUi::createDescriptorSetLayout() {
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings.push_back(uboLayoutBinding);

#if 1
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings.push_back(samplerLayoutBinding);
#endif

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(_app->getDeviceContext().device_, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void PipelineUi::createDescriptorSets() {
	auto dc = _app->getDeviceContext().device_;

	const auto& swap = _app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap.swapChainImages.size();

	std::vector<VkDescriptorSetLayout> layouts(swapChainSize, _descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainSize);
	allocInfo.pSetLayouts = layouts.data();

	_descriptorSets.resize(swapChainSize);

	if (vkAllocateDescriptorSets(dc, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainSize; i++) {
		VkDescriptorBufferInfo bufferInfo = {};

		bufferInfo.buffer = _uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		std::vector<VkDescriptorImageInfo> imageInfoList;
		for (const auto& sceneNode : _sceneNodes) {
			sceneNode->buildImageInfoList(imageInfoList);
		}

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

#if 1
		VkWriteDescriptorSet descSampler = {};
		descSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descSampler.dstSet = _descriptorSets[i];
		descSampler.dstBinding = 1;
		descSampler.dstArrayElement = 0;
		descSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSampler.descriptorCount = static_cast<uint32_t>(imageInfoList.size());
		descSampler.pImageInfo = imageInfoList.data();
		descriptorWrites.push_back(descSampler);
#endif

		vkUpdateDescriptorSets(dc, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

VkVertexInputBindingDescription PipelineUi::Vertex2D::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex2D);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> PipelineUi::Vertex2D::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	VkVertexInputAttributeDescription posDesc = {};
	posDesc.binding = 0;
	posDesc.location = 0;
	posDesc.format = VK_FORMAT_R32G32_SFLOAT;
	posDesc.offset = offsetof(Vertex2D, _pos);
	attributeDescriptions.push_back(posDesc);

#if 1
	VkVertexInputAttributeDescription texDesc = {};
	texDesc.binding = 0;
	texDesc.location = 1;
	texDesc.format = VK_FORMAT_R32G32_SFLOAT;
	texDesc.offset = offsetof(Vertex2D, _texCoord);
	attributeDescriptions.push_back(texDesc);
#endif

	return attributeDescriptions;
}

bool PipelineUi::Vertex2D::operator==(const Vertex2D& other) const {
	return _pos == other._pos /* && _texCoord == other._texCoord */;
}

