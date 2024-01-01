#pragma once

namespace Example {

// Platform illustrates how singletons with special requirements can be
// realized, for example:
// - The implementation is created and destroyed via initialize and finalize
//   functions
// - The implementation must not use heap allocation
// - The implementation can be replaced by a mock for testing purposes
class Platform {
  public:
	// This part is the platform interface:
	virtual int cpuCount() = 0;
	virtual ~Platform() noexcept = default;

	// These are the instance management functions:
	static void initialize();
	static void finalize();
	static Platform& get() { return *sm_impl; }

  private:
	static inline Platform* sm_impl;
	friend class MockPlatform;
};

} // namespace Example
