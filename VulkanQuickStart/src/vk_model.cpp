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

#include "vk_device_context.h"
#include "vk_vertex_types.h"
#include "vk_model.h"
#include <triMesh.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

using namespace std;
using namespace VK;

namespace std {
	template<> struct hash<Vertex3_PNCTf> {
		size_t operator()(Vertex3_PNCTf const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

Model::Model(DeviceContext& dc, const TriMesh::CMeshPtr& meshPtr)
	: _dc(&dc)
{
	loadModel(meshPtr);
	createVertexBuffer();
	createIndexBuffer();
}

Model::Model(DeviceContext& dc)
	: _dc(&dc)
{
}

void Model::addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, const VkDescriptorSet& descSet) const {
	VkBuffer vertexBuffers[] = { getVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(cmdBuff, getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descSet, 0, nullptr);

	vkCmdDrawIndexed(cmdBuff, numIndices(), 1, 0, 0, 0);
}

void Model::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
}

Model::BoundingBox Model::getBounds() const {
	return _bounds;
}

namespace {
	Vector3f conv(const glm::vec3& pt) {
		return Vector3f(pt[0], pt[1], pt[2]);
	}
}

void Model::loadModel(const TriMesh::CMeshPtr& meshPtr) {
	_bounds.clear();

	vertices_.resize(meshPtr->numVertices());
	for (size_t vertIdx = 0; vertIdx < meshPtr->numVertices(); vertIdx++) {
		const auto& scrVert = meshPtr->getVert(vertIdx);
		Vertex3_PNCTf& vertex = vertices_[vertIdx];

		vertex.pos = {
			(float) scrVert._pt[0],
			(float) scrVert._pt[1],
			(float) scrVert._pt[2],
		};

		_bounds.merge(conv(vertex.pos));

		vertex.texCoord = { 0, 0 };

		vertex.norm = { 0.0, 0.0, 0.0 };
		vertex.color = { 1.0f, 1.0f, 1.0f };
	}

	indices_.reserve(3 * meshPtr->numTris());
	for (size_t i = 0; i < meshPtr->numTris(); i++) {
		const auto& srcTri = meshPtr->getTri(i);
		indices_.push_back((uint32_t)srcTri[0]);
		indices_.push_back((uint32_t)srcTri[1]);
		indices_.push_back((uint32_t)srcTri[2]);
	}

	// Sharing vertices, so flat shading isn't working!
	for (size_t i = 0; i < meshPtr->numTris(); i++) {
		const auto& srcTri = meshPtr->getTri(i);
		auto& vert0 = vertices_[srcTri[0]];
		auto& vert1 = vertices_[srcTri[1]];
		auto& vert2 = vertices_[srcTri[2]];

		glm::vec3 v0 = vert1.pos - vert0.pos;
		glm::vec3 v1 = vert2.pos - vert0.pos;
		glm::vec3 n = glm::normalize(glm::cross(v0, v1));

		vert0.norm += n;
		vert1.norm += n;
		vert2.norm += n;
	}

	for (auto& vert : vertices_) {
		normalize(vert.norm);
	}
}

void Model::createVertexBuffer() {
	vertexBuffer_.create(*_dc, vertices_, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void Model::createIndexBuffer() {
	indexBuffer_.create(*_dc, indices_, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

