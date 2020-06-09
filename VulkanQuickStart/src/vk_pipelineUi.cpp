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

#include <array>
#include <vk_pipelineUi.h>
#include <vk_pipelineSceneNodeUi.h>
#include <vk_shaderPool.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

PipelineUi::PipelineUi(const VulkanAppPtr& app)
	: Pipeline(app, "UiPipeline")
{
	_ubo._color = glm::vec4(1, 0, 0, 1);
	auto& shaders = app->getShaderPool();
	if (!shaders.getShader(getShaderId()))
		shaders.addShader(getShaderId(), { "shaders/shader_ui_vert.spv", "shaders/shader_ui_frag.spv" });
}

void PipelineUi::addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const {
	for (const auto& sceneNode : _sceneNodes) {
		SceneNodeUiPtr ptr = dynamic_pointer_cast<PipelineSceneNodeUi>(sceneNode);
		ptr->addCommandsIdx(cmdBuff, _pipelineLayout, swapChainIdx);
	}
}

void PipelineUi::cleanupSwapChain() {
	PipelineBase::cleanupSwapChain();

	for (auto& sceneNode : _sceneNodes) {
		SceneNodeUiPtr ptr = dynamic_pointer_cast<PipelineSceneNodeUi>(sceneNode);
		ptr->cleanupSwapChain(this);
	}
}

void PipelineUi::updateUniformBuffers(size_t swapChainIndex) {
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
		sceneNode->updateUniformBuffer(swapChainIndex);
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

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings.push_back(samplerLayoutBinding);

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(_app->getDeviceContext()->_device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

VkVertexInputBindingDescription VertexUi::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(VertexUi);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> VertexUi::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	VkVertexInputAttributeDescription posDesc = {};
	posDesc.binding = 0;
	posDesc.location = 0;
	posDesc.format = VK_FORMAT_R32G32_SFLOAT;
	posDesc.offset = offsetof(VertexUi, _pos);
	attributeDescriptions.push_back(posDesc);

#if 1
	VkVertexInputAttributeDescription texDesc = {};
	texDesc.binding = 0;
	texDesc.location = 1;
	texDesc.format = VK_FORMAT_R32G32_SFLOAT;
	texDesc.offset = offsetof(VertexUi, _texCoord);
	attributeDescriptions.push_back(texDesc);
#endif

	return attributeDescriptions;
}

bool VertexUi::operator==(const VertexUi& other) const {
	return _pos == other._pos /* && _texCoord == other._texCoord */;
}

