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

#include <algorithm>

#include <vk_pipelineList.h>
#include <vk_pipelineBase.h>

using namespace std;
using namespace VK;

bool PipelineComparePaintLayer(const PipelineBasePtr& pl1, const PipelineBasePtr& pl2) {
	return pl1->getPaintLayer() < pl2->getPaintLayer();
}

void PipelineList::add(const PipelineBasePtr& pl) {
	std::lock_guard lg(_mutex);
	_pipelines.push_back(pl);
	std::sort(_pipelines.begin(), _pipelines.end(), PipelineComparePaintLayer);
}

void PipelineList::resized(const VkRect2D& rect) {
	iterate([&](const PipelineBasePtr& pipeline) {
		pipeline->setViewportRect(rect);
		pipeline->setScissorRect(rect);
	});
}

