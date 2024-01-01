#include <example/example_hello.hpp>

#include <example/example_logger.hpp>

namespace Example {

std::string hello(std::string_view name)
{
	if (g_logger) {
		g_logger->log("Example::hello called");
	}

	if (name.empty()) {
		return "Hello!";
	}
	return "Hello " + std::string(name) + "!";
}

} // namespace Example
