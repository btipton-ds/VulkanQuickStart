/*

This file is part of the TipScat Project.

	The TipScat project is sole property of Robert R Tipton and Dark Sky Innovative solutions.
	All rights reserved.

	Robert R Tipton - Author

	Dark Sky Innovative Solutions http://darkskyinnovation.com/

*/

#include <vk_defines.h>

#include <map>
#include <memory>
#include <iostream>
#include <fstream>

#include <vk_logger.h>

using namespace std;
using namespace VK;

#ifdef _WIN32
#pragma warning( disable : 26812 )
#endif // _WIN32

shared_ptr<Logger> Logger::_sLogger;

void Logger::create(const string& name) {
    if (!_sLogger) {
        _sLogger = shared_ptr<Logger>(new Logger(name));
    } else {
        logWarn(string("Attempted to create logger twice. Name: ") + name);
    }
}

void Logger::disable(Level level) {
    if (_sLogger)
        _sLogger->_enabled &= ~((int)level);
}

void Logger::enable(Level level) {
    if (_sLogger)
        _sLogger->_enabled |= (int)level;
}

namespace {
	inline ostream& createOut(const std::string& name, shared_ptr<ostream>& outPtr) {
		outPtr = make_shared<ofstream>(name + ".log");
		if (outPtr->good())
			return *outPtr;
		else {
			cout << "Failed to create log file: '" << name << "'\n";
			cout.flush();
			return cout;
		}
	}
}
Logger::Logger(const std::string& name)
	: _out(createOut(name, _outPtr))
{}

void Logger::logInfo(const std::string& message, bool echoToConsole) {
    if (_sLogger && _sLogger->_enabled & LEVEL_INFO) {
        _sLogger->_out << "info: " << message << "\n";
        _sLogger->_out.flush();
	}
    if (echoToConsole)
        cout << "info: " << message << "\n";
}

void Logger::logWarn(const std::string& message, bool echoToConsole) {
    if (_sLogger && _sLogger->_enabled & LEVEL_WARN) {
        _sLogger->_out << "warn: " << message << "\n";
        _sLogger->_out.flush();
    }
    if (echoToConsole)
        cout << "warn: " << message << "\n";
}

void Logger::logError(const std::string& message, bool echoToConsole) {
    if (_sLogger && _sLogger->_enabled & LEVEL_ERROR) {
        _sLogger->_out << "error: " << message << "\n";
        _sLogger->_out.flush();
    }
    if (echoToConsole)
        cerr << "error: " << message << "\n";
}

void Logger::logException(const std::string& message, bool echoToConsole) {
    if (_sLogger && _sLogger->_enabled & LEVEL_ERROR) {
        _sLogger->_out << "exception: " << message << "\n";
        _sLogger->_out.flush();
    }
    if (echoToConsole)
        cerr << "exception: " << message << "\n";
}

void Logger::chk(VkResult err) {
	if (err != VK_SUCCESS) {
		THROW(vkResultString(err));
	}
}

const char* Logger::vkResultString(VkResult result) {

    switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
//        case VK_ERROR_INCOMPATIBLE_VERSION_KHR: return "VK_ERROR_INCOMPATIBLE_VERSION_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_NOT_PERMITTED_EXT: return "VK_ERROR_NOT_PERMITTED_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR: return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR: return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR: return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR: return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT: return "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT";
//        case VK_RESULT_RANGE_SIZE: return "VK_RESULT_RANGE_SIZE";
        default: return "Unknown VkResult type";
    }
}
