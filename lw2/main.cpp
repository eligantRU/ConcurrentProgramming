#include <CommandLineUtils.hpp>
#include <TimeUtils.hpp>
#include <BlurUtils.hpp>

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
			const auto r = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & pixel) {
				return acc + (pixel ? (*pixel).R : 0);
			});
			const auto g = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & pixel) {
				return acc + (pixel ? (*pixel).G : 0);
			});
			const auto b = accumulate(optPixels.cbegin(), optPixels.cend(), static_cast<size_t>(0), [](size_t acc, const auto & pixel) {
				return acc + (pixel ? (*pixel).B : 0);
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

	const auto bluredBitmap = MergeBitmaps(std::move(subBitmaps), bmp.Width(), bmp.Height());
	ExportPixels(bluredBitmap.Pixels(), bmp.Width(), bmp.Height(), imgOutName.data());
}

}

int main(int argc, char * argv[])
{
	try
	{
		const auto runTime = MeasureTime([&]() {
			if (argc != 5) // TODO: Boost Program Options
			{
				throw std::exception("Invalid command line arguments. Should be:\nEXE <input_file_name> <output_file_name> <threads_count> <cores_count>");
			}

			const std::string imgIn = argv[1];
			const std::string imgOut = argv[2];
			const auto threadsCount = ExtractPositiveInteger(argv[3]);
			const auto coresCount = ExtractPositiveInteger(argv[4]);
			Blur(imgIn, imgOut, threadsCount, coresCount);
		});
		std::cerr << runTime << "ms" << std::endl;
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unhandled non-STL exception" << std::endl;
	}
}
