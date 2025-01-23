#include <example/example_logger.hpp>

namespace Example {

// Note that this callback is not initialized. We want the application to decide
// where log messages go rather than spitting out noise to stdout.
void (*onLog)(std::string_view message, std::string_view file, long line);

void logToStdout(std::string_view message, std::string_view file, long line)
{
	fmt::println("[{}:{}] {}", file, line, message);
}

} // namespace Example
