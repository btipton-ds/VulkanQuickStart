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

#include <map>

#include <vk_nodeAPI.h>

using namespace std;
using namespace VQS_API;

Api::Api()
{}

Api::~Api()
{}

class ApiHandler {
public:
	virtual CmdData doCommand(const CmdData& command) = 0;
};

using ApiHandlerPtr = shared_ptr<ApiHandler>;

class ApiImpl : public Api
{
public:
	ApiImpl();
	~ApiImpl();
	CmdData doCommand(CommandId cmd, const CmdData& command) override;
	void getFrame(uint8_t* buffer, size_t& width, size_t& height) const override;

private:
	map<CommandId, ApiHandlerPtr> _commandMap;
};

shared_ptr<Api> getVqsApi()
{
	static shared_ptr<Api> api;

	if (!api)
		api = make_shared<ApiImpl>();

	return api;
}

class InitHandler : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		return CmdData(CmdDataType::NONE);
	}
};

ApiImpl::ApiImpl()
	: Api()
{
	_commandMap[CommandId::CMD_INIT] = make_shared<InitHandler>();
}

ApiImpl::~ApiImpl()
{}

CmdData ApiImpl::doCommand(CommandId cmd, const CmdData& command)
{
	auto iter = _commandMap.find(cmd);

	if (iter != _commandMap.end()) {
		return iter->second->doCommand(command);
	}

	return CmdData(CmdDataType::Error);
}

void ApiImpl::getFrame(uint8_t* buffer, size_t& width, size_t& height) const
{

}
