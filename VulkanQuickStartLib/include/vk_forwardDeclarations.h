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
#include <vector>

/*
	This file is not strictly required, but it reduces the work if the forward declrations change. You only have to change each one once.
*/

namespace TriMesh {
	class CMesh;
	using CMeshPtr = std::shared_ptr<CMesh>;
}

namespace VK {
	struct DeviceContext;
	using DeviceContextPtr = std::shared_ptr<DeviceContext>;

	class Buffer;
	using BufferPtr = std::shared_ptr<Buffer>;

	class Image;
	using ImagePtr = std::shared_ptr<Image>;

	class TextureImage;
	using TextureImagePtr = std::shared_ptr<TextureImage>;

	class PostDrawTask;
	using PostDrawTaskPtr = std::shared_ptr<PostDrawTask>;

	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<VulkanApp>;

	class OffscreenPassBase;
	using OffscreenPassBasePtr = std::shared_ptr<OffscreenPassBase>;

	class ShaderPool;
	using ShaderPoolPtr = std::shared_ptr<ShaderPool>;

	class PipelineBase;
	using PipelineBasePtr = std::shared_ptr<PipelineBase>;

	template<class VERT_TYPE, class UBO_TYPE>
	class Pipeline;

	template<class PIPELINE_TYPE>
	using PipelinePtr = std::shared_ptr<PIPELINE_TYPE>;

	template<class VERT_TYPE>
	class SceneNode;

	class PipelineUi;
	using PipelineUiPtr = std::shared_ptr<PipelineUi>;

	class PipelineUboGroupBase;
	using PipelineUboGroupBasePtr = std::shared_ptr<PipelineUboGroupBase>;

	template<class UBO_TYPE>
	class PipelineUboGroup;
	template<class UBO_TYPE>
	using PipelineUboGroupPtr = std::shared_ptr<PipelineUboGroup<UBO_TYPE>>;

	class SceneNodeXForm;
	using SceneNodeXFormPtr = std::shared_ptr<SceneNodeXForm>;

	class SceneNodeBase;
	using SceneNodeBasePtr = std::shared_ptr<SceneNodeBase>;

	class ModelPNC3f;
	using ModelPNC3fPtr = std::shared_ptr<ModelPNC3f>;

	class ComputeStepBase;
	using ComputeStepBasePtr = std::shared_ptr<ComputeStepBase>;

	class PipelinePNC3f;
	using PipelinePNC3fPtr = std::shared_ptr<PipelinePNC3f>;

	class PipelinePNCT3f;
	using PipelinePNCT3fPtr = std::shared_ptr<PipelinePNCT3f>;

	class TransformFunc;
	using TransformFuncPtr = std::shared_ptr<TransformFunc>;

	namespace UI {
		class Window;
		using WindowPtr = std::shared_ptr<Window>;

		class Button;
		using ButtonPtr = std::shared_ptr<Button>;
	}

}