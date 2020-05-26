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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace VK {

	struct DeviceContext;

	class Image {
	public:
		~Image();
		void destroy();
		operator VkImage() const;
		operator VkImageView() const;

		void set(DeviceContext& dc, VkImage image, VkDeviceMemory memory, VkImageView view);

		void create(DeviceContext& dc, VkFormat format, VkImageUsageFlags flagBits, uint32_t width, uint32_t height, VkSampleCountFlagBits _msaaSamples);

		void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
			VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		static VkImageView createImageView(const Image& image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		VkImageView createImageView(VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

		void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	protected:
		DeviceContext* dc_ = nullptr;
		VkImage image_ = VK_NULL_HANDLE;
		VkDeviceMemory memory_ = VK_NULL_HANDLE;
		VkImageView view_ = VK_NULL_HANDLE;
	};

	inline Image::operator VkImage() const {
		return image_;
	}

	inline Image::operator VkImageView() const {
		return view_;
	}
}