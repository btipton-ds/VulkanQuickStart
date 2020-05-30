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

#include <memory>
#include <vector>

/*
	This file is not strictly required, but it reduces the work if the forward declrations change. You only have to change each one once.
*/

namespace VK {
	struct DeviceContext;
	struct Buffer;

	class Image;
	using ImagePtr = std::shared_ptr<Image>;

	class TextureImage;
	using TextureImagePtr = std::shared_ptr<TextureImage>;

	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<class VulkanApp>;

	class ShaderPool;
	using ShaderPoolPtr = std::shared_ptr<ShaderPool>;

	class PipelineBase;
	using PipelineBasePtr = std::shared_ptr<PipelineBase>;
	template<class UBO_TYPE, class VERT_TYPE>
	class Pipeline;
	template<class PIPELINE_TYPE>
	using PipelinePtr = std::shared_ptr<PIPELINE_TYPE>;

	class PipelineUi;
	using PipelineUiPtr = std::shared_ptr<PipelineUi>;

	class SceneNodeBase;
	using SceneNodeBasePtr = std::shared_ptr<SceneNodeBase>;

	template<class PIPELINE_TYPE>
	class SceneNode;

	class SceneNodeGroup;
	using SceneNodeGroupPtr = std::shared_ptr<SceneNodeGroup>;
	using SceneNodeGroupConstPtr = std::shared_ptr<const SceneNodeGroup>;

	class Model;
	using ModelPtr = std::shared_ptr<Model>;

	class ModelObj;
	using ModelObjPtr = std::shared_ptr<ModelObj>;

	class Scene;
	using ScenePtr = std::shared_ptr<Scene>;

	namespace UI {
		class Window;
		using WindowPtr = std::shared_ptr<Window>;

		class Button;
		using ButtonPtr = std::shared_ptr<Button>;
	}

}