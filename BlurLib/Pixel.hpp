#pragma once

#include <ostream>

struct Pixel
{
	friend std::ostream & operator<<(std::ostream & strm, const Pixel & pixel);

	uint8_t R;
	uint8_t G;
	uint8_t B;
};

using BitmapPixel = Pixel; // TODO:

std::ostream & operator<<(std::ostream & strm, const Pixel & pixel)
{
	return strm << std::hex << static_cast<short>(pixel.R) << static_cast<short>(pixel.G) << static_cast<short>(pixel.B);
}
