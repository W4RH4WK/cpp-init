#include <fstream>

#include <fmt/core.h>

#include <example/example_hello.hpp>
#include <example/example_logger.hpp>
#include <example/example_platform.hpp>

std::ofstream g_logFile;

int main(int argc, char* argv[])
{
	Example::Platform::init();

	// Set up logger
	{
		g_logFile = std::ofstream("logfile.txt");
		if (!g_logFile) {
			fmt::println("Could not create log file");
			return 1;
		}
		Example::onLog = +[](std::string_view message, std::string_view file, long line) {
			g_logFile << fmt::format("[{}:{}] Example: {}\n", file, line, message);
		};
	}

	if (argc != 2) {
		fmt::println("usage: {} <name>\n", argv[0]);
		return 1;
	}

	fmt::println("{}", Example::hello(argv[1]));

	fmt::println("We are running on {} CPUs.", Example::Platform::cpuCount());

	Example::Platform::deinit();

	return 0;
}
