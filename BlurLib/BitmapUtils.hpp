#pragma warning(disable : 4714)
#pragma warning(disable : 4244)

#pragma warning(push, 0)
#include <boost/gil/extension/io/bmp.hpp>
#pragma warning(pop)

#include "Bitmap.hpp"

struct ImageInfo
{
	std::vector<Pixel> pixels;
	size_t width;
	size_t height;
};

ImageInfo ImportPixels(std::string_view imgInName)
{
	using namespace::boost::gil;

	rgb8_image_t img;
	read_image(imgInName.data(), img, bmp_tag());
	
	std::vector<Pixel> pixels;
	pixels.reserve(img.width() * img.height() * num_channels<rgb8_image_t>());
	for_each_pixel(const_view(img), [&pixels](boost::gil::rgb8_pixel_t p) {
		pixels.push_back({
			at_c<0>(p),
			at_c<1>(p),
			at_c<2>(p)
		});
	});
	return {
		pixels,
		static_cast<size_t>(img.width()),
		static_cast<size_t>(img.height())
	};
}

void ExportPixels(std::vector<Pixel> pixels, size_t width, size_t height, std::string_view imgOutName)
{
	using namespace::boost::gil;

	rgb8_image_t img(width, height);
	
	rgb8_image_t::view_t v = view(img);
	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; ++x)
		{
			const auto & pixel = pixels[y * width + x];
			v(x, y) = rgb8_pixel_t(pixel.R, pixel.G, pixel.B);
		}
	}

	write_view(imgOutName.data(), const_view(img), bmp_tag());
}
