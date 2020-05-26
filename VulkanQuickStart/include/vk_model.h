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

#include <vector>
#include <memory>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include "vk_device_context.h"
#include "vk_buffer.h"
#include "vk_texture_image.h"
#include "vk_vertex_types.h"
#include <vk_sceneNode3D.h>
#include <vk_pipeline3D.h>

#include <boundingBox.h>

namespace TriMesh {
	class CMesh;
	using CMeshPtr = std::shared_ptr<CMesh>;
}

namespace VK {

	struct DeviceContext;
	class Model;
	using ModelPtr = std::shared_ptr<Model>;

	class Model : public SceneNode3D {
	public:
		using BoundingBox = CBoundingBox3D<float>;

		static inline ModelPtr create(DeviceContext& dc, const TriMesh::CMeshPtr& meshPtr) {
			return std::shared_ptr<Model>(new Model(dc, meshPtr));
		}

		void addCommands(VkCommandBuffer cmdBuff, VkPipelineLayout pipelineLayout, const VkDescriptorSet& descSet) const override;
		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const override;
		BoundingBox getBounds() const override;

		inline const std::vector<Vertex3_PNCTf>& getVertices() const {
			return vertices_;
		}

		inline const std::vector<uint32_t>& getIndices() const {
			return indices_;
		}

		inline const Buffer& getVertexBuffer() const {
			return vertexBuffer_;
		}

		inline const Buffer& getIndexBuffer() const {
			return indexBuffer_;
		}

		inline uint32_t numIndices() const {
			return static_cast<uint32_t>(indices_.size());
		}

	protected:
		Model(DeviceContext& dc);
		Model(DeviceContext& dc, const TriMesh::CMeshPtr& meshPtr);

		void loadModel(const TriMesh::CMeshPtr& meshPtr);

		void createVertexBuffer();
		void createIndexBuffer();

		DeviceContext* _dc = VK_NULL_HANDLE; // TODO I think this should be a parameter, not a member
		BoundingBox _bounds;
		std::vector<Vertex3_PNCTf> vertices_;
		std::vector<uint32_t> indices_;
		Buffer vertexBuffer_, indexBuffer_;
	};

}
