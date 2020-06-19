#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace::std;
using namespace::chrono;

int main()
{
	const auto startTime = high_resolution_clock::now();
	bool q;
	cin >> q;
	vector<size_t> nums(2, 0);
	iota(nums.begin(), nums.end(), 1);
	
	vector<thread> threads;
	for (size_t i = 0; i < nums.size(); ++i)
	{
		const auto threadNum = nums[i];
		threads.emplace_back([threadNum, startTime]() {
			for (size_t j = 0; j < 100000; ++j)
			{
				const auto nowTime = high_resolution_clock::now();
				const auto dt = duration_cast<microseconds>(nowTime - startTime).count() / 1000.f;
				auto dt_ = to_string(dt);
				replace(dt_.begin(), dt_.end(), '.', ',');
				((threadNum == 1) ? cout : cerr) << /*std::to_string(threadNum) + "|" +*/ dt_ + "\n";
			}
		});
	}
	for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
}
