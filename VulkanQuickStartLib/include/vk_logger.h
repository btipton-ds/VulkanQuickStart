#pragma once

/*

This file is part of the TipScat Project.

	The TipScat project is sole property of Robert R Tipton and Dark Sky Innovative solutions.
	All rights reserved.

	Robert R Tipton - Author

	Dark Sky Innovative Solutions http://darkskyinnovation.com/

*/

#include <vk_defines.h>

#include <string>
#include <memory>
#include <iostream>

#pragma warning (push)
#pragma warning( disable : 4251 )

namespace VK {

	class EXPORT_VQS Logger {
	public:
		enum Level {
			LEVEL_INFO = 1,
			LEVEL_WARN = 2,
			LEVEL_ERROR = 4,
			LEVEL_EXCEPTION = 8
		};

		static void create(const std::string& name);

		static void disable(Level level);
		static void enable(Level level);
		static void logInfo(const std::string& message, bool echoToConsole = false);
		static void logWarn(const std::string& message, bool echoToConsole = false);
		static void logError(const std::string& message, bool echoToConsole = true);
		static void logException(const std::string& message, bool echoToConsole = true);
		static void chk(VkResult err);
		static const char* vkResultString(VkResult result);

	private:
		Logger(const std::string& name);

		static std::shared_ptr<Logger> _sLogger;

		int _enabled = 0xffffffff;
		std::shared_ptr<std::ostream> _outPtr;
		std::ostream& _out;
	};

}

#define THROW(MSG) { \
	VK::Logger::logException(MSG); \
	throw std::runtime_error(MSG); \
}

#define VK_CHK(err) VK::Logger::chk(err);

#pragma warning (pop)
