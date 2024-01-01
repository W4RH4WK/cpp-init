#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

#include <example/example_hello.hpp>
#include <example/example_logger.test.hpp>

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
	auto* logger = MockLogger::initialize();

	REQUIRE(hello("") == "Hello!");
	REQUIRE(!logger->lastMessage.empty());
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
