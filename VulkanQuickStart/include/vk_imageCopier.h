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

#include <vk_forwardDeclarations.h>

namespace VK {

	class ImageCopier {
	public:
		ImageCopier(const VulkanAppPtr& app, VkImage srcImage, const VkExtent2D& extent, VkFormat format, size_t bufSize);
		~ImageCopier();
		const char* getPersistentCopy() const;
		const char* getVolitileCopy() const;
		bool getColorSwizzle() const;
		uint32_t getRowPitch() const;

	private:
		void copyImages(VkImage srcImage, const VkExtent2D& extent, VkFormat format, VkImage dstImage);

		void lockImages(VkCommandBuffer copyCmd, VkImage& srcImage, VkImage& dstImage);
		void unlockImages(VkCommandBuffer copyCmd, VkImage& srcImage, VkImage& dstImage);

		void createVkImage(VkDevice device, const VkExtent2D& extent, VkImage& dstImage);
		bool doesSupportsBlit(VkPhysicalDevice physicalDevice, VkFormat format);
		void blitImage(VkCommandBuffer copyCmd, const VkExtent2D& extent, VkImage& srcImage, VkImage& dstImage);
		void copyImage(VkCommandBuffer copyCmd, const VkExtent2D& extent, VkImage& srcImage, VkImage& dstImage);

		const VulkanAppPtr& _app;
		VkDevice _device;
		bool _colorSwizzle = false;
		uint32_t _rowPitch;
		size_t _bufSize;
		VkImage dstImage;
		VkMemoryRequirements memRequirements;
		VkMemoryAllocateInfo memAllocInfo;
		VkDeviceMemory dstImageMemory;
		VkSubresourceLayout subResourceLayout;


	};

	inline bool ImageCopier::getColorSwizzle() const {
		return _colorSwizzle;
	}

	inline uint32_t ImageCopier::getRowPitch() const {
		return _rowPitch;
	}


}