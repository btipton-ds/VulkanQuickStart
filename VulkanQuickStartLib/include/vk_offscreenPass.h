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
#include <functional>

#include <vk_forwardDeclarations.h>

#include <vk_offscreenPassBase.h>

namespace VK {

	template<class UBO_TYPE>
	class OffscreenPass : public OffscreenPassBase {
	public:
		using UboType = UBO_TYPE;
		using PipelineGroupType = PipelineUboGroup<UBO_TYPE>;
		using PipelineGroupTypePtr = PipelineUboGroupPtr<UBO_TYPE>;
		using PipelinePtr = typename PipelineGroupType::PipelinePtr;
		using PointerType = std::shared_ptr<OffscreenPass>;

		OffscreenPass(const VulkanAppPtr& app, VkFormat colorFormat);
		~OffscreenPass();

		VkRenderPass getRenderPass() const override;

		template<typename FUNC_TYPE>
		void setUboUpdateFunction(FUNC_TYPE f);
		bool updateUbo() override;

		void setUbo(const UBO_TYPE& ubo);
		const UBO_TYPE& getUbo() const;
		void setAntiAliasSamples(VkSampleCountFlagBits samples);

		void cleanupSwapChain() override;
		void build() override;

		void draw(VkCommandBuffer cmdBuff) override;

                PipelineGroupTypePtr& getPipelines();
                const PipelineGroupTypePtr& getPipelines() const;

	protected:
		VkSampleCountFlagBits getAntiAliasSamples() const override;
		inline void setRenderPass(VkRenderPass renderPass) override;

	private:
		UpdateUboFunctionType<UBO_TYPE> _updateUbo;
		PipelineGroupTypePtr _pipelines;
	};

	template<class UBO_TYPE>
	inline OffscreenPass<UBO_TYPE>::OffscreenPass(const VulkanAppPtr& app, VkFormat colorFormat)
		: OffscreenPassBase(app, colorFormat)
	{
		_pipelines = std::make_shared<PipelineGroupType>(app, 1);
	}

	template<class UBO_TYPE>
	inline OffscreenPass<UBO_TYPE>::~OffscreenPass() {
	}

	template<class UBO_TYPE>
	template<typename FUNC_TYPE>
	inline void OffscreenPass<UBO_TYPE>::setUboUpdateFunction(FUNC_TYPE f) {
		_updateUbo = f;
	}

	template<class UBO_TYPE>
	inline bool OffscreenPass<UBO_TYPE>::updateUbo() {
		UBO_TYPE ubo;
		if (_updateUbo && _updateUbo(_rect.extent.width, _rect.extent.height, ubo)) {
			setUbo(ubo);
			return true;
		} else {
			_pipelines->updateUbos(0);
		}
		return false;
	}

	template<class UBO_TYPE>
	inline void OffscreenPass<UBO_TYPE>::setAntiAliasSamples(VkSampleCountFlagBits samples) {
		_pipelines->setAntiAliasSamples(samples);
	}

	template<class UBO_TYPE>
	inline void OffscreenPass<UBO_TYPE>::cleanupSwapChain() {
		_pipelines->iterate([](const OffscreenPass::PipelinePtr& pl) {
			pl->cleanupSwapChain();
		});
	}

	template<class UBO_TYPE>
	inline void OffscreenPass<UBO_TYPE>::build() {
		_pipelines->iterate([](const OffscreenPass::PipelinePtr& pl) {
			if (pl->isVisible())
				pl->build();
		});
	}
	template<class UBO_TYPE>
	void OffscreenPass<UBO_TYPE>::draw(VkCommandBuffer cmdBuff) {
		_pipelines->iterate([cmdBuff](const OffscreenPass::PipelinePtr& pl) {
			if (pl->isVisible())
				pl->draw(cmdBuff, 0);
		});
	}

	template<class UBO_TYPE>
	inline void OffscreenPass<UBO_TYPE>::setUbo(const UBO_TYPE& ubo) {
		_pipelines->setUbo(ubo, 0);
	}

	template<class UBO_TYPE>
	const UBO_TYPE& OffscreenPass<UBO_TYPE>::getUbo() const {
		return _pipelines->getUbo();
	}

	template<class UBO_TYPE>
	inline VkRenderPass OffscreenPass<UBO_TYPE>::getRenderPass() const {
		return _pipelines->getRenderPass();
	}

	template<class UBO_TYPE>
	VkSampleCountFlagBits OffscreenPass<UBO_TYPE>::getAntiAliasSamples() const {
		return _pipelines->getAntiAliasSamples();
	}

	template<class UBO_TYPE>
	inline void OffscreenPass<UBO_TYPE>::setRenderPass(VkRenderPass renderPass) {
		_pipelines->setRenderPass(renderPass);
	}

	template<class UBO_TYPE>
        inline typename OffscreenPass<UBO_TYPE>::PipelineGroupTypePtr& OffscreenPass<UBO_TYPE>::getPipelines() {
            return _pipelines;
        }

	template<class UBO_TYPE>
        inline const typename OffscreenPass<UBO_TYPE>::PipelineGroupTypePtr& OffscreenPass<UBO_TYPE>::getPipelines() const {
            return _pipelines;
        }

    using OffscreenPass3D = OffscreenPass<UniformBufferObject3D>;
    using OffscreenPass3DPtr = typename OffscreenPass3D::PointerType;

}
