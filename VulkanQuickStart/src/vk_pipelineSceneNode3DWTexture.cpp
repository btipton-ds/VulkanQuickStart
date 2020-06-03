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

#include <vk_pipeline3DWithSampler.h>
#include <vk_pipelineSceneNode3DWSampler.h>
#include <vk_pipeline3D.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

PipelineSceneNode3DWSampler::PipelineSceneNode3DWSampler(const PipelineBasePtr& ownerPipeline)
	: Pipeline3DWSampler::PipelineSceneNode(ownerPipeline)
	, _modelXForm(glm::mat4(1.0f))
{
}

PipelineSceneNode3DWSampler::~PipelineSceneNode3DWSampler() {
}


void PipelineSceneNode3DWSampler::updateUniformBuffer(size_t swapChainIndex) {
	auto pipeline3D = dynamic_pointer_cast<Pipeline3DWSampler>(_ownerPipeline);
	auto ubo = pipeline3D->getUniformBuffer();
	ubo.modelView *= _modelXForm;
	updateUniformBufferTempl(swapChainIndex, ubo);
}
