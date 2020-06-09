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

#include <vk_pipelineSceneNodeBase.h>
#include <vk_pipelineBase.h>
#include <vk_app.h>

using namespace VK;

PipelineSceneNodeBase::PipelineSceneNodeBase(const PipelineBasePtr& ownerPipeline)
	: _ownerPipeline(ownerPipeline)
{}

PipelineSceneNodeBase::~PipelineSceneNodeBase() {
	auto& device = _ownerPipeline->getApp()->getDeviceContext()->_device;
	if (_descriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(device, _descriptorPool, nullptr);
}

void PipelineSceneNodeBase::toggleVisibility() {
	_visible = !_visible;
	_ownerPipeline->getApp()->changed();
}

void PipelineSceneNodeBase::setVisibility(bool visible) {
	_visible = visible;
	_ownerPipeline->getApp()->changed();
}

void PipelineSceneNodeBase::updateUniformBuffer(size_t swapChainIndex) {

}

void PipelineSceneNodeBase::cleanupSwapChain() {
	if (_uniformBuffers.empty())
		return;

	_uniformBuffers.clear();
	_descriptorSets.clear();

	auto devCon = _ownerPipeline->getApp()->getDeviceContext()->_device;
	if (_descriptorPool != VK_NULL_HANDLE)
		vkDestroyDescriptorPool(devCon, _descriptorPool, nullptr);
}

void PipelineSceneNodeBase::createUniformBuffers() {
	size_t uboSize = _ownerPipeline->getUboSize();
	if (uboSize == stm1)
		return;

	const auto& app = _ownerPipeline->getApp();
	const auto& swap = app->getSwapChain();
	auto devCon = app->getDeviceContext()->_device;

	size_t swapChainSize = (uint32_t)swap._vkImages.size();

	_uniformBuffers.clear();
	_uniformBuffers.reserve(swapChainSize);

	for (size_t i = 0; i < swapChainSize; i++) {
		_uniformBuffers.push_back(Buffer(app->getDeviceContext()));
		_uniformBuffers.back().create(uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}
