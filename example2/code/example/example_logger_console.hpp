#pragma once

#include <example/example_logger.hpp>

namespace Example {

class ConsoleLogger : public ILogger {
  public:
	static std::unique_ptr<ConsoleLogger> create() { return std::unique_ptr<ConsoleLogger>(new ConsoleLogger); }

	void log(std::string_view message) override { std::cout << message << "\n"; }

  private:
	ConsoleLogger() = default;
};

} // namespace Example
