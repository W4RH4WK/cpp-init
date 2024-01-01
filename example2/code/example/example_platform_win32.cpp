#include <example/example_platform.hpp>

namespace Example {

// PlatformWin32 is a concrete Platform implementation. It provides the member
// functions required by the interface and implements the necessary management
// function.
class PlatformWin32 : public Platform {
  public:
	PlatformWin32() { fmt::print("Initializing Platform for Win32\n"); }
	~PlatformWin32() noexcept { fmt::print("Finalizing Platform for Win32\n"); }

	int cpuCount() override { return int(std::thread::hardware_concurrency()); }
};

static std::optional<PlatformWin32> g_platform; // <- not allocated on the heap

void Platform::initialize()
{
	Platform::sm_impl = &g_platform.emplace();
}

void Platform::finalize()
{
	Platform::sm_impl = nullptr;
	g_platform.reset();
}

} // namespace Example
