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

Bitmap BlurBitmap(const Bitmap & bmp)
{
	std::vector<Pixel> pixels;
	pixels.reserve(bmp.Width() * bmp.Height());

	const auto getPixel = [](const Bitmap & bmp, size_t x, size_t y) -> std::optional<Pixel> {
		if ((x >= bmp.Width()) || (y >= bmp.Height()))
		{
			return std::nullopt;
		}
		return bmp.Pixel(x, y);
	};

	for (size_t y = 0; y < bmp.Height(); ++y)
	{
		for (size_t x = 0; x < bmp.Width(); ++x)
		{
			constexpr size_t matrixOrder = 9;

			std::array<std::optional<Pixel>, matrixOrder> optPixels = {
				getPixel(bmp, x - 1, y - 1), getPixel(bmp, x, y - 1), getPixel(bmp, x + 1, y - 1),
				getPixel(bmp, x - 1, y), getPixel(bmp, x, y), getPixel(bmp, x + 1, y),
				getPixel(bmp, x - 1, y + 1), getPixel(bmp, x, y + 1), getPixel(bmp, x + 1, y + 1),
			};
			const auto nulloptCount = count(optPixels.cbegin(), optPixels.cend(), std::nullopt);
			const auto r = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & optPixel) {
				return acc + (optPixel ? (*optPixel).R : 0);
			});
			const auto g = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & optPixel) {
				return acc + (optPixel ? (*optPixel).G : 0);
			});
			const auto b = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & optPixel) {
				return acc + (optPixel ? (*optPixel).B : 0);
			});
			pixels.push_back({
				static_cast<uint8_t>(r / (matrixOrder - nulloptCount)),
				static_cast<uint8_t>(g / (matrixOrder - nulloptCount)),
				static_cast<uint8_t>(b / (matrixOrder - nulloptCount))
			});
		}
	}
	return {pixels, bmp.Width(), bmp.Height()};
}

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

}

void Blur(std::string_view imgInName, std::string_view imgOutName, size_t threadsCount, size_t coresCount)
{
	auto [pixels, width, height] = ImportPixels(imgInName);
	Bitmap bmp(pixels, width, height);

	auto subBitmaps = BunchifyBitmap(bmp, threadsCount);
	std::vector<std::thread> threads;
	for (size_t i = 0; i < subBitmaps.size(); ++i)
	{
		threads.emplace_back([&subBitmaps, i]() {
			subBitmaps[i] = BlurBitmap(subBitmaps[i]);
		});
		SetThreadAffinityMask(threads.back(), coresCount);
	}
	for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

	std::vector<Pixel> totalBluredPixels;
	totalBluredPixels.reserve(bmp.Width() * bmp.Height());
	for (const auto & bluredBitmap : subBitmaps)
	{
		const auto & bluredPixels = bluredBitmap.Pixels();
		totalBluredPixels.insert(totalBluredPixels.end(), std::make_move_iterator(bluredPixels.cbegin()), std::make_move_iterator(bluredPixels.cend()));
	}
	ExportPixels(totalBluredPixels, bmp.Width(), bmp.Height(), imgOutName.data());
}
