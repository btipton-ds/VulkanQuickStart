#pragma once

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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <memory>

#include <vk_forwardDeclarations.h>
#include "vk_image.h"

namespace VK {

	class EXPORT_VQS TextureImage : public Image {
	public:
		static TextureImagePtr newPtr(const DeviceContextPtr& context, VkFormat format, VkImageUsageFlags flagBits, uint32_t width,
			uint32_t height, VkSampleCountFlagBits samples);

		TextureImage(const TextureImage& src) = default;
		~TextureImage();
		void destroy();

		VkSampler getSampler() const;

		static TextureImagePtr create(const DeviceContextPtr& context, const std::string& filename);
		static TextureImagePtr create(const DeviceContextPtr& context, size_t width, size_t height, const unsigned char* pixelsRGBA);
		static TextureImagePtr createImage(const DeviceContextPtr& context, uint32_t width, uint32_t height,
			VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		void copyBufferToImage(const Buffer& buffer, uint32_t width, uint32_t height);
		void generateMipmaps(VkFormat format, int32_t texWidth, int32_t texHeight);
		void createTextureSampler();
		const VkSamplerCreateInfo& getSamplerInfo() const;

		VkDescriptorImageInfo& getDescriptor();

	private:
		TextureImage(const DeviceContextPtr& context);
		void init(const std::string& filename);
		void init(size_t width, size_t height, const unsigned char* pixelsRGBA);
		void initImage(uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		VkSamplerCreateInfo _samplerInfo;
		uint32_t mipLevels_ = 0;
		VkSampler _sampler = VK_NULL_HANDLE;
                VkDescriptorImageInfo _descriptor;
	};

	inline TextureImagePtr TextureImage::newPtr(const DeviceContextPtr& context, VkFormat format, VkImageUsageFlags flagBits, uint32_t width,
		uint32_t height, VkSampleCountFlagBits samples) {
		TextureImagePtr ptr = std::shared_ptr<TextureImage>(new TextureImage(context));
		ptr->Image::create(format, flagBits, width, height, samples);
		ptr->createTextureSampler();
		return ptr;
	}

	inline const VkSamplerCreateInfo& TextureImage::getSamplerInfo() const {
		return _samplerInfo;
	}

	inline TextureImage::TextureImage(const DeviceContextPtr& context)
	: Image(context)
	{}

	inline VkSampler TextureImage::getSampler() const {
		return _sampler;
	}

	inline TextureImagePtr TextureImage::create(const DeviceContextPtr& context, const std::string& filename) {
		TextureImagePtr result(new TextureImage(context));
		result->init(filename);
		return result;
	}
	
	inline TextureImagePtr TextureImage::create(const DeviceContextPtr& context, size_t width, size_t height, const unsigned char* pixelsRGBA) {
		TextureImagePtr result(new TextureImage(context));
		result->init(width, height, pixelsRGBA);
		return result;
	}
	
	inline TextureImagePtr TextureImage::createImage(const DeviceContextPtr& context, uint32_t width, uint32_t height, VkSampleCountFlagBits numSamples,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) {
		TextureImagePtr result(new TextureImage(context));
		result->initImage(width, height, numSamples, format, tiling, usage, properties);
		return result;
	}

	inline VkDescriptorImageInfo& TextureImage::getDescriptor() {
		_descriptor = {};
		_descriptor.imageView = _view;
		_descriptor.sampler = _sampler;
		_descriptor.imageLayout = _imageLayout;

		return _descriptor;
	}

}
