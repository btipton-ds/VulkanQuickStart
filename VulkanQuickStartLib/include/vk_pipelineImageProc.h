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

#include <vk_logger.h>
#include <tm_boundingBox.h>
#include <vk_pipeline.h>
#include <vk_uniformBuffers.h>
#include <vk_vertexTypes.h>
#include <vk_initializers.h>

namespace VK {

	template<class UBO_TYPE>
	class PipelineImageProcTmpl : public Pipeline<Vertex2_PTf, UBO_TYPE> {
	public:
		using BoundingBox = CBoundingBox3D<float>;
		using PipelinePtr = std::shared_ptr<PipelineImageProcTmpl>;

		PipelineImageProcTmpl(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect);

		BoundingBox getBounds() const;

		void updateSceneNodeUbo(const typename Pipeline<Vertex2_PTf, UBO_TYPE>::SceneNodePtr& sceneNode, UBO_TYPE& ubo) const override;
	protected:
		virtual void createDescriptorSetLayout() override;
	};

	template<class UBO_TYPE>
	PipelineImageProcTmpl<UBO_TYPE>::PipelineImageProcTmpl(const PipelineUboGroupBasePtr& plGroup, const std::string& shaderId, const VkRect2D& rect)
		: Pipeline<Vertex2_PTf, UBO_TYPE>(plGroup, shaderId, rect)
	{}

	template<class UBO_TYPE>
	void PipelineImageProcTmpl<UBO_TYPE>::updateSceneNodeUbo(const typename Pipeline<Vertex2_PTf, UBO_TYPE>::SceneNodePtr& sceneNode, UBO_TYPE& ubo) const {
	}

	template<class UBO_TYPE>
	void PipelineImageProcTmpl<UBO_TYPE>::createDescriptorSetLayout() {
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
		uint32_t bindingIdx = 0;

		setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, bindingIdx++));
		setLayoutBindings.push_back(initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, bindingIdx++));

		VkDescriptorSetLayoutCreateInfo descriptorLayout = initializers::descriptorSetLayoutCreateInfo(setLayoutBindings);
		if (vkCreateDescriptorSetLayout(PipelineBase::_dc->_device, &descriptorLayout, nullptr, &(PipelineBase::_descriptorSetLayout)) != VK_SUCCESS) {
			THROW("failed to create descriptor set layout!");
		}
	}

	template<class UBO_TYPE>
	using PipelineImageProcTmplPtr = std::shared_ptr<PipelineImageProcTmpl<UBO_TYPE>>;

	using PipelineImageProc = PipelineImageProcTmpl<UBOImageProc>;
	using PipelineImageProcPtr = std::shared_ptr<PipelineImageProc>;

}
