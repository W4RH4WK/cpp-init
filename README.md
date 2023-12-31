# How to Start C++ Development?

Since I encounter this question a lot lately, here's a summary of my typical answer along with some pointers.

## Should I learn C++?

Depends on what your goal is.
C++ is certainly not dead, but better alternatives may exist, depending on your use-case.
Consider [Go](https://go.dev/) and [Rust](https://www.rust-lang.org/) before deciding to venture forth into C++.

Still here? Alright.

## Where do I start?

- First, get familiar with C.
  For this I recommend [Beej's Guide to C Programming](https://beej.us/guide/bgc/).

- Once you are somewhat fluent with C, you may move on to C++.
  [Learn C++](https://www.learncpp.com/) is an excellent guide that has you covered.

- If you need technical information on a language feature or library component, consult [C++ reference](https://cppreference.com/).

- Once you are somewhat familiar with C++, read through the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines) and see what makes sense for you.
  This also gives you some idea on how certain language features are supposed to be used.
  You may also be interested in [my personal opinions](opinions.md).

- Pick up [Git](https://git-scm.com/) along the way.
  Strictly speaking, Git is not necessary for C++, yet any modern programmer should be familiar with Git and use it in their projects.

- Pick up a scripting language like [Python](https://www.python.org/) or [Ruby](https://www.ruby-lang.org/) at some point as well.
  Certain tasks are better suited for a scripting language, knowing at least one will increase your productivity immensely.
  Oh, and stay away from JavaScript!

The most important aspect is that you get your hands dirty and start building things early on.
Theory is important, but practice is still paramount!

## Which Topics are Essential?

C++ is a complex language that uses various paradigms and conventions.
When wielding C++, you should understand these topics:

- Value semantics
    - Value vs. reference/pointer
    - Constructor/destructor
    - Copy/move semantics
    - [Rule of five / zero](https://en.cppreference.com/w/cpp/language/rule_of_three)
- `const`-correctness
- Ownership & lifetime
    - Scope-based resource management (i.e. RAII)
- Dynamic memory:
    - Avoid `new` / `delete`
    - Prefer `std::unique_ptr` to `std::shared_ptr`
    - Pick the right container (see sequential vs. associative container)
- Common bugs:
    - Use of uninitialized variables
    - Use-after-free
    - Iterator invalidation
    - Undefined behavior
- Compilation process (preprocessor, translation units, linking, etc.)
    - Enable warnings
    - Enable debug symbols
    - Enable optimizations
    - Sanitizers (most notably address sanitizer)
- Using a debugger

## Which Development Environment?

### Windows

- Use [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/community/)
    - Install with *Desktop development with C++*
    - When creating a new project, select *Empty Project (C++)*
    - From here, just add a source file and you are good to go

### Linux

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

Whenever you have a problem or question, create a small example of your issue in Compiler Explorer and provide a short link (top-right *Share*) to it along with your question.

## Should I use a Build System?

In general, yes.
A build system organizes and documents the build process and allows other tools, like IDEs, to correctly handle your project (e.g. auto-completion).

My personal preference is [CMake](https://cmake.org/), but [Premake](https://premake.github.io/) is also a pretty common choice.
There is a more advanced example available in [`example2`](example2/), featuring multiple CMake targets, tests, benchmarks, and dependencies.

## Where can I find exercises for beginners?

For this, I recommend [Advent of Code](https://adventofcode.com/events).
It's an annual advent calendar where you find a programming exercises behind each day.
The exercise is not specific to any programming language.
You can pick exercises from previous years and look up solutions on GitHub if you get stuck.

## Should I use AI assisted tooling?

Of course you can have conversations with ChatGPT (or similar) about C++ related topics, just always be critical of the answers you get.
However, AI assisted tooling should **not** be a part of your workflow at this point.

## Trouble Shooting

- Try to solve the issue yourself first.
  Use search engines and message boards in combination with logical thinking.
  Only after spending some time on the problem, you are pardoned to ask for help.

- Provide detailed information:
  1. State what exactly you are trying to achieve.
     A clear definition is important to rule out an [XY problem](https://en.wikipedia.org/wiki/XY_problem).
  2. State what you have done so far and describe your current state.
  3. Describe exactly which operation is causing you trouble.
     Provide information on what you are doing (e.g. which command you are running), what the error is (e.g. console output of the program), and what outcome you'd expect.
  4. Shortly state which other approaches you have already tried.
  5. Add auxiliary information that you deem important, like which operating system and software versions you are using.

If you are dealing with a C/C++ related issue:
- Enable and fix warnings in your code (`-Wall -Wextra` at least on GCC / Clang)
- Build and run your application with address sanitizer enabled
- Put together a minimum (not) working example — preferably on [Compiler Explorer](https://godbolt.org/)
