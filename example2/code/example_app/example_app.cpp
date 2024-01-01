#include <fmt/core.h>

#include <example/example_hello.hpp>
#include <example/example_logger_file.hpp>
#include <example/example_platform.hpp>

int main(int argc, char* argv[])
{
	Example::Platform::initialize();

	// Set up logger
	{
		const auto logFilename = "logfile.txt";
		Example::g_logger = Example::FileLogger::create(logFilename);
		if (!Example::g_logger) {
			fmt::print("Could not create log file: {}\n", logFilename);
			return 1;
		}
	}

	if (argc != 2) {
		fmt::print("usage: {} <name>\n\n", argv[0]);
		return 1;
	}

	fmt::print("{}\n", Example::hello(argv[1]));

	fmt::print("We are running on {} CPUs.\n", Example::Platform::get().cpuCount());

	Example::Platform::finalize();

	return 0;
}
