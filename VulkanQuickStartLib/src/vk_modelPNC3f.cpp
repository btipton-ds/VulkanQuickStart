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

#include <map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <vk_deviceContext.h>
#include <vk_vertexTypes.h>
#include <vk_modelPNC3f.h>
#include <triMesh.h>

#include <vk_app.h>

using namespace std;
using namespace VK;

namespace std {
	struct compareFunc {
		inline bool operator()(ModelPNC3f::VertexType const& lhs, ModelPNC3f::VertexType const& rhs) const {
			for (int i = 0; i < 3; i++) {
				if (lhs.pos[i] < rhs.pos[i])
					return true;
				else if (lhs.pos[i] > rhs.pos[i])
					return false;
			}
			return false;
		}
	};

	namespace {
		template<class T>
		inline glm::vec3 conv(const T& pt) {
			return glm::vec3(static_cast<float>(pt[0]), static_cast<float>(pt[1]), static_cast<float>(pt[2]));
		}

	}
}

ModelPNC3f::ModelPNC3f(const VulkanAppPtr& app, const TriMesh::CMeshPtr& meshPtr, const glm::vec3& color)
	: SceneNodePNC3f(app)
	, _vertexBuffer(app->getDeviceContext())
	, _indexBuffer(app->getDeviceContext())
{
	loadModel(meshPtr, color);
	createVertexBuffer();
	createIndexBuffer();
}

ModelPNC3f::ModelPNC3f(const VulkanAppPtr& app)
	: SceneNodePNC3f(app)
	, _vertexBuffer(app->getDeviceContext())
	, _indexBuffer(app->getDeviceContext())
{
}

void ModelPNC3f::addCommands(VkCommandBuffer cmdBuff) const {
	VkBuffer vertexBuffers[] = { getVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(cmdBuff, getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

//	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &_descriptorSets[swapChainIndex], 0, nullptr);

	vkCmdDrawIndexed(cmdBuff, numIndices(), 1, 0, 0, 0);
}

void ModelPNC3f::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
}

ModelPNC3f::BoundingBox ModelPNC3f::getBounds() const {
	return _bounds;
}

namespace {
	Vector3f conv(const glm::vec3& pt) {
		return Vector3f(pt[0], pt[1], pt[2]);
	}
}

void ModelPNC3f::loadModel(const TriMesh::CMeshPtr& meshPtr, const glm::vec3& color) {
	const float matchAngle = 30.0; // Degrees. Set angle > 90 for flat shading.
	const float matchCos = cosf(static_cast<float> (matchAngle * EIGEN_PI / 180.0));
	_bounds.clear();

	map<VertexType, size_t, compareFunc> vertMap;
	_vertices.clear();

	size_t numFused = 0;
	_indices.resize(3 * meshPtr->numTris());
	for (size_t i = 0; i < meshPtr->numTris(); i++) {
		const auto& srcTri = meshPtr->getTri(i);

		TriMesh::CVertex verts[] = {
			meshPtr->getVert(srcTri[0]),
			meshPtr->getVert(srcTri[1]),
			meshPtr->getVert(srcTri[2])
		};

		auto v0 = verts[1]._pt - verts[0]._pt;
		auto v1 = verts[2]._pt - verts[0]._pt;
		auto n = v0.cross(v1);
		checkNAN(n);

		for (size_t j = 0; j < 3; j++) {
			VertexType vertex;
			vertex.pos = conv(verts[j]._pt);
			vertex.norm = conv(n.normalized());
			vertex.color = color;

			_bounds.merge(conv(vertex.pos));

			uint32_t newIdx;
			auto iter = vertMap.find(vertex);
			if (iter == vertMap.end()) {
				size_t idx = _vertices.size();
				vertMap.insert(make_pair(vertex, idx));
				_vertices.push_back(vertex);
				newIdx = static_cast<uint32_t> (idx);
			}
			else {
				const VertexType& curVert = iter->first;
				float dp = glm::dot(vertex.norm, curVert.norm);
				if (dp > matchCos) {
					_vertices[iter->second].norm = curVert.norm + vertex.norm;
					newIdx = static_cast<uint32_t> (iter->second);
					numFused++;
				}
				else {
					size_t idx = _vertices.size();
					vertMap.insert(make_pair(vertex, idx));
					_vertices.push_back(vertex);
					newIdx = static_cast<uint32_t> (idx);
				}
			}
			_indices[3 * i + j] = newIdx;
		}

	}

	cout << "numFused: " << numFused << "\n";

	for (size_t i = 0; i < _vertices.size(); i++) {
		const auto& vert = _vertices[i];
		_vertices[i].norm = normalize(vert.norm);
	}
}

void ModelPNC3f::createVertexBuffer() {
	_vertexBuffer.create(_vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void ModelPNC3f::createIndexBuffer() {
	_indexBuffer.create(_indices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

