# How to Start C++ Development?

Since I encounter this question a lot lately, here's a summary of my typical answer along with some pointers.

- First get familiar with C.
  For this I can recommend [Beej's Guide to C Programming](https://beej.us/guide/bgc/).

- Once you are somewhat fluent with C, you may move on to C++.
  [Learn C++](https://www.learncpp.com/) is an excellent guide that has you covered.

- If you need technical information on a language feature or library component, search [C++ reference](https://cppreference.com/).

- Once you are somewhat familiar with C++, read through the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) and see what makes sense for you.

## Which Topics are Essential?

C++ is a complex language that uses various paradigms and conventions.
When wielding C++, you should understand these topics:

- Value semantics
    - Value vs. reference/pointer
    - Constructor/destructor
    - Copy/move semantics
- `const`-correctness
- Ownership & lifetime
    - Scope-based resource management (i.e. RAII)
- Dynamic memory:
    - Avoid `new`/`delete`
    - Prefer `std::unique_ptr` to `std::shared_ptr`
    - Pick the right container (see sequential vs. associative container)
- Common bugs:
    - Use of uninitialized variable
    - Use-after-free
    - Iterator invalidation
    - Undefined behavior
- Compilation process (preprocessor, translation units, linking, etc.)
    - Enable warnings
    - Enable debug symbols
    - Enable optimizations
    - Address sanitizer
- Debugger

## Which Development Environment?

### Windows

- Use [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/)
    - Install with *Desktop development with C++*
    - When creating a new project, select *Empty Project (C++)*
    - From here, just add a source file and you are good to go.

### Linux / macOS

- Use [Visual Studio Code](https://code.visualstudio.com/)
    - Install the [C/C++ extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)

- Install a compiler (GCC or Clang will do equally well)
    - E.g. for Debian, installs GCC with C++ support
      ```
      sudo apt install g++
      ```

- Install the [CMake](https://cmake.org/) build system
    - E.g. for Debian
      ```
      sudo apt install cmake
      ```

- Inspect the provided [example](example/) project

- To build and run, issue the following commands in the example folder:
    ```
    cmake -Bbuild -DCMAKE_BUILD_TYPE=Debug  # only needed the first time
    make -C build
    ./build/hello
    ```

### In the Browser

[Compiler Explorer](https://godbolt.org/) allows you write, compile, and run small C++ programs.
You can even use third-party libraries.

Whenever you have a problem / question, create a small example of your issue in Compiler Explorer and provide a short link (top-right *Share*) to it along with your question.

## Should I use a Build System?

In general, yes.
A build system organizes and documents the build process and allows other tools, like IDEs, to correctly handle your project (e.g. auto-completion).

My personal preference is [CMake](https://cmake.org/), but [Premake](https://premake.github.io/) is also a pretty common choice.