#include <filesystem>

#include <CommandLineUtils.hpp>
#include <TimeUtils.hpp>
#include <BlurUtils.hpp>
#include <ThreadUtils.hpp>

#include "ThreadPool.hpp"

namespace
{

void ThrowInvalidCommandLineArguments() // TODO: custom exception class with what overriding
{
	using namespace std::string_literals;
	throw std::exception(("Invalid command line arguments. Should be:\n"s
		+ "EXE <input_file_name> <output_file_name> <processing_mode> <blocks_count> <threads_count> <cores_count> <thread_priorities>\n"
		+ "Thread priorities:\n"
		+ "	0 - ABOVE_NORMAL\n"
		+ "	1 - BELOW_NORMAL\n"
		+ "	2 - HIGHEST\n"
		+ "	3 - IDLE\n"
		+ "	4 - LOWEST\n"
		+ "	5 - NORMAL\n"
		+ "	6 - TIME_CRTICIAL\n"
		+ "Processing mode:\n"
		+ "	tp - ThreadPool\n"
		+ "	ntp - NonThreadPool\n").c_str());
}

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


template <class T>
void Blur(std::string_view imgInName, std::string_view imgOutName, size_t blockCount, T && blurStrategy)
{
	auto [pixels, width, height] = ImportPixels(imgInName);
	Bitmap bmp(pixels, width, height);
	
	auto subBitmaps = BunchifyBitmap(bmp, blockCount);
	blurStrategy(subBitmaps);

	const auto bluredBitmap = MergeBitmaps(std::move(subBitmaps), bmp.Width(), bmp.Height());
	ExportPixels(bluredBitmap.Pixels(), bmp.Width(), bmp.Height(), imgOutName.data());
}

std::function<void(std::vector<Bitmap> &)> BlurStrategy(ProcessingMode mode, size_t coresCount, const std::vector<ThreadPriority> & threadPriorities)
{
	static ThreadPool pool(threadPriorities, coresCount);
	switch (mode)
	{
	case ProcessingMode::ThreadPool:
		return [](std::vector<Bitmap> & subBitmaps) {
			for (size_t i = 0; i < subBitmaps.size(); ++i)
			{
				pool.AddTask([&subBitmaps, i]() {
					subBitmaps[i] = BlurBitmap(subBitmaps[i]);
				});
			}
			pool.Join();
		};
	case ProcessingMode::NonThreadPool:
		return [&threadPriorities, coresCount](std::vector<Bitmap> & subBitmaps) {
			std::vector<std::thread> threads;
			threads.reserve(threadPriorities.size());
			for (size_t i = 0; i < subBitmaps.size(); ++i)
			{
				threads.emplace_back([&subBitmaps, i]() {
					subBitmaps[i] = BlurBitmap(subBitmaps[i]);
				});
				SetThreadAffinityMask(threads.back(), coresCount);
				SetThreadPriority(threads.back(), threadPriorities[i]);
			}
			for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
		};
	default:
		throw std::exception("Unexpected ProcessingMode");
	}
}

void BlurDir(std::string_view inputPath, std::string_view outputPath, ProcessingMode mode, size_t blockCount, size_t coresCount, const std::vector<ThreadPriority> & threadPriorities)
{
	if (!std::filesystem::exists(inputPath))
	{
		throw std::exception("This directory does not exist");
	}

	if (!std::filesystem::exists(outputPath))
	{
		std::filesystem::create_directories(outputPath);
	}

	const auto blurStrategy = BlurStrategy(mode, coresCount, threadPriorities);
	for (const auto & file : std::filesystem::directory_iterator(inputPath))
	{
		const auto path = file.path();
		if (path.extension() == ".bmp")
		{
			const auto input = path;
			const auto output = outputPath / path.filename();
			Blur(input.string(), output.string(), blockCount, blurStrategy);
		}
	}
}

}

int main(int argc, char * argv[])
{
	try
	{
		const auto runTime = MeasureTime([&]() {
			if (argc < 7) // TODO: Boost Program Options
			{
				ThrowInvalidCommandLineArguments();
			}

			const std::string dirIn = argv[1];
			const std::string dirOut = argv[2];
			const auto processingMode = ExtractProcessingMode(argv[3]);
			const auto blocksCount = ExtractPositiveInteger(argv[4]);
			const auto threadsCount = ExtractPositiveInteger(argv[5]);
			const auto coresCount = ExtractPositiveInteger(argv[6]);
			std::vector<ThreadPriority> threadPriorities;
			try
			{
				threadPriorities = std::move(ExtractThreadPriorities(threadsCount, argc, argv, +2));
			}
			catch (const std::exception &)
			{
				ThrowInvalidCommandLineArguments();
			}
			BlurDir(dirIn, dirOut, processingMode, blocksCount, coresCount, threadPriorities);
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
