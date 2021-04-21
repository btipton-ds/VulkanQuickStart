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

#include <vk_forwardDeclarations.h>
#include <vk_buffer.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS ComputeStepBase {
		// Uses a compute shader t process one image to another
	public:
		ComputeStepBase(const DeviceContextPtr& dc, const TextureImagePtr& srcImage, const TextureImagePtr& dstImage, const std::string& shaderId, size_t uboSize = 0);

		void setLocalDim(int localDim);
		void setPriorStep(const ComputeStepBasePtr& prior);

		void build();
		void submitCommands();

		const TextureImagePtr& getResultImage() const;

		virtual void updateUbo();

	protected:

		void createComputeQueue();
		void createDescriptorPool();
		void createUniformBuffers();
		void buildComputeCommandBuffer();
		void createCompute();
		void waitForFence() const;

		uint32_t _queueFamilyIndex = 0;
		size_t _uboSize = 0;

		DeviceContextPtr _dc;

		ComputeStepBasePtr _prior;
		TextureImagePtr _srcImage, _dstImage;
		std::string _shaderId;
		BufferPtr _uboBuf;

		VkQueue _queue = VK_NULL_HANDLE;
		int _localDim = 16;
		VkCommandBuffer _cmdBuf = VK_NULL_HANDLE;
		VkFence _fence = VK_NULL_HANDLE;

		VkCommandPool _commandPool = VK_NULL_HANDLE;
		VkDescriptorPool _descriptorPool;
		VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorSet _descriptorSet = VK_NULL_HANDLE;
		VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
		VkPipeline _pipeline = VK_NULL_HANDLE;
	};

	inline void ComputeStepBase::setLocalDim(int localDim) {
		_localDim = localDim;
	}

	inline void ComputeStepBase::setPriorStep(const ComputeStepBasePtr& prior) {
		_prior = prior;
	}

	inline const TextureImagePtr& ComputeStepBase::getResultImage() const {
		return _dstImage;
	}

	template<class UBO_TYPE>
	class ComputeStep : public ComputeStepBase {
	public:
		using UboType = UBO_TYPE;
		using PointerType = std::shared_ptr<ComputeStep<UBO_TYPE>>;
		inline static std::shared_ptr<ComputeStep<UBO_TYPE>> newPtr(const DeviceContextPtr& dc, const TextureImagePtr& srcImage,
			const TextureImagePtr& dstImage, const std::string& shaderId, const UBO_TYPE* ubo) {
			auto p = std::shared_ptr<ComputeStep<UBO_TYPE>>(new ComputeStep<UBO_TYPE>(dc, srcImage, dstImage, shaderId, ubo));
			return p;
		}

		void updateUbo() override {
			_uboBuf->update(*_ubo);
		}

	private:
		inline ComputeStep(const DeviceContextPtr& dc, const TextureImagePtr& srcImage, const TextureImagePtr& dstImage, const std::string& shaderId, const UBO_TYPE* ubo)
			: ComputeStepBase(dc, srcImage, dstImage, shaderId, sizeof(UboType))
			, _ubo(ubo)
		{
		}

		const UboType* _ubo;
	};

}

#pragma warning (pop)
