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

#include <defines.h>

#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vk_forwardDeclarations.h>
#include <vk_imageCopier.h>

namespace VK {

	class Image {
	public:
		static size_t pixelSize(VkFormat format);
		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		template<typename FUNC_TYPE>
		static size_t processImage(const VulkanAppPtr& app, VkImage image, const VkExtent2D& extent, VkFormat format, size_t bufSize, FUNC_TYPE func) {
			size_t newBufSize = extent.width * extent.height * pixelSize(format);
			if (bufSize != newBufSize)
				return newBufSize;

			ImageCopier copier(app, image, extent, format, bufSize);

			func(copier.getVolitileCopy(), copier.getRowPitch(), copier.getColorSwizzle());

			return bufSize;
		}

		static void saveImage(const std::string& filename, const VkExtent2D& extent, uint32_t rowPitch, bool colorSwizzle, const char* pix);

		static void saveImage(const std::string& filename, const VulkanAppPtr& app, VkImage image, const VkExtent2D& extent, VkFormat format) {
			size_t bufSize = processImage(app, image, extent, format, 0, [](const char* p, uint32_t rowPitch, bool colorSwizzle) {});
			if (bufSize != stm1) {
				bufSize = processImage(app, image, extent, format, bufSize, [&](const char* p, uint32_t rowPitch, bool colorSwizzle) {
					saveImage(filename, extent, rowPitch, colorSwizzle, p);
				});
			}
		}
		static size_t getImageData(const VulkanAppPtr& app, VkImage image, const VkExtent2D& extent, VkFormat format, const char*& data, size_t bufSize);

		~Image();
		void destroy();
		operator VkImage() const;
		operator VkImageView() const;

		void set(DeviceContext& dc, VkImage image, VkDeviceMemory memory, VkImageView view);

		void create(DeviceContext& dc, VkFormat format, VkImageUsageFlags flagBits, uint32_t width, uint32_t height, VkSampleCountFlagBits _msaaSamples);

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

		VkFormat getFormat() const;

		size_t getImageData(const VulkanAppPtr& app, const char*& data, size_t bufSize) const;

		void saveImage(const VulkanAppPtr& app, const std::string& filename) {
			saveImage(filename, app, _image, _extent, _format);
		}

	protected:
		DeviceContext* _dc = nullptr;
		VkFormat _format = VK_FORMAT_UNDEFINED;
		VkExtent2D _extent = {};
		VkImage _image = VK_NULL_HANDLE;
		VkDeviceMemory _memory = VK_NULL_HANDLE;
		VkImageView _view = VK_NULL_HANDLE;
	};

	inline Image::operator VkImage() const {
		return _image;
	}

	inline Image::operator VkImageView() const {
		return _view;
	}

	inline VkFormat Image::getFormat() const {
		return _format;
	}

	inline size_t Image::getImageData(const VulkanAppPtr& app, const char*& data, size_t bufSize) const {
		return getImageData(app, _image, _extent, _format, data, bufSize);
	}
}