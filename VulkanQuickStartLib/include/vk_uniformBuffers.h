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

#include <functional>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace VK {

	template<typename UBO_TYPE>
	using UpdateUboFunctionType = std::function<bool(uint32_t width, uint32_t height, UBO_TYPE& ubo)>;

	struct UniformBufferObject3D {
		alignas(16) float ambient;

		alignas(16) glm::mat4 modelView;
		alignas(16) glm::mat4 proj;
		alignas(16) int numLights;
		alignas(16) glm::vec3 lightDir[2];
	};

	struct UBO3DVert {
		alignas(16) glm::mat4 modelView;
		alignas(16) glm::mat4 proj;
	};

	struct UBO3DFrag {
		alignas(16) float ambient;
		alignas(16) int numLights;
		alignas(16) glm::vec3 lightDir[2];
	};

	struct UBOImageProc {
		unsigned int _dstWidth = 0;
		unsigned int _dstHeight = 0;

		unsigned int _srcWidth = 0;
		unsigned int _srcHeight = 0;
	};

}

