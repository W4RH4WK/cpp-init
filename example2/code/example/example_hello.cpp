#include <example/example_hello.hpp>

namespace Example {

std::string hello(std::string_view name)
{
	if (name.empty()) {
		return "Hello!";
	}
	return "Hello " + std::string(name) + "!";
}

} // namespace Example
