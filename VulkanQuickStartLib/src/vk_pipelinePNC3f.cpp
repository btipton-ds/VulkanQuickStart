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

#include <vk_logger.h>
#include <vk_pipelinePNC3f.h>

#include <stdexcept>
#include <string>
#include <fstream>
#include <array>
#include <algorithm>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vk_pipelinePNC3f.h>
#include <vk_sceneNodePNC3f.h>
#include <vk_deviceContext.h>
#include <vk_buffer.h>
#include <vk_vertexTypes.h>
#include <vk_app.h>

using namespace VK;
using namespace std;

PipelinePNC3f::PipelinePNC3f(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect)
	: Pipeline(plGroup, shaderId, rect)
{}

namespace {
	PipelinePNC3f::BoundingBox transform(const PipelinePNC3f::BoundingBox& bb, const glm::mat4& xform) {
		PipelinePNC3f::BoundingBox result;
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

PipelinePNC3f::BoundingBox PipelinePNC3f::getBounds() const {
	BoundingBox bb;

	for (auto& binding : _sceneNodeBindings) {
		auto sceneNode = binding->getSceneNode();
		SceneNodePNC3fPtr node3D = dynamic_pointer_cast<SceneNodePNC3f> (sceneNode);
		BoundingBox modelBb = node3D->getBounds();
		bb.merge(transform(modelBb, node3D->getModelTransform()));
	}
	return bb;
}

void PipelinePNC3f::updateSceneNodeUbo(const SceneNodePtr& sceneNode, UniformBufferObject3D& ubo) const {
	SceneNodePNC3fPtr ptr = dynamic_pointer_cast<SceneNodePNC3f>(sceneNode);
	ptr->updateUbo(ubo);
}

void PipelinePNC3f::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	setDescriptorUboCount(uboLayoutBinding.descriptorCount);

	std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(_dc->_device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
		THROW("failed to create descriptor set layout!");
	}
}
