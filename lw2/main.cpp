#include <CommandLineUtils.hpp>
#include <TimeUtils.hpp>
#include <BlurUtils.hpp>

namespace
{

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
