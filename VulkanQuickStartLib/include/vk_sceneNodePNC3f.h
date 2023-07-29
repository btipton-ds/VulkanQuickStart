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

#include <tm_boundingBox.h>
#include <vk_vertexTypes.h>
#include <vk_sceneNode.h>
#include <vk_uniformBuffers.h>
#include <vk_transformFunc.h>

namespace VK {

	class EXPORT_VQS SceneNodePNC3f : public SceneNode<Vertex3_PNCf> {
	public:
		using BoundingBox = CBoundingBox3Df;

		SceneNodePNC3f(const VulkanAppPtr& app);
		virtual ~SceneNodePNC3f();

		virtual BoundingBox getBounds() const = 0;

		void setModelTransform(const glm::mat4& xform);
		const glm::mat4& getModelTransform() const;
		glm::mat4& getModelTransform();

		void setModelTransformFunc(const TransformFuncPtr& func);

		template<class UBO_TYPE>
		inline void updateUbo(UBO_TYPE& ubo) const {
			glm::mat4 xform(1);
			if (_modelXFormFunc && _modelXFormFunc->update(xform))
				ubo.modelView *= _modelXForm * xform;
			else
				ubo.modelView *= _modelXForm;
		}

	private:
		glm::mat4 _modelXForm;
		TransformFuncPtr _modelXFormFunc;
	};
	using SceneNodePNC3fPtr = std::shared_ptr<SceneNodePNC3f>;
	using SceneNodePNC3fConstPtr = std::shared_ptr<const SceneNodePNC3f>;

	inline void SceneNodePNC3f::setModelTransform(const glm::mat4& xform) {
		_modelXForm = xform;
	}

	inline const glm::mat4& SceneNodePNC3f::getModelTransform() const {
		return _modelXForm;
	}

	inline glm::mat4& SceneNodePNC3f::getModelTransform() {
		return _modelXForm;
	}

	inline void SceneNodePNC3f::setModelTransformFunc(const TransformFuncPtr& func) {
		_modelXFormFunc = func;
	}

}

