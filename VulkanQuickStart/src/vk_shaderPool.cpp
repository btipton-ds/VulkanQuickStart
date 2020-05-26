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

#include <stdexcept>
#include <iostream>
#include <fstream>

#include <vk_shaderPool.h>

using namespace std;
using namespace VK;

void ShaderPool::ShaderRec::add(const std::string& filename, const VkShaderModule& shaderModule) {
	_shaderModules.push_back(shaderModule);
}

ShaderPool::ShaderPool(const DeviceContext& dc) 
	: _dc(dc)
{
}

ShaderPool::~ShaderPool() {
	for (auto& iter : _shaderRecs) {
		auto& shaderStages = iter.second->_shaderModules;
		for (auto& shaderModule : shaderStages) {
			vkDestroyShaderModule(_dc.device_, shaderModule, nullptr);
		}
		shaderStages.clear();
	}
	_shaderRecs.clear();
}

ShaderPool::ShaderRecPtr ShaderPool::addShader(const std::string& shaderId, const std::vector<std::string>& filenames) {

	if (_shaderRecs.count(shaderId) != 0) {
		throw runtime_error("A shader with this shaderId is already in the pool");
	}

	ShaderRecPtr shader = make_shared<ShaderRec>();
	for (const auto& filename : filenames) {
		auto vertCode = readFile(filename);
		shader->add(filename, createShaderModule(_dc, vertCode));
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
		auto& shaderStages = iter->second->_shaderModules;
		for (auto& shaderModule : shaderStages) {
			vkDestroyShaderModule(_dc.device_, shaderModule, nullptr);
		}
		shaderStages.clear();
		_shaderRecs.erase(shaderId);
	}
}

VkShaderModule ShaderPool::createShaderModule(const DeviceContext& dc, const std::vector<char>& code) const {
	auto device = dc.device_;

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

vector<char> ShaderPool::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

