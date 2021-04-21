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

#include <vector>
#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vk_vertexTypes.h>
#include <vk_sceneNode.h>
#include <vk_uniformBuffers.h>
#include <vk_pipelineImageProc.h>

namespace VK {

	template<class UBO_TYPE>
	class SceneNodeImageProcTmpl : public PipelineImageProcTmpl<UBO_TYPE>::SceneNode {
	public:
		SceneNodeImageProcTmpl(const VulkanAppPtr& app, const TextureImagePtr& srcImage);

		void addCommands(VkCommandBuffer cmdBuff) const override;
		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const override;

	private:
		Buffer _vertexBuffer, _indexBuffer;
		TextureImagePtr _srcImage;
	};


	template<class UBO_TYPE>
	SceneNodeImageProcTmpl<UBO_TYPE>::SceneNodeImageProcTmpl(const VulkanAppPtr& app, const TextureImagePtr& srcImage)
		: PipelineImageProc::SceneNode(app)
		, _vertexBuffer(app->getDeviceContext())
		, _indexBuffer(app->getDeviceContext())
		, _srcImage(srcImage)
	{
		// This should be a class local define or using clause, but it won't compile in gcc.
		std::vector<Vertex2_PTf> vertices = {
			{{-1, -1}, {0, 0} },
			{{ 1, -1}, {1, 0}},
			{{ 1,  1}, {1, 1}},
			{{-1,  1}, {0, 1}},
		};

		_vertexBuffer.create(vertices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		std::vector<uint32_t> indices = {
			0,1,2,
			0,2,3
		};

		_indexBuffer.create(indices, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}

	template<class UBO_TYPE>
	void SceneNodeImageProcTmpl<UBO_TYPE>::buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const {
		imageInfoList.clear();

		VkDescriptorImageInfo imageInfo = _srcImage->getDescriptor();
		imageInfoList.push_back(imageInfo);

	}

	template<class UBO_TYPE>
	void SceneNodeImageProcTmpl<UBO_TYPE>::addCommands(VkCommandBuffer cmdBuff) const {
		VkBuffer vertexBuffers[] = { _vertexBuffer };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(cmdBuff, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(cmdBuff, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmdBuff, 6, 1, 0, 0, 0);
	}

	using SceneNodeImageProc = SceneNodeImageProcTmpl<UBOImageProc>;
	using SceneNodeImageProcPtr = std::shared_ptr<SceneNodeImageProc>;
	using SceneNodeImageProcConstPtr = std::shared_ptr<const SceneNodeImageProc>;

}
