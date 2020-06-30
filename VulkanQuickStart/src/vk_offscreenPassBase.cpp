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

#include <array>

#include <vk_logger.h>
#include <vk_offscreenPassBase.h>
#include <vk_deviceContext.h>
#include <vk_image.h>
#include <vk_pipelineBase.h>
#include <vk_app.h>

using namespace std;
using namespace VK;

OffscreenPassBase::OffscreenPassBase(const VulkanAppPtr& app, VkFormat colorFormat)
	: _deviceContext(app->getDeviceContext())
	, _colorFormat(colorFormat)
	, _depthFormat(app->findDepthFormat())
{
}

OffscreenPassBase::~OffscreenPassBase() {
	cleanup();
}

void OffscreenPassBase::init(const VkExtent2D& extent) {

	_rect = { {0, 0}, extent };
	auto device = _deviceContext->_device;

	VkSampleCountFlagBits antiAliasSampleBits = getAntiAliasSamples();

	// Find a suitable _depth format
	VkFormat fbColorFormat = _colorFormat; // VK_FORMAT_R8G8B8A8_UNORM; // VK_FORMAT_R32G32B32A32_SFLOAT

	uint32_t colorUsageBits = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	_color = TextureImage::newPtr(_deviceContext, fbColorFormat, colorUsageBits, _rect.extent.width, _rect.extent.height, antiAliasSampleBits);

	// Create _sampler to sample from the attachment in the fragment shader
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	samplerInfo.addressModeV = samplerInfo.addressModeU;
	samplerInfo.addressModeW = samplerInfo.addressModeU;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHK(vkCreateSampler(device, &samplerInfo, nullptr, &_sampler));

	// Depth stencil attachment
	VkFormat fbDepthFormat = _depthFormat;

	_depth = make_shared<Image>(_deviceContext);
	_depth->create(fbDepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, _rect.extent.width, _rect.extent.height, antiAliasSampleBits);

	// Create a separate render pass for the offscreen rendering as it may differ from the one used for scene rendering

	std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
	// Color attachment
	attchmentDescriptions[0].format = fbColorFormat;
	attchmentDescriptions[0].samples = antiAliasSampleBits;
	attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_GENERAL; // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// Depth attachment
	attchmentDescriptions[1].format = fbDepthFormat;
	attchmentDescriptions[1].samples = antiAliasSampleBits;
	attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attchmentDescriptions[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
	VkAttachmentReference depthReference = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpassDescription = {};
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// Create the actual renderpass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attchmentDescriptions.size());
	renderPassInfo.pAttachments = attchmentDescriptions.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();

	VkRenderPass renderPass;
	VK_CHK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
	setRenderPass(renderPass);

	VkImageView attachments[2];
	attachments[0] = _color->getImageView();
	attachments[1] = _depth->getImageView();

	VkFramebufferCreateInfo fbufCreateInfo = {};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.renderPass = renderPass;
	fbufCreateInfo.attachmentCount = 2;
	fbufCreateInfo.pAttachments = attachments;
	fbufCreateInfo.width = _rect.extent.width;
	fbufCreateInfo.height = _rect.extent.height;
	fbufCreateInfo.layers = 1;

	VK_CHK(vkCreateFramebuffer(device, &fbufCreateInfo, nullptr, &_frameBuffer));

	// Fill a descriptor for later use in a descriptor set 
	_descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	_descriptor.imageView = _color->getImageView();
	_descriptor.sampler = _sampler;
}

void OffscreenPassBase::cleanup() {
	auto device = _deviceContext->_device;

	if (_sampler)
		vkDestroySampler(device, _sampler, nullptr);
	if (_frameBuffer)
		vkDestroyFramebuffer(device, _frameBuffer, nullptr);

	_sampler = VK_NULL_HANDLE;
	_frameBuffer = VK_NULL_HANDLE;
}
