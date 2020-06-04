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

#include <vk_imageCopier.h>
#include <vk_image.h>
#include <vk_app.h>

#include <VulkanDevice.hpp>
#include <VulkanInitializers.hpp>
#include <VulkanTools.h>

using namespace VK;


ImageCopier::ImageCopier(VulkanApp* app, const Image& srcImage, size_t bufSize)
	: _app(app)
	, _bufSize(bufSize)
	, _srcImage(srcImage)
	, _extent(srcImage.getImageInfo().extent)
	, _format(srcImage.getImageInfo().format)
{
	auto& dc = _app->getDeviceContext();
	_device = _app->getDeviceContext().device_;

	createVkImage();

	// Create memory to back up the image
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(_device, _dstImage, &memRequirements);
	VkMemoryAllocateInfo memAllocInfo(vks::initializers::memoryAllocateInfo());
	memAllocInfo.allocationSize = memRequirements.size;
	// Memory must be host visible to copy from
	memAllocInfo.memoryTypeIndex = dc.getMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	VK_CHECK_RESULT(vkAllocateMemory(_device, &memAllocInfo, nullptr, &_dstImageMemory));
	VK_CHECK_RESULT(vkBindImageMemory(_device, _dstImage, _dstImageMemory, 0));

	copyImages();

	// Get layout of the image (including row pitch)
	_subResource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
	vkGetImageSubresourceLayout(_device, _dstImage, &_subResource, &_subResourceLayout);
	_rowPitch = _subResourceLayout.rowPitch;
}

ImageCopier::~ImageCopier() {
	// Clean up resources
	vkUnmapMemory(_device, _dstImageMemory);
	vkFreeMemory(_device, _dstImageMemory, nullptr);
	vkDestroyImage(_device, _dstImage, nullptr);
}

const char* ImageCopier::getPersistentCopy() const {
	const char* tempPtr = getVolitileCopy();

	char* p = new char[_bufSize];
	memcpy(p, tempPtr, _bufSize);

	return p;
}

const char* ImageCopier::getVolitileCopy() const {
	// Map image memory so we can start copying from it
	char* tempPtr;
	vkMapMemory(_device, _dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&tempPtr);
	tempPtr += _subResourceLayout.offset;
	return tempPtr;
}

void ImageCopier::copyImages() {
	auto& dc = _app->getDeviceContext();
	bool supportsBlit = doesSupportsBlit();

	_colorSwizzle = false;
	// Check if source is BGR 
	// Note: Not complete, only contains most common and basic BGR surface formats for demonstation purposes
	if (!supportsBlit)
	{
		std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM };
		_colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), _format) != formatsBGR.end());
	}

	// Do the actual blit from the swapchain image to our host visible destination image
	VkCommandBuffer copyCmd = _app->beginSingleTimeCommands();

	lockImages(copyCmd);
	if (supportsBlit)
		blitImage(copyCmd);
	else
		copyImage(copyCmd);

	unlockImages(copyCmd);

	//	vulkanDevice->flushCommandBuffer(copyCmd, queue);
	_app->endSingleTimeCommands(copyCmd);
}

void ImageCopier::lockImages(VkCommandBuffer copyCmd) {
	vks::tools::insertImageMemoryBarrier(
		copyCmd,
		_dstImage,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	vks::tools::insertImageMemoryBarrier(
		copyCmd,
		_srcImage.getVkImage(),
		VK_ACCESS_MEMORY_READ_BIT,
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
}

void ImageCopier::unlockImages(VkCommandBuffer copyCmd) {
	vks::tools::insertImageMemoryBarrier(
		copyCmd,
		_dstImage,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	vks::tools::insertImageMemoryBarrier(
		copyCmd,
		_srcImage.getVkImage(),
		VK_ACCESS_TRANSFER_READ_BIT,
		VK_ACCESS_MEMORY_READ_BIT,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
}



void ImageCopier::createVkImage() {
	// Create the linear tiled destination image to copy to and to read the memory from
	VkImageCreateInfo imageCreateCI = _srcImage.getImageInfo();
	/*
	imageCreateCI.imageType = VK_IMAGE_TYPE_2D;
	// Note that vkCmdBlitImage (if supported) will also do format conversions if the swapchain color format would differ
	imageCreateCI.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateCI.extent = _extent;
	imageCreateCI.extent.depth = 1;
	imageCreateCI.arrayLayers = 1;
	imageCreateCI.mipLevels = 1;
	imageCreateCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateCI.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateCI.tiling = VK_IMAGE_TILING_LINEAR;
	*/
	imageCreateCI.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	// Create the image
	VK_CHECK_RESULT(vkCreateImage(_device, &imageCreateCI, nullptr, &_dstImage));

}

bool ImageCopier::doesSupportsBlit() {
	auto& dc = _app->getDeviceContext();
	// Check blit support for source and destination
	VkFormatProperties formatProps;

	// Check if the device supports blitting from optimal images (the swapchain images are in optimal format)
	vkGetPhysicalDeviceFormatProperties(dc.physicalDevice_, _format, &formatProps);

	if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
		return false;
	}

	// Check if the device supports blitting to linear images 
	vkGetPhysicalDeviceFormatProperties(dc.physicalDevice_, VK_FORMAT_R8G8B8A8_UNORM, &formatProps);
	if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
		return false;
	}

	return true;
}


void ImageCopier::blitImage(VkCommandBuffer copyCmd) {
	VkOffset3D blitSize;
	blitSize.x = _extent.width;
	blitSize.y = _extent.height;
	blitSize.z = 1;
	VkImageBlit imageBlitRegion{};
	imageBlitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.srcSubresource.layerCount = 1;
	imageBlitRegion.srcOffsets[1] = blitSize;
	imageBlitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageBlitRegion.dstSubresource.layerCount = 1;
	imageBlitRegion.dstOffsets[1] = blitSize;

	// Issue the blit command
	vkCmdBlitImage(
		copyCmd,
		_srcImage.getVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		_dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageBlitRegion,
		VK_FILTER_NEAREST);
}

void ImageCopier::copyImage(VkCommandBuffer copyCmd) {
	// Otherwise use image copy (requires us to manually flip components)
	VkImageCopy imageCopyRegion{};
	imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyRegion.srcSubresource.layerCount = 1;
	imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCopyRegion.dstSubresource.layerCount = 1;
	imageCopyRegion.extent = _extent;
	imageCopyRegion.extent.depth = 1;

	// Issue the copy command
	vkCmdCopyImage(
		copyCmd,
		_srcImage.getVkImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		_dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&imageCopyRegion);
}
