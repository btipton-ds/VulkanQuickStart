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

#include <mutex>
#include <vector>
#include <memory>

#include <vk_forwardDeclarations.h>

#include <vulkan/vulkan_core.h>

#include <vk_pipelineUboGroupBase.h>

namespace VK {

	template<class UBO_TYPE>
	class PipelineUboGroup : public PipelineUboGroupBase {
		/*
		This class contains a list of pipelines which share a common uniform buffer object and related descriptors.
		*/
	public:
		using UboType = UBO_TYPE;
		using Pipeline = VK::PipelineUbo<UBO_TYPE>;
		using PipelinePtr = std::shared_ptr<Pipeline>;

		PipelineUboGroup(const VulkanAppPtr& app);

		void add(const PipelinePtr& pl);
		void resized(const VkRect2D& rect);

		const UboType& getUbo() const;
		void setUbo(const UboType& ubo, uint32_t swapChainImageIndex);

		template<typename FUNC_TYPE>
		void iterate(FUNC_TYPE func);

		void cleanupSwapChain();
		const PipelinePtr& getPipeline(size_t idx) const;

	private:
		static bool PipelineComparePaintLayer(const PipelinePtr& pl1, const PipelinePtr& pl2);

		std::vector<PipelinePtr> _pipelines;
		UboType _ubo;
	};

	template<class UBO_TYPE>
	inline PipelineUboGroup<UBO_TYPE>::PipelineUboGroup(const VulkanAppPtr& app)
		: PipelineUboGroupBase(app)
	{}

	template<class UBO_TYPE>
	bool PipelineUboGroup<UBO_TYPE>::PipelineComparePaintLayer(const PipelinePtr& pl1, const PipelinePtr& pl2) {
		return pl1->getPaintLayer() < pl2->getPaintLayer();
	}

	template<class UBO_TYPE>
	void PipelineUboGroup<UBO_TYPE>::add(const PipelinePtr& pl) {
		std::lock_guard lg(_mutex);
		pl->setUniformBufferPtr(&_ubo);
		_pipelines.push_back(pl);
		std::sort(_pipelines.begin(), _pipelines.end(), PipelineComparePaintLayer);
	}

	template<class UBO_TYPE>
	void PipelineUboGroup<UBO_TYPE>::resized(const VkRect2D& rect) {
		iterate([&](const auto& pipeline) {
			pipeline->setViewportRect(rect);
			pipeline->setScissorRect(rect);
		});
	}

	template<class UBO_TYPE>
	const typename PipelineUboGroup<UBO_TYPE>::UboType& PipelineUboGroup<UBO_TYPE>::getUbo() const {
		return _ubo;
	}

	template<class UBO_TYPE>
	void PipelineUboGroup<UBO_TYPE>::setUbo(const UboType& ubo, uint32_t swapChainImageIndex) {
		_ubo = ubo;
		iterate([&](const PipelinePtr& pipeline) {
			if (pipeline->isVisible() && pipeline->numSceneNodes() > 0) {
				pipeline->updateUniformBuffers(swapChainImageIndex);
			}
		});
	}

	template<class UBO_TYPE>
	template<typename FUNC_TYPE>
	inline void PipelineUboGroup<UBO_TYPE>::iterate(FUNC_TYPE func) {
		std::lock_guard lg(_mutex);
		for (auto& pl : _pipelines) {
			func(pl);
		}
	}

	template<class UBO_TYPE>
	inline void PipelineUboGroup<UBO_TYPE>::cleanupSwapChain() {
		iterate([&](const PipelinePtr& pipeline) {
			pipeline->cleanupSwapChain();
		});
	}

	template<class UBO_TYPE>
	inline const typename PipelineUboGroup<UBO_TYPE>::PipelinePtr& PipelineUboGroup<UBO_TYPE>::getPipeline(size_t idx) const {
		return _pipelines[idx];
	}
}
