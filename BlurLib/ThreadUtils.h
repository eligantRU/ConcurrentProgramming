#include <thread>

void SetThreadAffinityMask(std::thread & thread, size_t coresCount);
