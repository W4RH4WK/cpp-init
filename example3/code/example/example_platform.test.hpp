#pragma once

#include <example/example_platform.hpp>

namespace Example::Platform {

// Re-initialize platform functions with a mock implementation for testing.
inline void initMock()
{
	cpuCount = +[] { return 512; };
}

} // namespace Example::Platform
