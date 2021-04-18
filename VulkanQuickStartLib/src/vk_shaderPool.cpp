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

#include <stdexcept>
#include <iostream>
#include <fstream>

#include <vk_logger.h>
#include <vk_shaderPool.h>

using namespace std;
using namespace VK;

ShaderPool::Shader::~Shader() {
	if (_module != VK_NULL_HANDLE)
		vkDestroyShaderModule(_device, _module, nullptr);

	_module = VK_NULL_HANDLE;
}

void ShaderPool::ShaderRec::add(VkDevice device, VkShaderStageFlagBits stage, VkShaderModule shaderModule) {
	ShaderPtr shader = make_shared<Shader>();
	shader->_device = device;
	shader->_stage = stage;
	shader->_module = shaderModule;
	_shaders.push_back(shader);
}

ShaderPool::ShaderPool(DeviceContext* dc) 
	: _dc(dc)
{
}

ShaderPool::ShaderRecPtr ShaderPool::addShader(const std::string& shaderId, const std::vector<std::string>& filenames) {

	if (_shaderRecs.count(shaderId) != 0) {
		throw runtime_error("A shader with this shaderId is already in the pool");
	}

	ShaderRecPtr shader = make_shared<ShaderRec>();
	for (const auto& filename : filenames) {
		auto vertCode = readFile(filename);

		VkShaderStageFlagBits stage = VK_SHADER_STAGE_VERTEX_BIT;
		if (filename.find("vert") != string::npos)
			stage = VK_SHADER_STAGE_VERTEX_BIT;
		else if (filename.find("frag") != string::npos)
			stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		else if (filename.find("comp") != string::npos)
			stage = VK_SHADER_STAGE_COMPUTE_BIT;
		else
			throw runtime_error("unexptected shader filename pattern");

		shader->add(_dc->_device, stage, createShaderModule(vertCode));
	}

	addShader(shaderId, shader);
	return shader;
}


void ShaderPool::addShader(const string& shaderId, const ShaderRecPtr& shader) {
	_shaderRecs[shaderId] = shader;
}

ShaderPool::ShaderRecPtr ShaderPool::getShader(const string& shaderId) const {
	auto iter = _shaderRecs.find(shaderId);
	if (iter != _shaderRecs.end())
		return iter->second;
	return ShaderRecPtr();
}

void ShaderPool::removeShader(const string& shaderId) {
	auto iter = _shaderRecs.find(shaderId);
	if (iter != _shaderRecs.end()) {
		_shaderRecs.erase(shaderId);
	}
}

VkShaderModule ShaderPool::createShaderModule(const std::vector<char>& code) const {
	auto device = _dc->_device;

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		THROW("failed to create shader module!");
	}

	return shaderModule;
}

vector<char> ShaderPool::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		THROW("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

