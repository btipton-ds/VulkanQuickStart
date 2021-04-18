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

#include <iostream>
#include <cstring>

#include "vk_deviceContext.h"
#include "vk_buffer.h"
#include <vk_app.h>

using namespace std;
using namespace VK;

Buffer::~Buffer() {
	destroy();
}

void Buffer::destroy() {
	if (buffer_ != VK_NULL_HANDLE) {
		vkDestroyBuffer(_context->_device, buffer_, nullptr);
		vkFreeMemory(_context->_device, bufferMemory_, nullptr);
		buffer_ = VK_NULL_HANDLE;
	}
}

void Buffer::update(const void* value, VkDeviceSize size) {
	if (size != _size)
		throw "Invalid buffer size";
	void* data;
	vkMapMemory(_context->_device, bufferMemory_, 0, size, 0, &data);
	memcpy(data, value, size);
	vkUnmapMemory(_context->_device, bufferMemory_);
}

void Buffer::create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	if (size > DEV_MAX_BUF_SIZE) {
		throw runtime_error("tryint to create a buffer larger than DEV_MAX_BUF_SIZE.");
	}
	destroy();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(_context->_device, &bufferInfo, nullptr, &buffer_) != VK_SUCCESS) {
		throw runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_context->_device, buffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _context->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(_context->_device, &allocInfo, nullptr, &bufferMemory_) != VK_SUCCESS) {
		throw runtime_error("failed to allocate buffer memory!");
	}

	if (bufferMemory_ == (VkDeviceMemory)0x12 || bufferMemory_ == (VkDeviceMemory)0x58) {
		cout << "Leaked memory\n";
	}
	vkBindBufferMemory(_context->_device, buffer_, bufferMemory_, 0);
	_size = size;
}

void Buffer::create(const void* value, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	if (size > DEV_MAX_BUF_SIZE) {
		throw runtime_error("tryint to create a buffer larger than DEV_MAX_BUF_SIZE.");
	}

	Buffer stagingBuffer(_context);
	stagingBuffer.create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.update(value, size);

	create(size, usage, properties);

	copyBuffer(stagingBuffer, size);
}

void Buffer::copyBuffer(const Buffer& srcBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = _context->beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer.buffer_, buffer_, 1, &copyRegion);

	_context->endSingleTimeCommands(commandBuffer);
}
