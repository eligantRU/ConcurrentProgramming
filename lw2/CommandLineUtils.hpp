#include <algorithm>
#include <string>

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
