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
#include <vk_sceneNodeBinding.h>

namespace VK {

	template<class UBO_TYPE>
	class PipelineUbo : public PipelineBase {
	public:
		using UboType = UBO_TYPE;

		PipelineUbo(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect)
			: PipelineBase(plGroup, shaderId, rect) 
		{}

		inline void setUniformBufferPtr(const UboType* uboPtr) {
			_ubo = uboPtr;
		}

		inline const UboType& getUniformBuffer() const {
			return *_ubo;
		}

	protected:
		const UboType* _ubo;
	};



	template<class VERT_TYPE, class UBO_TYPE>
	class Pipeline : public PipelineUbo<UBO_TYPE> {
	public:
		using VertexType = VERT_TYPE;
		using SceneNode = GCC_CLASS SceneNode<VERT_TYPE>;
		using SceneNodePtr = std::shared_ptr<SceneNode>;
		using PipelinePtr = std::shared_ptr<Pipeline>;
		using BindingPtr = SceneNodeToPipelineBindingPtr<VERT_TYPE, UBO_TYPE>;

		Pipeline(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect);

		void cleanupSwapChain() override;
		BindingPtr addSceneNode(const SceneNodePtr& node);
		void removeSceneNode(const BindingPtr& node);
		size_t numSceneNodes() const override;

		void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const override;

		void updateUniformBuffers(size_t swapChainIndex) override;
		virtual void updateSceneNodeUbo(const SceneNodePtr& sceneNode, UBO_TYPE& ubo) const = 0;

		size_t getUboSize() const override;

	protected:
		void buildSceneNodeBindings() override {
			for (auto& binding : _sceneNodeBindings) {
				binding->build();
			}
		}

		std::vector<BindingPtr> _sceneNodeBindings;
		//		SceneNodeList _sceneNodeBindings;
	};

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipeline(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect) {
		PIPELINE_TYPE* ptr = new PIPELINE_TYPE(plGroup, shaderId, rect);
		return PipelinePtr<PIPELINE_TYPE>(ptr);
	}

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipelineWithSource(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect, const std::vector<std::string>& filenames) {
		auto pipeline = createPipeline<PIPELINE_TYPE>(plGroup, shaderId, rect);
		auto& shaders = plGroup->getApp()->getDeviceContext()->getShaderPool();
		if (!shaders.getShader(shaderId))
			shaders.addShader(shaderId, filenames);
		return pipeline;
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline Pipeline<VERT_TYPE, UBO_TYPE>::Pipeline(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect)
	: PipelineUbo(plGroup, shaderId, rect)
	{ 
		_vertBindDesc = VERT_TYPE::getBindingDescription();
		_vertAttribDesc = VERT_TYPE::getAttributeDescriptions();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::cleanupSwapChain() {
		PipelineUbo::cleanupSwapChain();
		for (auto& binding : _sceneNodeBindings) {
			if (binding->isReady())
				binding->cleanup();
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const {
		for (const auto& binding : _sceneNodeBindings) {
			if (binding->isReady() && binding->isVisible())
				binding->addCommands(cmdBuff, _pipelineLayout, swapChainIdx);
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline size_t Pipeline<VERT_TYPE, UBO_TYPE>::getUboSize() const {
		return sizeof(UBO_TYPE);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline typename Pipeline<VERT_TYPE, UBO_TYPE>::BindingPtr Pipeline<VERT_TYPE, UBO_TYPE>::addSceneNode(const SceneNodePtr& node) {
		auto binding = std::make_shared<SceneNodeToPipelineBinding<VERT_TYPE, UBO_TYPE>>(this, node);
		_sceneNodeBindings.push_back(binding);
		changed();
		return binding;
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::removeSceneNode(const BindingPtr& node) {
		auto iter = std::find(_sceneNodeBindings.begin(), _sceneNodeBindings.end(), node);
		if (iter != _sceneNodeBindings.end()) {
			_sceneNodeBindings.erase(iter);
			changed();
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline size_t Pipeline<VERT_TYPE, UBO_TYPE>::numSceneNodes() const {
		return _sceneNodeBindings.size();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::updateUniformBuffers(size_t swapChainIndex) {
		for (auto& binding : _sceneNodeBindings) {
			if (binding->isReady())
				binding->updateUniformBuffer(swapChainIndex, *_ubo);
		}
	}

}
