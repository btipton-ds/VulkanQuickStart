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

#include <tm_boundingBox.h>
#include <vk_pipeline.h>
#include <vk_uniformBuffers.h>
#include <vk_vertexTypes.h>

namespace VK {
	class Pipeline3DWSampler;
	using Pipeline3DWSamplerPtr = std::shared_ptr<class Pipeline3DWSampler>;

	class Pipeline3DWSampler : public Pipeline<Vertex3_PNCTf, UniformBufferObject3D> {
	public:
		static uint32_t getMaxSamplers();
		using UniformBufferObject = UniformBufferObject3D;
		using BoundingBox = CBoundingBox3D<float>;
		using PipelinePtr = std::shared_ptr<Pipeline3DWSampler>;

		static std::string getShaderId();
		Pipeline3DWSampler(const VulkanAppPtr& app);

		void setUniformBufferPtr(const UniformBufferObject* ubo);
		BoundingBox getBounds() const;

		void addCommands(VkCommandBuffer cmdBuff, size_t swapChainIdx) const override;

		const UniformBufferObject& getUniformBuffer() const;

	protected:
		std::string getShaderIdMethod() override;
		virtual void createDescriptorSetLayout() override;

		const UniformBufferObject* _ubo;

	};

	inline uint32_t Pipeline3DWSampler::getMaxSamplers() {
		return 7; 
	};

	inline void Pipeline3DWSampler::setUniformBufferPtr(const UniformBufferObject* ubo) {
		_ubo = ubo;
	}


	inline const Pipeline3DWSampler::UniformBufferObject& Pipeline3DWSampler::getUniformBuffer() const {
		return *_ubo;
	}

}