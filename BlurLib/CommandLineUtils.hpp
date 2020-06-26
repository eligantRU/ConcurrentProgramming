#pragma once

#include <algorithm>
#include <vector>
#include <string>

#include "ProcessingMode.hpp"
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

std::vector<ThreadPriority> ExtractThreadPriorities(size_t threadsCount, size_t argc, char * argv[], int shift = 0)
{
	if (argc != (threadsCount + 5 + shift))
	{
		throw std::exception();
	}
	std::vector<ThreadPriority> threadPriorities;
	threadPriorities.reserve(threadsCount);
	for (size_t i = 5 + shift; i < argc; ++i)
	{
		threadPriorities.push_back(ToThreadPriority(std::stoi(argv[i])));
	}
	return threadPriorities;
}

ProcessingMode ExtractProcessingMode(std::string_view str)
{
	if (str == "tp")
	{
		return ProcessingMode::ThreadPool;
	}
	else if (str == "ntp")
	{
		return ProcessingMode::NonThreadPool;
	}
	throw std::exception((std::string("Unknown processing mode: ") + str.data()).c_str());
}
