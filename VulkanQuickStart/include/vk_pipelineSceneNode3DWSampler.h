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

#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vk_pipelineSceneNode.h>
#include <vk_pipeline3DWithSampler.h>

namespace VK {

	class PipelineSceneNode3DWSampler : public Pipeline3DWSampler::PipelineSceneNode {
	public:
		using BoundingBox = Pipeline3DWSampler::BoundingBox;
		using UniformBufferObject = Pipeline3DWSampler::UniformBufferObject;
		using XformFuncType = std::function<glm::mat4(const glm::mat4& src)>;

		PipelineSceneNode3DWSampler(const PipelineBasePtr& ownerPipeline);
		virtual ~PipelineSceneNode3DWSampler();

		virtual BoundingBox getBounds() const = 0;
		void updateUniformBuffer(size_t swapChainIndex) override;

		void setModelTransform(const glm::mat4& xform);
		const glm::mat4& getModelTransform() const;
		glm::mat4& getModelTransform();

		template<typename FUNC_TYPE>
		void setModelTransformFunc(FUNC_TYPE func);

	private:
		glm::mat4 _modelXForm;
		XformFuncType _modelXFormFunc;
	};

	using PipelineSceneNode3DWSamplerPtr = std::shared_ptr<PipelineSceneNode3DWSampler>;

	using SceneNode3DWithTexturePtr = std::shared_ptr<PipelineSceneNode3DWSampler>;

	inline void PipelineSceneNode3DWSampler::setModelTransform(const glm::mat4& xform) {
		_modelXForm = xform;
	}

	inline const glm::mat4& PipelineSceneNode3DWSampler::getModelTransform() const {
		return _modelXForm;
	}

	inline glm::mat4& PipelineSceneNode3DWSampler::getModelTransform() {
		return _modelXForm;
	}

	template<typename FUNC_TYPE>
	inline void PipelineSceneNode3DWSampler::setModelTransformFunc(FUNC_TYPE func) {
		_modelXFormFunc = func;
	}
}
