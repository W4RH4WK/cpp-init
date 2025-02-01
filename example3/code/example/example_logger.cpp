#include <example/example_logger.hpp>

namespace Example {

// Note that this callback is zero-initialized. We want the application to decide
// where log messages go rather than spitting out noise to stdout.
OnLog onLog;

void logToStdout(std::string_view message, std::string_view file, long line)
{
	fmt::println("[{}:{}] {}", file, line, message);
}

} // namespace Example
