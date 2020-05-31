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

#include <vk_pipelineBase.h>
#include <vk_sceneNode.h>

namespace VK {

	template<class UBO_TYPE, class VERT_TYPE>
	class Pipeline : public PipelineBase {
	public:
		using VertexType = VERT_TYPE;
		using SceneNode = GCC_CLASS SceneNode<Pipeline<UBO_TYPE, VERT_TYPE>>;
		using SceneNodePtr = std::shared_ptr<SceneNode>;
		using SceneNodeList = std::vector<SceneNodePtr>;

		Pipeline(const VulkanAppPtr& app);

		void addSceneNode(const SceneNodePtr& node);
		size_t numSceneNodes() const override;

		void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const override;

	protected:
		SceneNodeList _sceneNodes;
	};

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipeline(const VulkanAppPtr& app) {
		PIPELINE_TYPE* ptr = new PIPELINE_TYPE(app);
		return PipelinePtr<PIPELINE_TYPE>(ptr);
	}

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipelineWithSource(const VulkanAppPtr& app, const std::string& vertShaderFilename, const std::string& fragShaderFilename) {
		const std::string shaderId = PIPELINE_TYPE::getShaderId();
		PipelineBase::addShaders(app, shaderId, { vertShaderFilename , fragShaderFilename });
		return createPipeline<PIPELINE_TYPE>(app);
	}

	template<class UBO_TYPE, class VERT_TYPE>
	inline Pipeline<UBO_TYPE, VERT_TYPE>::Pipeline(const VulkanAppPtr& app)
	: PipelineBase(app)
	{ 
		_vertBindDesc = VERT_TYPE::getBindingDescription();
		_vertAttribDesc = VERT_TYPE::getAttributeDescriptions();
	}

	template<class UBO_TYPE, class VERT_TYPE>
	inline void Pipeline<UBO_TYPE, VERT_TYPE>::addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const {
		for (const auto& sceneNode : _sceneNodes)
			sceneNode->addCommands(cmdBuff, _pipelineLayout, _descriptorSets[swapChainIdx]);
	}


	template<class UBO_TYPE, class VERT_TYPE>
	inline void Pipeline<UBO_TYPE, VERT_TYPE>::addSceneNode(const SceneNodePtr& node) {
		_sceneNodes.push_back(node);
	}

	template<class UBO_TYPE, class VERT_TYPE>
	inline size_t Pipeline<UBO_TYPE, VERT_TYPE>::numSceneNodes() const {
		return _sceneNodes.size();
	}

}
