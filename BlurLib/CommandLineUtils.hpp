#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include "ThreadUtils.hpp"

bool IsNumber(const std::string_view & str)
{
	return std::all_of(str.begin(), str.end(), std::isdigit);
}

size_t ExtractPositiveInteger(std::string_view arg)
{
	try
	{
		if (!IsNumber(arg))
		{
			throw std::exception();
		}
		const auto value = std::stoi(arg.data());
		if (value <= 0)
		{
			throw std::exception();
		}
		return value;
	}
	catch (...)
	{
		throw std::exception(("'" + std::string(arg.data()) + "' should be a positive integer").c_str());
	}
}

std::vector<ThreadPriority> ExtractThreadPriorities(size_t threadsCount, size_t argc, char * argv[])
{
	if (argc != (threadsCount + 5))
	{
		throw std::exception();
	}
	std::vector<ThreadPriority> threadPriorities;
	threadPriorities.reserve(threadsCount);
	for (size_t i = 5; i < argc; ++i)
	{
		threadPriorities.push_back(ToThreadPriority(std::stoi(argv[i])));
	}
	return threadPriorities;
}
