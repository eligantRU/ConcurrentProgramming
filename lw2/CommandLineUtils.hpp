#include <algorithm>
#include <string>

bool IsNumber(const std::string_view & str)
{
	return std::all_of(str.begin(), str.end(), std::isdigit);
}

size_t ExtractThreadsCount(std::string_view threadsCount)
{
	try
	{
		if (!IsNumber(threadsCount))
		{
			throw std::exception();
		}
		const auto value = std::stoi(threadsCount.data());
		if (value <= 0)
		{
			throw std::exception();
		}
		return value;
	}
	catch (...)
	{
		throw std::exception("<threads_count> should be a positive integer");
	}
}
