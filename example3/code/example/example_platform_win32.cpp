#include <example/example_platform.hpp>

namespace Example::Platform {

void init()
{
	fmt::print("Initializing Platform for Win32\n");
}

void deinit()
{
	fmt::print("Finalizing Platform for Win32\n");
}

static int cpuCountImpl()
{
	return int(std::thread::hardware_concurrency());
}
constinit int (*cpuCount)() = cpuCountImpl; // <-- could be lambda

} // namespace Example::Platform
