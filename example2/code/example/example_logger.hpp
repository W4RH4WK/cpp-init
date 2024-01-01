#pragma once

namespace Example {

// ILogger defines a very basic logger interface for illustration purposes.
// There are two real implementations: ConsoleLogger and FileLogger; there's
// also a MockLogger that can be used for testing.
class ILogger {
  public:
	virtual void log(std::string_view) = 0;
	virtual ~ILogger() noexcept = default;
};

// g_logger is the default logger instance that can be accessed across the
// code-base. The instance is located on the heap and owned by this unique
// pointer.
inline std::unique_ptr<ILogger> g_logger;

} // namespace Example
