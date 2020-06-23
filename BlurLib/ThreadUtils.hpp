#pragma once

#pragma once

#include <thread>

enum class ThreadPriority : size_t
{
	ABOVE_NORMAL = 0,
	BELOW_NORMAL,
	HIGHEST,
	IDLE,
	LOWEST,
	NORMAL,
	TIME_CRTICIAL,
};

void SetThreadAffinityMask(std::thread & thread, size_t coresCount);

void SetThreadPriority(std::thread & thread, ThreadPriority priority);

ThreadPriority ToThreadPriority(int priority);
