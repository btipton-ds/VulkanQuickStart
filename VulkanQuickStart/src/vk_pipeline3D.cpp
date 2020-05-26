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

#include <vk_pipeline3D.h>

#include <stdexcept>
#include <string>
#include <fstream>
#include <array>

#include <vk_device_context.h>
#include <vk_buffer.h>
#include <vk_vertex_types.h>
#include <vk_app.h>

namespace VK {

	using namespace std;

	string PipelineVertex3D::getShaderId() {
		return "ShaderVertex3D";
	}

	string PipelineVertex3D::getShaderIdMethod() {
		return getShaderId();
	}

	PipelineVertex3D::PipelineVertex3D(VulkanApp* app)
		: Pipeline(app)
	{
		_vertBindDesc = Vertex3_PNCTf::getBindingDescription();
		_vertAttribDesc = Vertex3_PNCTf::getAttributeDescriptions();
	}

	void PipelineVertex3D::createDescriptorSetLayout() {
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

	string PipelineVertex3DWSampler::getShaderId() {
		return "ShaderVertex3DSampler";
	}

	string PipelineVertex3DWSampler::getShaderIdMethod() {
		return getShaderId();
	}

	PipelineVertex3DWSampler::PipelineVertex3DWSampler(VulkanApp* app)
		: PipelineVertex3D(app)
	{
	}

	void PipelineVertex3DWSampler::createDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(_app->getDeviceContext().device_, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void PipelineVertex3DWSampler::createDescriptorSets() {
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

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = _descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = _descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = static_cast<uint32_t>(imageInfoList.size());
			descriptorWrites[1].pImageInfo = imageInfoList.data();

			vkUpdateDescriptorSets(dc, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}


}