#pragma once

#define EXAMPLE_LOG(...) \
	do { \
		if (::Example::onLog) { \
			::Example::onLog(::fmt::format(__VA_ARGS__), __FILE__, __LINE__); \
		} \
	} while (0)

namespace Example {

// All we actually need is a simple function pointer, which allows the user of
// the library to redirect the library's log output to where-ever they need. One
// could provide some convenience functions as well.
using OnLog = void(*)(std::string_view message, std::string_view file, long line);
extern OnLog onLog;

// One such convenience functions would be logging to stdout.
void logToStdout(std::string_view message, std::string_view file, long line);

} // namespace Example
