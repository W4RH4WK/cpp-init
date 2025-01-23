#pragma once

// All functions inside the Example::Platform are stateful!
namespace Example::Platform {

// Platform specific setup and tear-down functions.
void init();
void deinit();

// All other platform functions are exposed as function pointers to allow for
// mocking.
extern int (*cpuCount)();

} // namespace Example::Platform
