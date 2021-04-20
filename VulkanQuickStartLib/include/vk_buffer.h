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

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vk_forwardDeclarations.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS Buffer {
	public:
		Buffer(const DeviceContextPtr& context);
		~Buffer();
		void destroy();
		void create(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		void create(const void* value, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		void copyBuffer(const Buffer& srcBuffer, VkDeviceSize size);
		void update(const void* value, VkDeviceSize size);

		operator VkBuffer() const;
		operator VkDeviceMemory() const;
		VkDeviceSize getSize() const;

		template<class T>
		void create(const std::vector<T>& vec, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		template<class T>
		void update(const T& value);

		template<class T>
		void updateVec(const std::vector<T>& vec);

	private:

		VkDeviceSize _size = 0;
		DeviceContextPtr _context;
		VkBuffer buffer_ = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory_ = VK_NULL_HANDLE;
	};

	inline Buffer::Buffer(const DeviceContextPtr& context)
		: _context(context)
	{}

	template<class T>
	inline void Buffer::create(const std::vector<T>& vec, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
		create(vec.data(), (VkDeviceSize)(sizeof(T) * vec.size()), usage, properties);
	}

	template<class T>
	inline void Buffer::updateVec(const std::vector<T>& vec) {
		update(vec.data(), (VkDeviceSize)(sizeof(T) * vec.size()));
	}

	template<class T>
	inline void Buffer::update(const T& value) {
		update(&value, sizeof(T));
	}

	inline Buffer::operator VkBuffer() const {
		return buffer_;
	}

	inline Buffer::operator VkDeviceMemory() const {
		return bufferMemory_;
	}

	inline VkDeviceSize Buffer::getSize() const {
		return _size;
	}

}

#pragma warning (pop)

