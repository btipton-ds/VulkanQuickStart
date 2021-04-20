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
#include <memory>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <vk_forwardDeclarations.h>
#include "vk_deviceContext.h"
#include "vk_buffer.h"
#include "vk_textureImage.h"
#include "vk_vertexTypes.h"
#include <vk_sceneNodePNC3f.h>

#include <tm_boundingBox.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS ModelPNC3f : public SceneNodePNC3f {
	public:
		using BoundingBox = CBoundingBox3D<float>;

		static inline ModelPNC3fPtr create(const VulkanAppPtr& app, const TriMesh::CMeshPtr& meshPtr, const glm::vec3& color = glm::vec3(1, 1, 1)) {
			return std::shared_ptr<ModelPNC3f>(new ModelPNC3f(app, meshPtr, color));
		}

		void addCommands(VkCommandBuffer cmdBuff) const override;
		void buildImageInfoList(std::vector<VkDescriptorImageInfo>& imageInfoList) const override;
		BoundingBox getBounds() const override;

		inline const std::vector<VertexType>& getVertices() const {
			return _vertices;
		}

		inline const std::vector<uint32_t>& getIndices() const {
			return _indices;
		}

		inline const Buffer& getVertexBuffer() const {
			return _vertexBuffer;
		}

		inline const Buffer& getIndexBuffer() const {
			return _indexBuffer;
		}

		inline uint32_t numIndices() const {
			return static_cast<uint32_t>(_indices.size());
		}

	protected:
		ModelPNC3f(const VulkanAppPtr& app);
		ModelPNC3f(const VulkanAppPtr& app, const TriMesh::CMeshPtr& meshPtr, const glm::vec3& color);

		void loadModel(const TriMesh::CMeshPtr& meshPtr, const glm::vec3& color);

		void createVertexBuffer();
		void createIndexBuffer();

		BoundingBox _bounds;
		std::vector<VertexType> _vertices;
		std::vector<uint32_t> _indices;
		Buffer _vertexBuffer, _indexBuffer;
	};

}

#pragma warning (pop)
