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

#include <vk_offscreenSurfaceBase.h>

namespace VK {

	/*
	OffscreenSurface and OffscreenSurfaceBase are NOT surfaces. They are collection of images, frame buffers, renderers etc. that fulfill the role of a surface
	for offscreen rendering.

	*/

	template<class UBO_TYPE>
	class OffscreenSurface : public OffscreenSurfaceBase {
	public:
		using UboType = UBO_TYPE;
		using PipelineGroupType = PipelineUboGroup<UBO_TYPE>;
		using PipelineGroupTypePtr = PipelineUboGroupPtr<UBO_TYPE>;
		using PipelinePtr = typename PipelineGroupType::PipelinePtr;
		using PointerType = std::shared_ptr<OffscreenSurface>;

		OffscreenSurface(const VulkanAppPtr& app, VkFormat colorFormat);
		~OffscreenSurface();

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
	inline OffscreenSurface<UBO_TYPE>::OffscreenSurface(const VulkanAppPtr& app, VkFormat colorFormat)
		: OffscreenSurfaceBase(app, colorFormat)
	{
		_pipelines = std::make_shared<PipelineGroupType>(app, 1);
	}

	template<class UBO_TYPE>
	inline OffscreenSurface<UBO_TYPE>::~OffscreenSurface() {
	}

	template<class UBO_TYPE>
	template<typename FUNC_TYPE>
	inline void OffscreenSurface<UBO_TYPE>::setUboUpdateFunction(FUNC_TYPE f) {
		_updateUbo = f;
	}

	template<class UBO_TYPE>
	inline bool OffscreenSurface<UBO_TYPE>::updateUbo() {
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
	inline void OffscreenSurface<UBO_TYPE>::setAntiAliasSamples(VkSampleCountFlagBits samples) {
		_pipelines->setAntiAliasSamples(samples);
	}

	template<class UBO_TYPE>
	inline void OffscreenSurface<UBO_TYPE>::cleanupSwapChain() {
		_pipelines->iterate([](const OffscreenSurface::PipelinePtr& pl) {
			pl->cleanupSwapChain();
		});
	}

	template<class UBO_TYPE>
	inline void OffscreenSurface<UBO_TYPE>::build() {
		_pipelines->iterate([](const OffscreenSurface::PipelinePtr& pl) {
			if (pl->isVisible())
				pl->build();
		});
	}
	template<class UBO_TYPE>
	void OffscreenSurface<UBO_TYPE>::draw(VkCommandBuffer cmdBuff) {
		_pipelines->iterate([cmdBuff](const OffscreenSurface::PipelinePtr& pl) {
			if (pl->isVisible())
				pl->draw(cmdBuff, 0);
		});
	}

	template<class UBO_TYPE>
	inline void OffscreenSurface<UBO_TYPE>::setUbo(const UBO_TYPE& ubo) {
		_pipelines->setUbo(ubo, 0);
	}

	template<class UBO_TYPE>
	const UBO_TYPE& OffscreenSurface<UBO_TYPE>::getUbo() const {
		return _pipelines->getUbo();
	}

	template<class UBO_TYPE>
	inline VkRenderPass OffscreenSurface<UBO_TYPE>::getRenderPass() const {
		return _pipelines->getRenderPass();
	}

	template<class UBO_TYPE>
	VkSampleCountFlagBits OffscreenSurface<UBO_TYPE>::getAntiAliasSamples() const {
		return _pipelines->getAntiAliasSamples();
	}

	template<class UBO_TYPE>
	inline void OffscreenSurface<UBO_TYPE>::setRenderPass(VkRenderPass renderPass) {
		_pipelines->setRenderPass(renderPass);
	}

	template<class UBO_TYPE>
        inline typename OffscreenSurface<UBO_TYPE>::PipelineGroupTypePtr& OffscreenSurface<UBO_TYPE>::getPipelines() {
            return _pipelines;
        }

	template<class UBO_TYPE>
        inline const typename OffscreenSurface<UBO_TYPE>::PipelineGroupTypePtr& OffscreenSurface<UBO_TYPE>::getPipelines() const {
            return _pipelines;
        }

    using OffscreenSurface3D = OffscreenSurface<UniformBufferObject3D>;
    using OffscreenSurface3DPtr = typename OffscreenSurface3D::PointerType;

}
