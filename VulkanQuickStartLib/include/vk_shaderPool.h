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
#include <map>
#include <string>

#include <vulkan/vulkan_core.h>

#include <vk_forwardDeclarations.h>
#include <vk_deviceContext.h>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS ShaderPool {
	public:
		struct Shader {
			~Shader();

			VkShaderStageFlagBits _stage = VK_SHADER_STAGE_VERTEX_BIT;
			VkDevice _device = VK_NULL_HANDLE;
			VkShaderModule _module = VK_NULL_HANDLE;
		};
		using ShaderPtr = std::shared_ptr<Shader>;
		struct ShaderRec {
			std::vector<ShaderPtr> _shaders;

			void add(VkDevice device, VkShaderStageFlagBits stage, VkShaderModule shaderModule);
		};

		using ShaderRecPtr = std::shared_ptr<ShaderRec>;

		ShaderPool(DeviceContext* dc);

		ShaderRecPtr addShader(const std::string& shaderId, const std::vector<std::string>& filenames);
		void addShader(const std::string& shaderId, const ShaderRecPtr& shader);
		ShaderRecPtr getShader(const std::string& shaderId) const;
		void removeShader(const std::string& shaderId);

	private:
		DeviceContext* _dc;
		VkShaderModule createShaderModule(const std::vector<char>& code) const;
		static std::vector<char> readFile(const std::string& filename);

		std::map<std::string, ShaderRecPtr> _shaderRecs;
	};

}

#pragma warning (pop)
