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

#include <vector>
#include <exception>

#include <vulkan/vulkan_core.h>

#include <vk_logger.h>
#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>
#include <vk_deviceContext.h>

namespace VK {

	template<class VERT_TYPE, class UBO_TYPE>
	class SceneNodeToPipelineBinding {
	public:
		using UboType = UBO_TYPE;
		using PipelineType = Pipeline<VERT_TYPE, UboType>;
		using SceneNodeType = SceneNode<VERT_TYPE>;
		using SceneNodeTypePtr = std::shared_ptr<SceneNodeType>;

		SceneNodeToPipelineBinding(PipelineType* pipeline, const SceneNodeTypePtr& sceneNode);
		~SceneNodeToPipelineBinding();

		const SceneNodeTypePtr& getSceneNode() const;

		void addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, size_t index) const;

		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const;
		void updateUniformBuffer(size_t index, UboType ubo);

		void build();
		void cleanup();
		void createDescriptorPool(VkDevice device);
		void createUniformBuffers(VkDevice device);
		void createDescriptorSets(VkDevice device);

		bool isReady() const {
			return _sceneNode->isVisible();
		}

		bool isVisible() const {
			return _sceneNode->isVisible();
		}

	private:
		PipelineType* _pipeline;
		SceneNodeTypePtr _sceneNode;

		size_t _numBuffers = 0;
		std::vector<Buffer> _uniformBuffers;
		VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> _descriptorSets;
	};

	template<class VERT_TYPE, class UBO_TYPE>
	using SceneNodeToPipelineBindingPtr = std::shared_ptr<SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>>;

	template<class VERT_TYPE, class UBO_TYPE>
	inline SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::SceneNodeToPipelineBinding(PipelineType* pipeline, const SceneNodeTypePtr& sceneNode)
		: _pipeline(pipeline)
		, _sceneNode(sceneNode)
	{
		_numBuffers = _pipeline->getPipelineGroup()->getNumBuffers();
		if (_numBuffers == 0)
			THROW("_numBuffers == 0");
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::build() {
		DeviceContextPtr context = _pipeline->getDeviceContext();
		VkDevice device = context->_device;

		createDescriptorPool(device);
		createUniformBuffers(device);
		createDescriptorSets(device);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::~SceneNodeToPipelineBinding() {
		cleanup();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline const typename SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::SceneNodeTypePtr& SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::getSceneNode() const {
		return _sceneNode;
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, size_t index) const {
		vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &_descriptorSets[index], 0, nullptr);
		_sceneNode->addCommands(cmdBuff);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
		_sceneNode->buildImageInfoList(imageInfoList);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::updateUniformBuffer(size_t index, UboType ubo) {
		_pipeline->updateSceneNodeUbo(_sceneNode, ubo);
		_uniformBuffers[index].update(ubo);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::cleanup() {
		VkDevice device = _pipeline->getDeviceContext()->_device;
		if (_descriptorPool != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(device, _descriptorPool, nullptr);
		_descriptorPool = VK_NULL_HANDLE;

		_descriptorSets.clear(); // Sets are destroyed when the pool is destroyed
		_uniformBuffers.clear();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::createDescriptorPool(VkDevice device) {
		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(_numBuffers * _pipeline->getDescriptorUboCount());
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(_numBuffers * _pipeline->getDescriptorSamplerCount());

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(_numBuffers);

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
			THROW("failed to create descriptor pool!");
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::createUniformBuffers(VkDevice device) {
		DeviceContextPtr context = _pipeline->getDeviceContext();
		size_t bufferSize = sizeof(UboType);

		if (_uniformBuffers.empty()) {
			_uniformBuffers.reserve(_numBuffers);
			for (size_t i = 0; i < _numBuffers; i++)
				_uniformBuffers.push_back(Buffer(context));
		}
		for (size_t i = 0; i < _numBuffers; i++) {
			_uniformBuffers[i].create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>::createDescriptorSets(VkDevice device) {
		if (_descriptorSets.empty())
			_descriptorSets.resize(_numBuffers);

		VkDescriptorSetLayout layout = _pipeline->getDescriptorSetLayout();
		std::vector<VkDescriptorSetLayout> layouts(_numBuffers, layout);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = _descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(_numBuffers);
		allocInfo.pSetLayouts = layouts.data();

		if (vkAllocateDescriptorSets(device, &allocInfo, _descriptorSets.data()) != VK_SUCCESS) {
			THROW("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < _numBuffers; i++) {
			VkDescriptorBufferInfo bufferInfo = {};

			bufferInfo.buffer = _uniformBuffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = _uniformBuffers[i].getSize();

			std::vector<VkDescriptorImageInfo> imageInfoList;
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

			uint32_t imageCount = std::min(_pipeline->getMaxSamplers(), static_cast<uint32_t> (imageInfoList.size()));

			if (imageCount > 0) {
				VkWriteDescriptorSet descSampler = {};
				descSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descSampler.dstSet = _descriptorSets[i];
				descSampler.dstBinding = 1;
				descSampler.dstArrayElement = 0;
				descSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descSampler.descriptorCount = imageCount;
				descSampler.pImageInfo = imageInfoList.data();
				descriptorWrites.push_back(descSampler);
			}
			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}


}