#include <fmt/core.h>

#include <example/example_hello.hpp>

int main(int argc, char* argv[])
{
	if (argc != 2) {
		fmt::print("usage: {} <name>\n\n", argv[0]);
		return 1;
	}

	fmt::print("{}\n", Example::hello(argv[1]));

	return 0;
}
