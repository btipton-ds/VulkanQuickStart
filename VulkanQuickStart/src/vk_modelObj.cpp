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

#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <vk_modelObj.h>

using namespace std;
using namespace VK;

namespace std {
	template<> struct hash<ModelObj::VertexType> {
		size_t operator()(ModelObj::VertexType const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};

	Vector3f conv(const glm::vec3& pt) {
		return Vector3f(pt[0], pt[1], pt[2]);
	}
}


ModelObj::ModelObj(DeviceContext& dc, const std::string& modelFilename, const std::string& imageFilename)
	: _dc(&dc)
{
	loadModel(modelFilename);
	createVertexBuffer();
	createIndexBuffer();
	_textureImage = TextureImage::create(dc, imageFilename);
}

void ModelObj::addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, const VkDescriptorSet& descSet) const {
	VkBuffer vertexBuffers[] = { getVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(cmdBuff, getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descSet, 0, nullptr);

	vkCmdDrawIndexed(cmdBuff, numIndices(), 1, 0, 0, 0);
}

void ModelObj::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
}

void ModelObj::getImageInfo(VkDescriptorImageInfo& imageInfo) {
	const auto& texture = getTexture();
	imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = *texture;
	imageInfo.sampler = *texture;
}

ModelObj::BoundingBox ModelObj::getBounds() const {
	return _bounds;
}

void ModelObj::loadModel(const std::string& filename) {
	_bounds.clear();

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<VertexType, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			VertexType vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.norm = { 0.0,0.0,0.0 };
			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices_.size());
				vertices_.push_back(vertex);
				_bounds.merge(conv(vertex.pos));
			}

			indices_.push_back(uniqueVertices[vertex]);
		}

		for (size_t i = 0; i < indices_.size(); i += 3) {
			auto& vert0 = vertices_[indices_[i]];
			auto& vert1 = vertices_[indices_[i + 1]];
			auto& vert2 = vertices_[indices_[i + 2]];

			glm::vec3 v0 = vert1.pos - vert0.pos;
			glm::vec3 v1 = vert2.pos - vert0.pos;
			glm::vec3 n = glm::normalize(glm::cross(v0, v1));

			vert0.norm = n;
			vert1.norm = n;
			vert2.norm = n;

		}
	}
}

void ModelObj::createVertexBuffer() {
	vertexBuffer_.create(*_dc, vertices_, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void ModelObj::createIndexBuffer() {
	indexBuffer_.create(*_dc, indices_, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}


