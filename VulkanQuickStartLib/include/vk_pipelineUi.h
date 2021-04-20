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

#include <vk_pipeline.h>
#include <vk_vertexTypes.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	struct UniformBufferObjectUi {
		glm::vec2 _offset;
		glm::vec2 _scale;
		glm::vec4 _color;
	};

	struct EXPORT_VQS VertexUi {
		VertexUi() = default;
		VertexUi(const VertexUi& src) = default;
		VertexUi(const glm::vec2& pos, const glm::vec2& tc);

		bool operator==(const VertexUi& other) const;
		static VkVertexInputBindingDescription getBindingDescription();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		glm::vec2 _pos;
		glm::vec2 _texCoord;
	};

	class EXPORT_VQS PipelineUi : public Pipeline<VertexUi, UniformBufferObjectUi> {
	public:
		using PipelinePtr = std::shared_ptr<PipelineUi>;
		using VertexType = VertexUi;

		PipelineUi(const PipelineUboGroupBasePtr& plGroup);
		void updateSceneNodeUbo(const SceneNodePtr& sceneNode, UniformBufferObjectUi& ubo) const override;

	protected:
		virtual void createDescriptorSetLayout() override;
	};

	using PipelineUiPtr = std::shared_ptr<PipelineUi>;
	
	inline VertexUi::VertexUi(const glm::vec2& pos, const glm::vec2& tc)
		: _pos(pos)
		, _texCoord(tc)
	{ }

}

#pragma warning (pop)
