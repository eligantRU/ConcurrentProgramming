#include <functional>
#include <algorithm>
#include <iostream>
#include <optional>
#include <numeric>
#include <ostream>
#include <fstream>
#include <vector>
#include <thread>
#include <array>

#include "BitmapUtils.hpp"
#include "ThreadUtils.h"

namespace
{

template<class T>
std::vector<std::vector<T>> Bunchify(const std::vector<T> & container, size_t bunchSize)
{
	std::vector<std::vector<T>> bunches;
	for(size_t i = 0; i < container.size(); i += bunchSize)
	{
		bunches.emplace_back(container.cbegin() + i, container.cbegin() + std::min(container.size(), i + bunchSize));
	}
	return bunches;
}

}

Bitmap MergeBitmaps(std::vector<Bitmap> && bitmaps, size_t width, size_t height)
{
	std::vector<Pixel> totalBluredPixels;
	totalBluredPixels.reserve(width * height);
	for (const auto & bluredBitmap : bitmaps)
	{
		const auto & bluredPixels = bluredBitmap.Pixels();
		totalBluredPixels.insert(totalBluredPixels.end(), std::make_move_iterator(bluredPixels.cbegin()), std::make_move_iterator(bluredPixels.cend()));
	}
	return { std::move(totalBluredPixels), width, height };
}

std::vector<Bitmap> BunchifyBitmap(const Bitmap & bmp, size_t bunchSize)
{
	const size_t processingHeight = bmp.Height() / bunchSize;

	std::vector<Bitmap> bunches;
	bunches.reserve(processingHeight);
	for (const auto & bunch : Bunchify(bmp.Pixels(), processingHeight * bmp.Width()))
	{
		bunches.emplace_back(bunch, bmp.Width(), bunch.size() / bmp.Width());
	}
	return bunches;
}
