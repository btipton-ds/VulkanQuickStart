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

#include <vk_sceneNode.h>

namespace VK {


	class SceneNodeGroup;
	using SceneNodeGroupPtr = std::shared_ptr<SceneNodeGroup>;
	using SceneNodeGroupConstPtr = std::shared_ptr<const SceneNodeGroup>;

	class SceneNodeGroup : public SceneNode {
	public:
		SceneNodeGroup();
		virtual ~SceneNodeGroup();

		void addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, const VkDescriptorSet& descSet) const override;
		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const override;

		template<typename FUNC_TYPE>
		inline void traverse(FUNC_TYPE func) const {
			for (const auto& child : _childScenes) {
				SceneNodeGroupConstPtr gr = std::dynamic_pointer_cast<const SceneNodeGroup> (child);
				if (gr) {
					gr->traverse(func);
				}
				else {
					func(child);
				}
			}
		}

		template<typename FUNC_TYPE>
		inline void traverse(FUNC_TYPE func) {
			for (auto& child : _childScenes) {
				SceneNodeGroupConstPtr gr = std::dynamic_pointer_cast<const SceneNodeGroup> (child);
				if (gr) {
					gr->traverse(func);
				}
				else {
					func(child);
				}
			}
		}

		size_t numChildNodes() const;
		size_t addChild(const SceneNodePtr& child);
		const SceneNodePtr& getChild(size_t index) const;
		SceneNodePtr& getChild(size_t index);

	private:
		std::vector<SceneNodePtr> _childScenes;
	};

	inline SceneNodeGroup::SceneNodeGroup() {

	}

	inline SceneNodeGroup::~SceneNodeGroup() {

	}

	inline size_t SceneNodeGroup::numChildNodes() const {
		return _childScenes.size();
	}

	inline size_t SceneNodeGroup::addChild(const SceneNodePtr& child) {
		size_t result = _childScenes.size();
		_childScenes.push_back(child);
		return result;
	}

	inline const SceneNodePtr& SceneNodeGroup::getChild(size_t index) const {
		return _childScenes[index];
	}

	inline SceneNodePtr& SceneNodeGroup::getChild(size_t index) {
		return _childScenes[index];
	}

}
