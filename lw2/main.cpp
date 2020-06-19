#include <iostream>

#include "CommandLineUtils.hpp"
#include "TimeUtils.hpp"
#include "Blur.hpp"

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
