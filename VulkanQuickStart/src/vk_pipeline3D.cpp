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

#include <vk_pipeline3D.h>

#include <stdexcept>
#include <string>
#include <fstream>
#include <array>
#include <algorithm>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vk_pipeline3D.h>
#include <vk_sceneNode3D.h>
#include <vk_deviceContext.h>
#include <vk_buffer.h>
#include <vk_vertexTypes.h>
#include <vk_app.h>

using namespace VK;
using namespace std;

string PipelineVertex3D::getShaderId() {
	return "PipelineVertex3D";
}

string PipelineVertex3D::getShaderIdMethod() {
	return getShaderId();
}

PipelineVertex3D::PipelineVertex3D(const VulkanAppPtr& app)
	: Pipeline(app)
{}

namespace {
	inline glm::vec3 conv(const Vector3f& pt) {
		return glm::vec3(pt[0], pt[1], pt[2]);
	}
	inline glm::vec4 conv4(const Vector3f& pt) {
		return glm::vec4(pt[0], pt[1], pt[2], 1);
	}

	PipelineVertex3D::BoundingBox transform(const PipelineVertex3D::BoundingBox& bb, const glm::mat4& xform) {
		PipelineVertex3D::BoundingBox result;
		glm::vec4 pt(0, 0, 0, 1);
		for (int i = 0; i < 2; i++) {
			pt[0] = i == 0 ? bb.getMin()[0] : bb.getMax()[0];
			for (int j = 0; j < 2; j++) {
				pt[1] = j == 0 ? bb.getMin()[1] : bb.getMax()[1];
				for (int k = 0; k < 2; k++) {
					pt[2] = k == 0 ? bb.getMin()[2] : bb.getMax()[2];
					pt = xform * pt;
					result.merge(Vector3f(pt[0], pt[1], pt[2]));
				}
			}
		}
		return result;
	}
}

PipelineVertex3D::BoundingBox PipelineVertex3D::getBounds() const {
	BoundingBox bb;
	for (auto& sceneNode : _sceneNodes) {
		SceneNode3DPtr node3D = dynamic_pointer_cast<SceneNode3D> (sceneNode);
		BoundingBox modelBb = node3D->getBounds();
		bb.merge(transform(modelBb, node3D->getModelTransform()));
	}
	return bb;
}

void PipelineVertex3D::updateUniformBuffer(size_t swapChainIndex) {
	for (auto& sceneNode : _sceneNodes) {
		if (sceneNode->isEnabled())
			sceneNode->updateUniformBuffer(this, swapChainIndex);
	}

}

void PipelineVertex3D::createDescriptorSetLayout() {

	// Stage flags being a bit field, leads to the belief that you can use one layout binding for two shader stages.
	// Reading and testing indicates this doesn't work. If there's a way to make it work, I haven't found it.
	// It looks like there can be a UBO for each stage, but they need to be distinct - one for each stage.

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(_app->getDeviceContext().device_, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void PipelineVertex3D::createDescriptorSets() {
	auto dc = _app->getDeviceContext().device_;

	const auto& swap = _app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap._vkImages.size();

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
			if (sceneNode->isEnabled())
				sceneNode->buildImageInfoList(imageInfoList);
		}

		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = _descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(dc, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void PipelineVertex3D::createUniformBuffers() {
	size_t bufferSize = sizeof(UniformBufferObject);
	const auto& swap = _app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap._vkImages.size();

	_uniformBuffers.reserve(swapChainSize);

	for (size_t i = 0; i < swapChainSize; i++) {
		_uniformBuffers.push_back(Buffer(_app.get()));
		_uniformBuffers.back().create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}


