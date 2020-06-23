#pragma once

#include <ctime>

template <class T>
auto MeasureTime(T && fn)
{
	const auto begin = std::clock();
	fn();
	const auto end = std::clock();
	return std::difftime(end, begin);
}
