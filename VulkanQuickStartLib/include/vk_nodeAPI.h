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
#include <vk_exports.h>
#include <memory>
#include <string>

#include <memory>
#include <inttypes.h>

namespace VK {
	class VulkanApp;
	using VulkanAppPtr = std::shared_ptr<VulkanApp>;
}

namespace VQS_API
{
	enum class CommandId {
		CMD_OpenFile,
		CMD_MouseDown,
		CMD_MouseUp,
		CMD_MouseEnter,
		CMD_MouseLeave,
		CMD_MouseMove,
		CMD_MouseOut,
		CMD_MouseOver,
		CMD_MouseWheel,
		CMD_Result,
		CMD_Unknown,
		CMD_LAST
	};

	enum class CmdDataType {
		PT_2D,
		STRING,
		Error,
		NONE
	};

	struct CmdData {
		inline CmdData(CmdDataType t)
		{
			type = t;
		}
		CmdDataType type;
	};

	struct CmdDataPoint2d : public CmdData {
		inline CmdDataPoint2d(double xIn = 0, double yIn = 0)
			: CmdData(CmdDataType::PT_2D)
			, x(xIn)
			, y(yIn)
		{}
		double x, y;
	};

	struct CmdDataString : public CmdData {
		inline CmdDataString(const std::string& s) : CmdData(CmdDataType::STRING), str(s) {}
		std::string str;
	};

	class EXPORT_VQS Api {
	public:
		Api();
		virtual ~Api();

		// CANNOT use stl vector/itereators due to debugging mismatch. TODO BRT - fix this
		virtual size_t getNumCommands() const = 0;
		virtual std::string getCommand(size_t idx) const = 0;
		virtual CmdData doCommand(CommandId cmd, const CmdData& command) = 0;
		virtual void initVQS(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers) = 0;
		virtual void setFrameBuffers(uint32_t width, uint32_t height, uint32_t numBuffers, uint8_t** buffers) = 0;
		virtual uint32_t getHeadlessFrameIndex() const = 0;
		virtual void doneWithHeadlessFrameIndex() = 0;
		virtual void setAppPtr(const VK::VulkanAppPtr& app) = 0;

	};

}

typedef std::shared_ptr<VQS_API::Api>(*getApiFunc)();
std::shared_ptr<VQS_API::Api> EXPORT_VQS getVqsApi();
