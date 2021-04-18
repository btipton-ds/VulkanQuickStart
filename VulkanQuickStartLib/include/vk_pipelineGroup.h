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

namespace VK {


	template<class PL_TYPE>
	class PipelineGroup {
		/*
		PipelineGroup allows a set of pipelines to be treated as a group.
		Handy for when you render the 'same pipeline' to multiple targets which require the pipeline to be specialized into multiple parts.
		*/
	public:
		inline void add(const PL_TYPE& pl) {
			_pipelines.push_back(pl);
		}

		template<class T>
		inline void addSceneNode(const T& n) {
			for (auto& pl : _pipelines) {
				pl->addSceneNode(n);
			}
		}

		inline void toggleVisiblity() {
			for (auto& pl : _pipelines) {
				pl->toggleVisiblity();
			}
		}

		inline void setPolygonMode(VkPolygonMode mode) {
			for (auto& pl : _pipelines) {
				pl->setPolygonMode(mode);
			}
		}

	private:
		std::vector<PL_TYPE> _pipelines;
	};

}
