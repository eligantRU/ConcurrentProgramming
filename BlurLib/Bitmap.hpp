#pragma once

#include <ostream>
#include <vector>

#include "Pixel.hpp"

class Bitmap
{
public:
	Bitmap(std::vector<Pixel> pixels, size_t width, size_t height)
		:m_pixels(pixels)
		,m_width(width)
		,m_height(height)
	{
		if (((width * height) != pixels.size()) || !width || !height)
		{
			throw std::exception("Invalid dimensions");
		}
	}

	const std::vector<Pixel> & Pixels() const
	{
		return m_pixels;
	}

	Pixel Pixel(size_t x, size_t y) const
	{
		return m_pixels[y * m_width + x];
	}

	size_t Width() const
	{
		return m_width;
	}

	size_t Height() const
	{
		return m_height;
	}

private:
	std::vector<BitmapPixel> m_pixels;
	size_t m_width;
	size_t m_height;
};
