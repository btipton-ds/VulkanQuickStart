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

#include <boundingBox.h>
#include <vk_pipeline.h>

namespace VK {

	struct UniformBufferObject3D {
		alignas(16) float ambient;

		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) int numLights;
		alignas(16) glm::vec3 lightDir[2];
	};

	class PipelineVertex3D : public Pipeline<UniformBufferObject3D> {
	public:
		using UniformBufferObject = UniformBufferObject3D;
		using BoundingBox = CBoundingBox3D<float>;


		static std::string getShaderId();

		PipelineVertex3D(VulkanApp* app);
		void updateUniformBuffer(size_t swapChainIndex) override;

		const UniformBufferObject& getUniformBuffer() const;
		UniformBufferObject& getUniformBuffer();

	protected:
		std::string getShaderIdMethod() override;
		virtual void createDescriptorSetLayout() override;
		virtual void createDescriptorSets() override;
		virtual void createUniformBuffers() override;

		UniformBufferObject _ubo;
	};

	inline const PipelineVertex3D::UniformBufferObject& PipelineVertex3D::getUniformBuffer() const {
		return _ubo;
	}

	inline PipelineVertex3D::UniformBufferObject& PipelineVertex3D::getUniformBuffer() {
		return _ubo;
	}

	class PipelineVertex3DWSampler : public PipelineVertex3D {
	public:
		static std::string getShaderId();
		PipelineVertex3DWSampler(VulkanApp* app);

	protected:
		std::string getShaderIdMethod() override;
		virtual void createDescriptorSetLayout() override;
		virtual void createDescriptorSets() override;
	};



}