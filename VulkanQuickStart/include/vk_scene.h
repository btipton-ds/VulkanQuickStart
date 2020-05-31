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

#include <vulkan/vulkan_core.h>

#include <vk_sceneNode.h>

namespace VK {

	class Scene {
	public:
		Scene();
		virtual ~Scene();

		void addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, VkDescriptorSet& descSet);
	
		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList);

		size_t addRootSceneNode(const SceneNodeBasePtr& node);
		const SceneNodeBasePtr& getRootSceneNode(size_t index) const;
		SceneNodeBasePtr& getRootSceneNode(size_t index);

	private:
		std::vector<SceneNodeBasePtr> _rootNodes;
	};
	using ScenePtr = std::shared_ptr<Scene>;

	inline size_t Scene::addRootSceneNode(const SceneNodeBasePtr& node) {
		size_t result = _rootNodes.size();
		_rootNodes.push_back(node);
		return result;
	}

	inline const SceneNodeBasePtr& Scene::getRootSceneNode(size_t index) const {
		return _rootNodes[index];
	}

	inline SceneNodeBasePtr& Scene::getRootSceneNode(size_t index) {
		return _rootNodes[index];
	}

}
