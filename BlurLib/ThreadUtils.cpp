#include "ThreadUtils.h"

#ifdef _WIN32
#include <Windows.h>
#endif

void SetThreadAffinityMask(std::thread & thread, size_t coresCount)
{
#ifdef _WIN32
SetThreadAffinityMask(thread.native_handle(), (1 << coresCount) - 1);
#else
#pragma message("SetThreadAffinityMask ignored")
#endif
}
