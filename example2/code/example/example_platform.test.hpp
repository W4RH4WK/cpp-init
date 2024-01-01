#pragma once

#include <example/example_platform.hpp>

namespace Example {

// MockPlatform sets / overrides the current Platform for testing purposes.
// Member functions just get/set member variables.
class MockPlatform : public Platform {
  public:
	MockPlatform() { Platform::sm_impl = this; }
	~MockPlatform() noexcept { Platform::sm_impl = nullptr; }

	int cpuCount() override { return mockCpuCount; }
	int mockCpuCount = 1;
};

} // namespace Example
