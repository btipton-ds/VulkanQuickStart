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

#include <vk_forwardDeclarations.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS ImageCopier {
	public:
		struct MappedMemory {
			MappedMemory(const ImageCopier& copier);
			~MappedMemory();
			const char* getData() const;

		private:
			VkDevice _device;
			VkDeviceMemory _memory;
			const char* _data;
		};

		ImageCopier(const DeviceContextPtr& context, const Image& srcImage, size_t bufSize);
		~ImageCopier();
		const char* getPersistentCopy() const;
		bool getColorSwizzle() const;
		VkImageSubresource getSubResource() const;
		VkSubresourceLayout getSubResourceLayout() const;

		VkDevice getDevice() const;
		VkDeviceMemory getDstImageMemory() const;

	private:
		void copyImages();

		void lockImages(VkCommandBuffer copyCmd);
		void unlockImages(VkCommandBuffer copyCmd);

		void createVkImage();
		bool doesSupportsBlit();
		void blitImage(VkCommandBuffer copyCmd);
		void copyImage(VkCommandBuffer copyCmd);

		DeviceContextPtr _context;
		VkDevice _device = VK_NULL_HANDLE;
		const Image& _srcImage;
		VkExtent3D _extent;
		VkFormat _format;
		bool _colorSwizzle = false;
		VkDeviceSize _rowPitch;
		size_t _bufSize;
		VkImage _dstImage = VK_NULL_HANDLE;
		VkDeviceMemory _dstImageMemory = VK_NULL_HANDLE;
		VkImageSubresource _subResource;
		VkSubresourceLayout _subResourceLayout;


	};

	inline bool ImageCopier::getColorSwizzle() const {
		return _colorSwizzle;
	}

	inline VkImageSubresource ImageCopier::getSubResource() const {
		return _subResource;
	}

	inline VkSubresourceLayout ImageCopier::getSubResourceLayout() const {
		return _subResourceLayout;
	}

	inline VkDevice ImageCopier::getDevice() const {
		return _device;
	}

	inline VkDeviceMemory ImageCopier::getDstImageMemory() const {
		return _dstImageMemory;
	}

	inline ImageCopier::MappedMemory::MappedMemory(const ImageCopier& copier)
		: _device(copier.getDevice())
		, _memory(copier.getDstImageMemory())
	{
		vkMapMemory(_device, _memory, 0, VK_WHOLE_SIZE, 0, (void**)&_data);
		_data += copier.getSubResourceLayout().offset;
	}

	inline ImageCopier::MappedMemory::~MappedMemory() {
		if (_memory != VK_NULL_HANDLE)
			vkUnmapMemory(_device, _memory);
	}

	inline const char* ImageCopier::MappedMemory::getData() const {
		return _data;
	}

}

#pragma warning (pop)
