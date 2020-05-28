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

#include <iostream>
#include <cstring>

#include "vk_deviceContext.h"
#include "vk_buffer.h"

using namespace std;
using namespace VK;

Buffer::~Buffer() {
	destroy();
}

void Buffer::destroy() {
	if (dc_ && buffer_ != VK_NULL_HANDLE) {
		vkDestroyBuffer(dc_->device_, buffer_, nullptr);
		vkFreeMemory(dc_->device_, bufferMemory_, nullptr);
		buffer_ = VK_NULL_HANDLE;
		dc_->buffers_.erase(this);
	}
	if (buffer_ != VK_NULL_HANDLE) {
		cout << "device leak\n";
	}
}

void Buffer::update(const void* value, size_t size) {
	void* data;
	vkMapMemory(dc_->device_, bufferMemory_, 0, size, 0, &data);
	memcpy(data, value, size);
	vkUnmapMemory(dc_->device_, bufferMemory_);
}

void Buffer::create(DeviceContext& dc, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	if (size > DEV_MAX_BUF_SIZE) {
		throw runtime_error("tryint to create a buffer larger than DEV_MAX_BUF_SIZE.");
	}
	destroy();
	dc_ = &dc;
	dc_->buffers_.insert(this);

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(dc.device_, &bufferInfo, nullptr, &buffer_) != VK_SUCCESS) {
		throw runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(dc.device_, buffer_, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = dc.findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(dc.device_, &allocInfo, nullptr, &bufferMemory_) != VK_SUCCESS) {
		throw runtime_error("failed to allocate buffer memory!");
	}

	if (bufferMemory_ == (VkDeviceMemory)0x12 || bufferMemory_ == (VkDeviceMemory)0x58) {
		cout << "Leaked memory\n";
	}
	vkBindBufferMemory(dc.device_, buffer_, bufferMemory_, 0);
}

void Buffer::create(DeviceContext& dc, const void* value, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
	if (size > DEV_MAX_BUF_SIZE) {
		throw runtime_error("tryint to create a buffer larger than DEV_MAX_BUF_SIZE.");
	}

	Buffer stagingBuffer;
	stagingBuffer.create(dc, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.update(value, size);

	create(dc, size, usage, properties);

	copyBuffer(stagingBuffer, size);
}

void Buffer::copyBuffer(const Buffer& srcBuffer, size_t size) {
	VkCommandBuffer commandBuffer = dc_->beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer.buffer_, buffer_, 1, &copyRegion);

	dc_->endSingleTimeCommands(commandBuffer);
}
