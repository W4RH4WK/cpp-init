#pragma once

#include <example/example_logger.hpp>

namespace Example {

class MockLogger : public ILogger {
  public:
	static std::unique_ptr<MockLogger> create() { return std::unique_ptr<MockLogger>(new MockLogger); }

	static MockLogger* initialize()
	{
		g_logger = create();
		return static_cast<MockLogger*>(g_logger.get());
	}

	void log(std::string_view message) override { lastMessage = message; }

	std::string lastMessage;

  private:
	MockLogger() = default;
};

} // namespace Example
