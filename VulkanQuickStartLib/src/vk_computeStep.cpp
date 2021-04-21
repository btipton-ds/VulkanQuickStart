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

#include <vulkan/vulkan_core.h>

#include <vk_logger.h>
#include <vk_computeStep.h>
#include <vk_deviceContext.h>
#include <vk_initializers.h>
#include <vk_textureImage.h>
#include <vk_vertexTypes.h>
#include <vk_shaderPool.h>

using namespace std;
using namespace VK;

ComputeStepBase::ComputeStepBase(const DeviceContextPtr& dc, const TextureImagePtr& srcImage, const TextureImagePtr& dstImage, const std::string& shaderId, size_t uboSize)
	: _uboSize(uboSize)
	, _dc(dc)
	, _srcImage(srcImage)
	, _dstImage(dstImage)
	, _shaderId(shaderId)
{}

void ComputeStepBase::updateUbo() {
	// Base class does nothing
}

void ComputeStepBase::build()
{
	createUniformBuffers();
	createDescriptorPool();
	createComputeQueue();
	createCompute();
	buildComputeCommandBuffer();
	//prepared = true;
}

void ComputeStepBase::createUniformBuffers() {
	if (_uboSize > 0) {
		_uboBuf = make_shared<Buffer>(_dc);

		_uboBuf->create(_uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}

void ComputeStepBase::createComputeQueue()
{
	auto device = _dc->_device;
	auto pDevice = _dc->_physicalDevice;

	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, NULL);
	assert(queueFamilyCount >= 1);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount, queueFamilyProperties.data());

	// Some devices have dedicated compute queues, so we first try to find a queue that supports compute and not graphics
	bool computeQueueFound = false;
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
		{
			_queueFamilyIndex = i;
			computeQueueFound = true;
			break;
		}
	}
	// If there is no dedicated compute queue, just find the first queue family that supports compute
	if (!computeQueueFound)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				_queueFamilyIndex = i;
				computeQueueFound = true;
				break;
			}
		}
	}

	// Compute is mandatory in Vulkan, so there must be at least one queue family that supports compute
	if (!computeQueueFound)
		throw runtime_error("Did not find a compute queue");

	vkGetDeviceQueue(device, _queueFamilyIndex, 0, &_queue);
}

void ComputeStepBase::createCompute() {
	VkDevice device = _dc->_device;

	// Create compute pipeline
	// Compute pipelines are created separate from graphics pipelines even if they use the same queue

	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
	uint32_t bindingIdx = 0;

	setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, bindingIdx++));
	setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, bindingIdx++));

	// Last binding: UBO, if present
	if (_uboBuf)
		setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, bindingIdx++));

	VkDescriptorSetLayoutCreateInfo descriptorLayout = initializers::descriptorSetLayoutCreateInfo(setLayoutBindings);
	VK_CHK(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &_descriptorSetLayout));

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
		initializers::pipelineLayoutCreateInfo(&_descriptorSetLayout, 1);

	VK_CHK(vkCreatePipelineLayout(device, &pPipelineLayoutCreateInfo, nullptr, &_pipelineLayout));

	VkDescriptorSetAllocateInfo allocInfo =
		initializers::descriptorSetAllocateInfo(_descriptorPool, &_descriptorSetLayout, 1);

	VK_CHK(vkAllocateDescriptorSets(device, &allocInfo, &_descriptorSet));
	std::vector<VkWriteDescriptorSet> computeWriteDescriptorSets;
	VkDescriptorBufferInfo bufferInfo = {};
	bindingIdx = 0;

	computeWriteDescriptorSets.push_back(initializers::writeDescriptorSet(_descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, bindingIdx++, &_srcImage->getDescriptor()));
	computeWriteDescriptorSets.push_back(initializers::writeDescriptorSet(_descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, bindingIdx++, &_dstImage->getDescriptor()));

	if (_uboBuf) {
		bufferInfo.buffer = *_uboBuf;
		bufferInfo.offset = 0;
		bufferInfo.range = _uboBuf->getSize();

		computeWriteDescriptorSets.push_back(initializers::writeDescriptorSet(_descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, bindingIdx++, &bufferInfo));
	}

	vkUpdateDescriptorSets(device, (uint32_t)computeWriteDescriptorSets.size(), computeWriteDescriptorSets.data(), 0, NULL);

	// Create compute shader pipelines
	VkComputePipelineCreateInfo computePipelineCreateInfo =
		initializers::computePipelineCreateInfo(_pipelineLayout, 0);

	ShaderPool& shaderPool = _dc->getShaderPool();
	auto shaders = shaderPool.getShader(_shaderId)->_shaders;
	if (shaders.size() != 1)
		throw runtime_error("Expected only one shader for compute");
	auto shaderRec = shaders.front();

	computePipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	computePipelineCreateInfo.stage.stage = shaderRec->_stage;
	computePipelineCreateInfo.stage.module = shaderRec->_module;
	computePipelineCreateInfo.stage.pName = "main";

	VK_CHK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &_pipeline));

	// Separate command pool as queue family for compute may be different than graphics
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = _queueFamilyIndex;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHK(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &_commandPool));

	// Create a command buffer for compute operations
	VkCommandBufferAllocateInfo cmdBufAllocateInfo =
		initializers::commandBufferAllocateInfo(
			_commandPool,
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			1);

	VK_CHK(vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, &_cmdBuf));

	// Fence for compute CB sync
	VkFenceCreateInfo fenceCreateInfo = initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VK_CHK(vkCreateFence(device, &fenceCreateInfo, nullptr, &_fence));

}

void ComputeStepBase::createDescriptorPool() {
	VkDevice device = _dc->_device;
	std::vector<VkDescriptorPoolSize> poolSizes;
	poolSizes.push_back(initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 2));
	if (_uboSize > 0)
		poolSizes.push_back(initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1));

	VkDescriptorPoolCreateInfo descriptorPoolInfo = initializers::descriptorPoolCreateInfo(poolSizes);
	VK_CHK(vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &_descriptorPool));
}

void ComputeStepBase::buildComputeCommandBuffer() {
	// Flush the queue if we're rebuilding the command buffer after a pipeline change to ensure it's not currently in use
	vkQueueWaitIdle(_queue);

	VkCommandBufferBeginInfo cmdBufInfo = {};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VK_CHK(vkBeginCommandBuffer(_cmdBuf, &cmdBufInfo));

	vkCmdBindPipeline(_cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, _pipeline);
	vkCmdBindDescriptorSets(_cmdBuf, VK_PIPELINE_BIND_POINT_COMPUTE, _pipelineLayout, 0, 1, &_descriptorSet, 0, 0);

	const VkExtent3D& extent = _dstImage->getImageInfo().extent;
	uint32_t nw = extent.width / _localDim;
	uint32_t nh = extent.height / _localDim;
	vkCmdDispatch(_cmdBuf, nw, nh, 1);

	vkEndCommandBuffer(_cmdBuf);
}

void ComputeStepBase::waitForFence() const {
	VkDevice device = _dc->_device;
	vkWaitForFences(device, 1, &_fence, VK_TRUE, UINT64_MAX);
}

void ComputeStepBase::submitCommands() {
	VkDevice device = _dc->_device;

	waitForFence();
	if (_prior)
		_prior->waitForFence();
	vkResetFences(device, 1, &_fence);

	VkSubmitInfo computeSubmitInfo = {};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &_cmdBuf;

	VK_CHK(vkQueueSubmit(_queue, 1, &computeSubmitInfo, _fence));
}
