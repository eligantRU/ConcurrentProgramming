#include <string>

#include "ThreadUtils.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{

int ToWinThreadPriority(ThreadPriority priority)
{
	switch (priority)
	{
	case ThreadPriority::ABOVE_NORMAL:
		return THREAD_PRIORITY_ABOVE_NORMAL;
	case ThreadPriority::BELOW_NORMAL:
		return THREAD_PRIORITY_BELOW_NORMAL;
	case ThreadPriority::HIGHEST:
		return THREAD_PRIORITY_HIGHEST;
	case ThreadPriority::IDLE:
		return THREAD_PRIORITY_IDLE;
	case ThreadPriority::LOWEST:
		return THREAD_PRIORITY_LOWEST;
	case ThreadPriority::NORMAL:
		return THREAD_PRIORITY_NORMAL;
	case ThreadPriority::TIME_CRTICIAL:
		return THREAD_PRIORITY_TIME_CRITICAL;
	default:
		throw std::exception("Unknown ThreadPriority");
	}
}

}

ThreadPriority ToThreadPriority(int priority)
{
	switch (priority)
	{
	case 0:
		return ThreadPriority::ABOVE_NORMAL;
	case 1:
		return ThreadPriority::BELOW_NORMAL;
	case 2:
		return ThreadPriority::HIGHEST;
	case 3:
		return ThreadPriority::IDLE;
	case 4:
		return ThreadPriority::LOWEST;
	case 5:
		return ThreadPriority::NORMAL;
	case 6:
		return ThreadPriority::TIME_CRTICIAL;
	default:
		throw std::exception(("Unknown ThreadPriority: " + std::to_string(priority)).c_str());
	}
}

void SetThreadAffinityMask(std::thread & thread, size_t coresCount)
{
#ifdef _WIN32
SetThreadAffinityMask(thread.native_handle(), (1 << coresCount) - 1);
#else
#pragma message("SetThreadAffinityMask ignored")
#endif
}

void SetThreadPriority(std::thread & thread, ThreadPriority priority)
{
#ifdef _WIN32
SetThreadPriority(thread.native_handle(), ToWinThreadPriority(priority));
#else
#pragma message("SetThreadAffinityMask ignored")
#endif
}
