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

#include <stdexcept>
#include <algorithm>
#include <iostream>

#include "vk_deviceContext.h"
#include "vk_buffer.h"
#include "vk_textureImage.h"
#include <vk_app.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;
using namespace VK;

TextureImage::~TextureImage() {
	destroy();
}

void TextureImage::destroy() {
	if (_sampler != VK_NULL_HANDLE) {
		vkDestroySampler(_context->_device, _sampler, nullptr);
		_sampler = VK_NULL_HANDLE;
		Image::destroy();
	}
	if (_sampler != VK_NULL_HANDLE) {
		cout << "textureSampler leak\n";
	}
}

void TextureImage::init(const string& filename) {
	destroy();
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	if (!pixels)
		throw runtime_error("Unable to read image file");
	init(texWidth, texHeight, pixels);
	stbi_image_free(pixels);
}

void TextureImage::init(size_t texWidth, size_t texHeight, const unsigned char* pixelsRGBA) {
	destroy();

	VkDeviceSize imageSize = texWidth * texHeight * 4;
	mipLevels_ = static_cast<uint32_t>(floor(log2(max(texWidth, texHeight)))) + 1;

	if (!pixelsRGBA) {
		throw runtime_error("failed to load texture image!");
	}

	Buffer stagingBuffer(_context);
	stagingBuffer.create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.update(pixelsRGBA, static_cast<size_t>(imageSize));

	initImage((uint32_t)texWidth, (uint32_t)texHeight, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	transitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels_);
	copyBufferToImage(stagingBuffer, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

	generateMipmaps(VK_FORMAT_R8G8B8A8_UNORM, (uint32_t)texWidth, (uint32_t)texHeight);
	createTextureSampler();

	_view = createImageView(VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels_);
}

void TextureImage::initImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {

	_imageInfo = {};
	_imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	_imageInfo.imageType = VK_IMAGE_TYPE_2D;
	_imageInfo.extent.width = width;
	_imageInfo.extent.height = height;
	_imageInfo.extent.depth = 1;
	_imageInfo.mipLevels = mipLevels_;
	_imageInfo.arrayLayers = 1;
	_imageInfo.format = format;
	_imageInfo.tiling = tiling;
	_imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	_imageInfo.usage = usage;
	_imageInfo.samples = numSamples;
	_imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(_context->_device, &_imageInfo, nullptr, &_image) != VK_SUCCESS) {
		throw runtime_error("failed to create image!");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(_context->_device, _image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = _context->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(_context->_device, &allocInfo, nullptr, &_memory) != VK_SUCCESS) {
		throw runtime_error("failed to allocate image memory!");
	}

	vkBindImageMemory(_context->_device, _image, _memory, 0);
}

void TextureImage::copyBufferToImage(const Buffer& buffer, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = _context->beginSingleTimeCommands();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(commandBuffer, buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	_context->endSingleTimeCommands(commandBuffer);
}

void TextureImage::generateMipmaps(VkFormat imageFormat, int32_t texWidth, int32_t texHeight) {

	// Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(_context->_physicalDevice, imageFormat, &formatProperties);

	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw runtime_error("texture image format does not support linear blitting!");
	}

	VkCommandBuffer commandBuffer = _context->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = _image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels_; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit = {};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(commandBuffer,
			_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels_ - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	_context->endSingleTimeCommands(commandBuffer);
}

void TextureImage::createTextureSampler() {
	_samplerInfo = {};
	_samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	_samplerInfo.magFilter = VK_FILTER_LINEAR;
	_samplerInfo.minFilter = VK_FILTER_LINEAR;
	_samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	_samplerInfo.anisotropyEnable = VK_TRUE;
	_samplerInfo.maxAnisotropy = 16;
	_samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	_samplerInfo.unnormalizedCoordinates = VK_FALSE;
	_samplerInfo.compareEnable = VK_FALSE;
	_samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	_samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	_samplerInfo.minLod = 0;
	_samplerInfo.maxLod = static_cast<float>(mipLevels_);
	_samplerInfo.mipLodBias = 0;

	if (vkCreateSampler(_context->_device, &_samplerInfo, nullptr, &_sampler) != VK_SUCCESS) {
		throw runtime_error("failed to create texture sampler!");
	}
}
