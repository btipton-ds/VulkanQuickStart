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

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vk_forwardDeclarations.h>
#include <vk_imageCopier.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS Image {
	public:
		static size_t pixelSize(VkFormat format);
		size_t imageSize() const;

		static VkImageView createImageView(const DeviceContextPtr& context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		static void saveImageJpg(const std::string& filename, int width, int height, int numChannels, const char* data);

		template<typename FUNC_TYPE>
		void processImage(FUNC_TYPE func) const {
			size_t bufSize = imageSize();

			ImageCopier copier(_context, *this, bufSize);
			ImageCopier::MappedMemory mem(copier);

			func(mem.getData(), copier.getSubResourceLayout(), copier.getColorSwizzle());
		}

		void saveImage(const std::string& filename) const;
		size_t getImageData(const char*& data, size_t bufSize) const;

		Image(const DeviceContextPtr& context);
		Image(const Image& src) = default;
		Image(const DeviceContextPtr& context, const VkSwapchainCreateInfoKHR& info, VkImage image);
		~Image();

		void destroy();

		void set(VkImage image, VkDeviceMemory memory, VkImageView view);

		void create(VkFormat format, VkImageUsageFlags usageFlags, uint32_t width, uint32_t height, VkSampleCountFlagBits samples);

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		VkImageView getImageView() const;

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

		VkFormat getFormat() const;

		const VkImageCreateInfo& getImageInfo() const;
		VkImage getVkImage() const;

		const DeviceContextPtr& getContext() const;

		VkImageLayout getLayout() const;

	protected:
		void saveImage(const std::string& filename, const VkSubresourceLayout& vkLayout, bool colorSwizzle, const char* pix) const;

		DeviceContextPtr _context;
		VkImageCreateInfo _imageInfo = {};
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;
		VkImageLayout _imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	};

	inline size_t Image::imageSize() const {
		size_t result = _imageInfo.extent.width * _imageInfo.extent.height * pixelSize(_imageInfo.format);
		return result;
	}

	inline VkImageView Image::getImageView() const {
		return _view;
	}

	inline VkImage Image::getVkImage() const {
		return _image;
	}

	inline const VkImageCreateInfo& Image::getImageInfo() const {
		return _imageInfo;
	}

	inline VkFormat Image::getFormat() const {
		return _imageInfo.format;
	}

	inline const DeviceContextPtr& Image::getContext() const {
		return _context;
	}

	inline VkImageLayout Image::getLayout() const {
		return _imageLayout;
	}

}

#pragma warning (pop)
