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

#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <tm_boundingBox.h>
#include <vulkan/vulkan_core.h>

#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>
#include <vk_textureImage.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS SceneNodeBase {
	public:
		SceneNodeBase(const VulkanAppPtr& app);
		virtual ~SceneNodeBase();

		void toggleVisibility();
		void setVisibility(bool visible);
		bool isVisible() const;
		bool isReady() const;
		void setReady(bool ready);

		virtual void addCommands(VkCommandBuffer cmdBuff) const = 0;
		virtual void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const = 0;

		const VulkanAppPtr& getApp() const;

	protected:
		bool _ready = false;
		bool _visible = true;
		VulkanAppPtr _app;
	};

	inline bool SceneNodeBase::isVisible() const {
		return _visible;
	}

	inline bool SceneNodeBase::isReady() const {
		return _ready;
	}

	inline void SceneNodeBase::setReady(bool ready) {
		_ready = ready;
	}

	inline const VulkanAppPtr& SceneNodeBase::getApp() const {
		return _app;
	}
}

#pragma warning (pop)
