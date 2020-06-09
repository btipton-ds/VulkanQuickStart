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

#include <unordered_map>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <vk_modelObj.h>
#include <vk_app.h>

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


ModelObj::ModelObj(const PipelineBasePtr& _ownerPipeline, const std::string& path, const std::string& filename)
	: PipelineSceneNode3DWSampler(_ownerPipeline)
	, _vertexBuffer(_ownerPipeline->getApp()->getDeviceContext())
	, _indexBuffer(_ownerPipeline->getApp()->getDeviceContext())
{
	loadModel(path, filename);
	createVertexBuffer();
	createIndexBuffer();
}

void ModelObj::addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, size_t swapChainIndex) const {
	VkBuffer vertexBuffers[] = { getVertexBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);

	vkCmdBindIndexBuffer(cmdBuff, getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	vkCmdBindDescriptorSets(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &_descriptorSets[swapChainIndex], 0, nullptr);

	vkCmdDrawIndexed(cmdBuff, numIndices(), 1, 0, 0, 0);
}

void ModelObj::buildImageInfoList(vector<VkDescriptorImageInfo>& imageInfoList) const {
	imageInfoList.clear();

	for (const auto& texture : _textureImagesDiffuse) {
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->getImageView();
		imageInfo.sampler = texture->getSampler();
		imageInfoList.push_back(imageInfo);
	}
}

ModelObj::BoundingBox ModelObj::getBounds() const {
	return _bounds;
}

namespace {
	void replaceAllDirTokens(string& data)
	{
		string toSearch;
		string replaceStr;
#ifndef _WIN32
		toSearch = "\\";
		replaceStr = "/";
#else
		toSearch = "/";
		replaceStr = "\\";
#endif
		// Get the first occurrence
		size_t pos = data.find(toSearch);

		// Repeat till end is reached
		while (pos != string::npos)
		{
			// Replace this occurrence of Sub String
			data.replace(pos, toSearch.size(), replaceStr);
			// Get the next occurrence from the current position
			pos = data.find(toSearch, pos + replaceStr.size());
		}
	}

}

void ModelObj::loadModel(string path, string filename) {
	_bounds.clear();

	tinyobj::attrib_t attrib;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;
	string warn, err;

#ifndef _WIN32
	replaceAllDirTokens(path);
	replaceAllDirTokens(filename);
#else
	replaceAllDirTokens(path);
	replaceAllDirTokens(filename);
#endif

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (path + filename).c_str(), path.c_str())) {
		throw runtime_error(warn + err);
	}

	unordered_map<VertexType, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			VertexType vertex = {};

			vertex.texId = shape.mesh.material_ids[index.vertex_index];
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
				uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
				_vertices.push_back(vertex);
				_bounds.merge(conv(vertex.pos));
			}

			_indices.push_back(uniqueVertices[vertex]);
		}

		for (size_t i = 0; i < _indices.size(); i += 3) {
			auto& vert0 = _vertices[_indices[i]];
			auto& vert1 = _vertices[_indices[i + 1]];
			auto& vert2 = _vertices[_indices[i + 2]];

			glm::vec3 v0 = vert1.pos - vert0.pos;
			glm::vec3 v1 = vert2.pos - vert0.pos;
			glm::vec3 n = glm::normalize(glm::cross(v0, v1));

			vert0.norm = n;
			vert1.norm = n;
			vert2.norm = n;

		}
	}

	for (auto mat : materials) {
		string fName = "";
		if (mat.diffuse_texname.length() > 0) {
			fName = mat.diffuse_texname;
		} else if (mat.bump_texname.length() > 0) {
			fName = mat.bump_texname;
		} else {
			throw runtime_error("This option is not supported yet.");
		}
		replaceAllDirTokens(fName);
		auto tex = TextureImage::create(_ownerPipeline->getApp()->getDeviceContext(), path + fName);
		_textureImagesDiffuse.push_back(tex);
	}

}

void ModelObj::createVertexBuffer() {
	_vertexBuffer.create(_vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

void ModelObj::createIndexBuffer() {
	_indexBuffer.create(_indices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}


void ModelObj::createDescriptorPool() {
	auto app = _ownerPipeline->getApp();
	const auto& swap = app->getSwapChain();
	auto devCon = app->getDeviceContext()->device_;

	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swap._vkImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swap._vkImages.size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swap._vkImages.size());

	if (vkCreateDescriptorPool(devCon, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void ModelObj::createDescriptorSets() {
	auto app = _ownerPipeline->getApp();
	auto dc = app->getDeviceContext()->device_;

	const auto& swap = app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap._vkImages.size();

	std::vector<VkDescriptorSetLayout> layouts(swapChainSize, _ownerPipeline->getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool; // TODO I haven't figured out yet if there should be one pool for the entire pipeline or not. Attempts to do that all crashed.
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
		bufferInfo.range = _uniformBuffers[i].getSize();

		vector<VkDescriptorImageInfo> imageInfoList;
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

		uint32_t imageCount = min(Pipeline3DWSampler::getMaxSamplers(), static_cast<uint32_t> (imageInfoList.size()));

		VkWriteDescriptorSet descSampler = {};
		descSampler.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descSampler.dstSet = _descriptorSets[i];
		descSampler.dstBinding = 1;
		descSampler.dstArrayElement = 0;
		descSampler.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descSampler.descriptorCount = imageCount;
		descSampler.pImageInfo = imageInfoList.data();
		descriptorWrites.push_back(descSampler);

		vkUpdateDescriptorSets(dc, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void ModelObj::createUniformBuffers() {
	auto app = _ownerPipeline->getApp();
	size_t bufferSize = sizeof(UniformBufferObject);
	const auto& swap = app->getSwapChain();
	size_t swapChainSize = (uint32_t)swap._vkImages.size();

	_uniformBuffers.clear();
	_uniformBuffers.reserve(swapChainSize);

	for (size_t i = 0; i < swapChainSize; i++) {
		_uniformBuffers.push_back(Buffer(app->getDeviceContext()));
		_uniformBuffers.back().create(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}
