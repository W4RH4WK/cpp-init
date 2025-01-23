#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <example/example_hello.hpp>
#include <example/example_logger.hpp>
#include <example/example_platform.test.hpp>

using namespace Example;

TEST_CASE("hello with name", "[hello]")
{
	REQUIRE(hello("Tim") == "Hello Tim!");
}

TEST_CASE("hello without name", "[hello]")
{
	REQUIRE(hello("") == "Hello!");
}

TEST_CASE("hello logging", "[hello]")
{
	// Set log callback to some mock implementation.
	static std::string g_lastLogMessage;
	onLog = +[](std::string_view message, std::string_view, long) { g_lastLogMessage = message; };

	REQUIRE(hello("") == "Hello!");
	REQUIRE(!g_lastLogMessage.empty());
}

TEST_CASE("hello benchmarks", "[hello]")
{
	BENCHMARK("with name")
	{
		return hello("Tim");
	};

	BENCHMARK("without name")
	{
		return hello("");
	};
}

TEST_CASE("platform mock", "[platform]")
{
	Platform::initMock();

	REQUIRE(Platform::cpuCount() == 512);
}
