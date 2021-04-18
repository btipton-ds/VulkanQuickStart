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

#include <vk_vertexTypes.h>

using namespace std;
using namespace VK;

VkVertexInputBindingDescription Vertex2_Pf::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex2_PTf);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

vector<VkVertexInputAttributeDescription> Vertex2_Pf::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	attributeDescriptions.resize(1);

	size_t idx = 0;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = (uint32_t)idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex2_PTf, pos);

	return attributeDescriptions;
}

VkVertexInputBindingDescription Vertex2_PTf::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex2_PTf);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

vector<VkVertexInputAttributeDescription> Vertex2_PTf::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	attributeDescriptions.resize(2);

	size_t idx = 0;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = (uint32_t)idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex2_PTf, pos);

	idx++;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = (uint32_t)idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex2_PTf, texCoord);
	return attributeDescriptions;
}

VkVertexInputBindingDescription Vertex2Cd::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex2Cd);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

array<VkVertexInputAttributeDescription, 2> Vertex2Cd::getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R64G64_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex2Cd, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex2Cd, color);

	return attributeDescriptions;
}

VkVertexInputBindingDescription Vertex3_PNCf::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex3_PNCf);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

vector<VkVertexInputAttributeDescription> Vertex3_PNCf::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	attributeDescriptions.resize(3);

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex3_PNCf, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex3_PNCf, norm);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex3_PNCf, color);

	return attributeDescriptions;
}

bool Vertex3_PNCf::operator==(const Vertex3_PNCf& other) const {
	return pos == other.pos &&
		norm == other.norm &&
		color == other.color;
}

VkVertexInputBindingDescription Vertex3_PNCTf::getBindingDescription() {
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex3_PNCTf);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

vector<VkVertexInputAttributeDescription> Vertex3_PNCTf::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	attributeDescriptions.resize(5);

	uint32_t idx = 0;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex3_PNCTf, pos);

	idx++;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex3_PNCTf, norm);

	idx++;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex3_PNCTf, color);

	idx++;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[idx].offset = offsetof(Vertex3_PNCTf, texCoord);

	idx++;
	attributeDescriptions[idx].binding = 0;
	attributeDescriptions[idx].location = idx;
	attributeDescriptions[idx].format = VK_FORMAT_R32_SINT;
	attributeDescriptions[idx].offset = offsetof(Vertex3_PNCTf, texId);

	return attributeDescriptions;
}

bool Vertex3_PNCTf::operator==(const Vertex3_PNCTf& other) const {
	return pos == other.pos &&
		norm == other.norm &&
		color == other.color &&
		texCoord == other.texCoord &&
		texId == other.texId;
}
