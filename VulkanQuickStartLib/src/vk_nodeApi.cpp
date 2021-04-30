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

#include <iostream>
#include <map>

#include <vk_nodeAPI.h>
#include <vk_main.h>
#include <vk_app.h>

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

	size_t getNumCommands() const override;
	string getCommand(size_t idx) const override;
	CmdData doCommand(CommandId cmd, const CmdData& command) override;
	void initVQS(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers) override;
	void setFrameBuffers(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers) override;
	uint32_t getHeadlessFrameIndex() const override;
	void doneWithHeadlessFrameIndex() override;
	void setAppPtr(const VK::VulkanAppPtr& app) override;

private:
	VK::VulkanAppPtr _app;
	map<CommandId, ApiHandlerPtr> _commandMap;
};

shared_ptr<Api> getVqsApi()
{
	static shared_ptr<ApiImpl> api;

	if (!api)
		api = make_shared<ApiImpl>();

	return api;
}

class MouseDownHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*) &command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseUpHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseEnterHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseMoveHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseLeaveHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseOutHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseOverHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class MouseWheelHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataPoint2d* ptr = (CmdDataPoint2d*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

class OpenFileHandler final : public ApiHandler
{
public:
	virtual CmdData doCommand(const CmdData& command)
	{
		CmdDataString* ptr = (CmdDataString*)&command;
		if (ptr) {
			// TODO BRT - Need to open the file. Before that we need all the pipelines and stuff running.
		}
		return CmdData(CmdDataType::NONE);
	}
};

ApiImpl::ApiImpl()
	: Api()
{
	// TODO BRT - Need a real factory for these, the commands and handler bindings can be messed up
	_commandMap[CommandId::CMD_OpenFile] = make_shared<OpenFileHandler>();

	_commandMap[CommandId::CMD_MouseDown] = make_shared<MouseDownHandler>();
	_commandMap[CommandId::CMD_MouseUp] = make_shared<MouseUpHandler>();
	_commandMap[CommandId::CMD_MouseEnter] = make_shared<MouseEnterHandler>();
	_commandMap[CommandId::CMD_MouseMove] = make_shared<MouseMoveHandler>();
	_commandMap[CommandId::CMD_MouseLeave] = make_shared<MouseLeaveHandler>();
	_commandMap[CommandId::CMD_MouseOut] = make_shared<MouseOutHandler>();
	_commandMap[CommandId::CMD_MouseOver] = make_shared<MouseOverHandler>();
	_commandMap[CommandId::CMD_MouseWheel] = make_shared<MouseWheelHandler>();
}

ApiImpl::~ApiImpl()
{}

inline CommandId incr(CommandId id) {
	int i = (int)id;
	return (CommandId)(i + 1);
}

void ApiImpl::setAppPtr(const VK::VulkanAppPtr& app)
{
	_app = app;
}

size_t ApiImpl::getNumCommands() const
{
	return (size_t)CommandId::CMD_LAST;
}

CmdData ApiImpl::doCommand(CommandId cmd, const CmdData& command)
{
	auto iter = _commandMap.find(cmd);

	if (iter != _commandMap.end()) {
		return iter->second->doCommand(command);
	}

	return CmdData(CmdDataType::Error);
}

void ApiImpl::initVQS(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers)
{
	VK::VulkanAppPtr app = initHeadless(width, height, numBuffers, buffers, true);
	getVqsApi()->setAppPtr(app);

}

void ApiImpl::setFrameBuffers(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers)
{
	_app->setHeadlessFrameBuffers(width, height, numBuffers, buffers, true);
}

uint32_t ApiImpl::getHeadlessFrameIndex() const
{
	return _app->getHeadlessFrameIndex();
}

void ApiImpl::doneWithHeadlessFrameIndex()
{
	_app->doneWithHeadlessFrameIndex();
}

string ApiImpl::getCommand(size_t idx) const
{
	string result;
	for (CommandId id = CommandId::CMD_OpenFile; id != CommandId::CMD_LAST; id = incr(id)) {
		switch (id)
		{
		case CommandId::CMD_OpenFile:
			result = "CMD_OpenFile";
			break;
		case CommandId::CMD_MouseDown:
			result = "CMD_MouseDown";
			break;
		case CommandId::CMD_MouseUp:
			result = "CMD_MouseUp";
			break;
		case CommandId::CMD_MouseEnter:
			result = "CMD_MouseEnter";
			break;
		case CommandId::CMD_MouseLeave:
			result = "CMD_MouseLeave";
			break;
		case CommandId::CMD_MouseMove:
			result = "CMD_MouseMove";
			break;
		case CommandId::CMD_MouseOut:
			result = "CMD_MouseOut";
			break;
		case CommandId::CMD_MouseOver:
			result = "CMD_MouseOver";
			break;
		case CommandId::CMD_MouseWheel:
			result = "CMD_MouseWheel";
			break;
		case CommandId::CMD_Result:
			result = "CMD_Result";
			break;
		case CommandId::CMD_Unknown:
			result = "CMD_Result";
			break;
		default:
			throw("CommandId enum is not mapped to a string");
		}
		if ((size_t)id == idx)
			return result;
	}

	return result;
}
