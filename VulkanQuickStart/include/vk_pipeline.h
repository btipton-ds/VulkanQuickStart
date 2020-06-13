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
#include <vk_pipelineSceneNode.h>

namespace VK {

	template<class UBO_TYPE>
	class PipelineUbo : public PipelineBase {
	public:
		using UboType = UBO_TYPE;

		PipelineUbo(const VulkanAppPtr& app, const std::string& shaderId, const VkRect2D& rect)
			: PipelineBase(app, shaderId, rect) 
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
		using PipelineSceneNode = GCC_CLASS PipelineSceneNode<Pipeline<VERT_TYPE, UBO_TYPE>>;
		using SceneNodePtr = std::shared_ptr<PipelineSceneNode>;
		using SceneNodeList = std::vector<SceneNodePtr>;

		Pipeline(const VulkanAppPtr& app, const std::string& shaderId, const VkRect2D& rect);

		void cleanupSwapChain() override;
		void addSceneNode(const SceneNodePtr& node);
		void removeSceneNode(const SceneNodePtr& node);
		size_t numSceneNodes() const override;

		void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const override;

		void updateUniformBuffers(size_t swapChainIndex) override;

		size_t getUboSize() const override;

	protected:
		void buildSceneNodes() override;

		SceneNodeList _sceneNodes;
	};

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipeline(const VulkanAppPtr& app, const std::string& shaderId, const VkRect2D& rect) {
		PIPELINE_TYPE* ptr = new PIPELINE_TYPE(app, shaderId, rect);
		return PipelinePtr<PIPELINE_TYPE>(ptr);
	}

	template<class PIPELINE_TYPE>
	inline typename PIPELINE_TYPE::PipelinePtr createPipelineWithSource(const VulkanAppPtr& app, const std::string& shaderId, const VkRect2D& rect, const std::string& vertShaderFilename, const std::string& fragShaderFilename) {
		auto pipeline = createPipeline<PIPELINE_TYPE>(app, shaderId, rect);
		auto& shaders = app->getDeviceContext()->getShaderPool();
		if (!shaders.getShader(shaderId))
			shaders.addShader(shaderId, { vertShaderFilename , fragShaderFilename });
		return pipeline;
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline Pipeline<VERT_TYPE, UBO_TYPE>::Pipeline(const VulkanAppPtr& app, const std::string& shaderId, const VkRect2D& rect)
	: PipelineUbo(app, shaderId, rect)
	{ 
		_vertBindDesc = VERT_TYPE::getBindingDescription();
		_vertAttribDesc = VERT_TYPE::getAttributeDescriptions();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::cleanupSwapChain() {
		PipelineUbo::cleanupSwapChain();
		for (auto& sceneNode : _sceneNodes) {
			if (sceneNode->isReady())
				sceneNode->cleanupSwapChain();
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const {
		for (const auto& sceneNode : _sceneNodes) {
			if (sceneNode->isReady() && sceneNode->isVisible())
				sceneNode->addCommands(cmdBuff, _pipelineLayout, swapChainIdx);
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline size_t Pipeline<VERT_TYPE, UBO_TYPE>::getUboSize() const {
		return sizeof(UBO_TYPE);
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::buildSceneNodes() {
		for (const auto& sceneNode : _sceneNodes) {
			sceneNode->createDescriptorPool();
			sceneNode->createUniformBuffers();
			sceneNode->createDescriptorSets();
			sceneNode->setReady(true);
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::addSceneNode(const SceneNodePtr& node) {
		_sceneNodes.push_back(node);
		changed();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::removeSceneNode(const SceneNodePtr& node) {
		auto iter = std::find(_sceneNodes.begin(), _sceneNodes.end(), node);
		if (iter != _sceneNodes.end()) {
			_sceneNodes.erase(iter);
			changed();
		}
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline size_t Pipeline<VERT_TYPE, UBO_TYPE>::numSceneNodes() const {
		return _sceneNodes.size();
	}

	template<class VERT_TYPE, class UBO_TYPE>
	inline void Pipeline<VERT_TYPE, UBO_TYPE>::updateUniformBuffers(size_t swapChainIndex) {
		for (auto& sceneNode : _sceneNodes) {
			if (sceneNode->isReady())
				sceneNode->updateUniformBuffer(swapChainIndex);
		}
	}

}
