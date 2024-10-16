# Opinions

**Trigger warning:** this documents contains my personal opinions.
Take everything here with a grain of salt and feel free to disagree!
Your milage may vary.

## Naming Conventions

In C++ we have a bunch of different naming conventions.
Chances are that you'll be either working on a personal project where it doesn't matter which convention you use, or you'll be working on an existing project where you have to adhere to whatever is already used.
Anyway, here are my personal preferences in case you are interested:

- Use *camel-case* as default (e.g. `configFile`, `loadConfig`)
- Capitalize constants, enum members, types, type parameters, concepts, and namespaces (e.g. `ConfigManager`, `ConfigPath`)
- Use *all-caps* for defines / macros along with a project specific prefix (e.g. `ANKER_DEBUG_BREAK`)
- Function names should start with a verb as they *do* something (e.g. `loadConfig`)
- Add adjectives to variables to convey additional information (e.g. `activeUser` instead of `user`)

Certain identifiers are prefixed to make their *impact* easily recognizable in code:
- Prefix global variables with `g_`
- Prefix non-public member variables with `m_`
- Prefix static member variables with `sm_`
- Prefix interface types with `I` (e.g. `IAudioServer`)

For filenames:
- Use lowercase letters with underscores
- Add a project specific prefix
- Use `.hpp` and `.cpp` for C++ code
- Use `.h` and `.c` for C code
- Example: `anker_config_manager.hpp`

Consider [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html).

## Where to put Things

- Minimize variable scope
  - Prefer local variables
  - Prefer static locals to global variables
  - Hide global variables inside source files if possible
- Parameters
  - Take primitives by value
  - Take structs and classes by `const&`
  - Prefer return values to output parameters
- Heap allocation
  - Avoid unnecessary heap allocations
  - Avoid using `new` / `delete` or `malloc` / `free`
  - Prefer `std::unique_ptr` over `std::shared_ptr`
  - Use containers when you have multiple elements
    - `std::array` and `std::vector` should be your default containers
    - Prefer `std::array` over native arrays

## Constness

In general, the fewer things are mutable, the better.
Especially long-living variables should be marked `const` whenever possible.

I prefer to omit the `const` for parameters and short-lived variables to ease readability.
However, marking them `const` as well is technically better.

`const` should not be used when returning values or for non-static member variables (prefer a getter here).

## `struct` vs. `class`

There's hardly any difference whether you use the keyword `struct` or `class` for defining a type.
My rule of thumb is to use `class` when the type has non-public members, or special copy / move semantics.
The keywords `protected`, `private`, `delete` are good indicators for using `class`.

Also, put the `public` part first, then `protected`, then `private`.

## On Code Duplication

The urge to minimize code duplication is often thrown around when people talk about *clean* code.
Code duplication **can** indeed be a source of bugs, but not every form of code duplication is equally problematic or avoidable.
I tend to distinguish between 3 different categories:

- **Avoidable code duplication:** is code that got duplicated primarily because of the author's laziness.
  Copy-paste was easier than putting a little more thought into it.
  This form of code duplication is characterized by you **always** being better off refactoring the code, removing the duplication.

- **Unavoidable code duplication:** is a form of code duplication (or boilerplate code) that we cannot avoid because of technical reasons.
  This commonly happens when our programming language or framework is missing some crucial feature (e.g. reflection).
  While ways to work around these limitations may exist, they often create more problems and are commonly not worth the additional complexity.
  Here, the tradeoff is in favor of keeping the code duplication over introducing a new, intrusive mechanism that mitigates the duplication.
  
  Note: when code duplication is unavoidable, you can still make an effort to minimize the actual number of lines needed for each duplication.
  I'd rather duplicate a single function call than a 30-line block at each location.

- **Critical code duplication:** is code that is duplicated because the current architecture doesn't allow or support a form where less duplication is possible.
  Here, one has to inspect the situation thoroughly and weigh whether it'd be better to keep the current architecture and accept code duplication, or to change the architecture allowing the duplication to be removed.
  The bottom line is: code duplication is bad, but picking the wrong abstraction is far worse.

## Member Function vs. Regular Function

Often you have the choice of writing a function as a member function or a regular (free-standing) function.
My rule of thumb is to create a member function whenever it is related **primarily** to **one instance** of the type (e.g. `Vector2::length`)
If the implementation primarily deals with 2 (or more) instances of the same type, I prefer a regular function (e.g. `lerp` for `Vector2`).

Sometimes I opt for a static member function instead of a regular function when having the typename as prefix for the function call increases readability.
Consider `Vector2::dot` vs. just `dot`.

A common alternative to my rule of thumb is to focus on the public interface of a type.
If some functionality can be implemented by using just the public interface of a type, implement that functionality as a regular function. [[C++ Core Guidelines C.4]](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c4-make-a-function-a-member-only-if-it-needs-direct-access-to-the-representation-of-a-class)
I dislike this approach as changes to the public interface of a type quickly cascade to functions outside the class.

## On Singletons

I quite dislike the canonical singleton pattern in C++ where the singleton instance is created lazily on first use (typically from a static local).
The issue is that this gives you barely any control over the construction and destruction of the instance and its members.
Instead, I prefer to make the singleton instance accessible directly by exposing it as a global variable.

However, I also think that the singleton pattern is overused in general and that the common requirements are orthogonal:

- It doesn't make sense to have multiple instances of this class; vs.
- We need a way to access this component from multiple locations across the code-base

In [`example2`](example2/) you can find a logger and platform abstraction mechanism.
The logger mechanism is not a singleton, there's just a default instance.
The platform abstraction is realized as a singleton with dedicated setup and teardown functions, as well as the ability to use a mock implementation for testing.

Thread-safety is generally not an issue here since all such components should be set up at program start, by the main thread, in a well defined order.

## On Exceptions

Check out [Error Handling](error_handling.md) for my rambling about exceptions.

## Compile Times ain't the Issue

Suffering from long compile-times?
Use pre-compiled headers and avoid unnecessary uses of templates.

For reference, the source code for [The Legend of Heroes: Trails through Daybreak](https://store.steampowered.com/app/2138610/The_Legend_of_Heroes_Trails_through_Daybreak/) is around 1M lines of modern C++ code. The game code + engine code + dependencies compile on my AMD Ryzen 9 7950X in around 30 seconds — no cache, with optimizations enabled.
Recompilation, when changing only a single translation unit, is typically under 2 seconds.

I therefore believe that long compile times are not inherent to using C++.

## Build Systems suck

Yes, they do.
[CMake](https://cmake.org/) sucks, [Premake](https://premake.github.io/) maybe a bit less.
However, even CMake sucks less than ruling your own, custom build system because most C++ programmers and maintainers know the oddities and issues of CMake.
People don't have any experience with your custom build system and don't know about its pitfalls and surprising behavior.

> Better the devil you know than the one you don't.

## Dependency Management sucks

To be fair, I do like that not every C++ projects comes with a million third-party dependencies like your typical JavaScript project.
But granted, given how many basic use-cases are not covered by the standard library, it's hard to go without any third-party library.

While [vcpkg](https://vcpkg.io/) and [Conan](https://conan.io/) are the go-to package managers for C++ nowadays, I've encountered issues with both of them.
Most specifically it is a hassle to develop a patch for one of your dependencies and have your project use the modified dependency instead of the original version. [[Conan#13743]](https://github.com/conan-io/conan/issues/13743) [[vcpkg#16874]](https://github.com/microsoft/vcpkg/issues/16874)

My current approach, as stupid as it sounds, is to include the sources of third-party dependencies along with the project (i.e. vendoring) and compile them along with the project using custom build instructions.
This works surprisingly well for most dependencies and allows me to define exactly how the dependencies are built.
A major downside of this approach is that you have no way to upgrade a dependency automatically, rendering this approach impractical for security critical projects.
For dependencies that are super complicated to build, I build them separately with the original build system and add the build outputs to my project.

I may come back to vcpkg at some point in the future though.
In the meantime, you can have a look at this example [`build_sdl2.rb`](https://github.com/W4RH4WK/Anker/blob/72851cb06c0027d925ca5ab9d6995459914f4a4a/external/sdl2/build_sdl2.rb) which builds SDL2 and copies the relevant files into my project.

## The Standard Library sucks

Well, that depends, but yes, many parts are certainly problematic.
Still, it's a better idea to pick the parts that do work than completely ruling your own.
Like with build systems, most C++ programmers are aware of the standard library's features and shortcomings.
And chances are pretty high that your custom re-implementation will have similar, or even worse, shortcomings thanks to C++ being such a complex language.

So, unless you have a *really* good point not to use the standard library, just use the standard library!

## What about C-style C++?

People often throw around the term C-Style C++ or [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) when talking about which language features to use or to avoid.
To me, these discussions often seem like a straw man argument that come from a lack of understanding.
Sure, there are lots of issues with C++, but some features can improve code quality.

For instance, I often hear people recommending `printf` over `std::cout` because it's *more natural* or more similar to other language's printing mechanism.
What's often missing from this discussion is `printf`'s lack of type-safety; `printf` completely trusts the format string you provide and expects arguments to match up with the format specifiers (e.g. `%s`).
Accidentally mixing up the argument list often results in undefined behavior, crashing the program — if you are lucky!
With `std::cout` this mistake cannot happen; the compiler enforces that every type you feed into `std::cout` is printable, otherwise you get an error **at compile-time**.

Similar problems arise when using `malloc` / `free` over smart pointers.
Yes, smart pointers are not a silver bullet; they can still result in leaks and can degrade performance in certain situations, like releasing a chain of smart pointers.
However, the average C++ programmer causes more issues doing manual memory management; so you are likely better off using smart pointers.

So, when you decide for or against using certain language features, don't blindly accept someone else's recommendation.
Base your decision on a clear understanding of the benefits and drawbacks.
If you think you are missing something, ask your peers why, or why not to use the feature and really pressure for a **technical** answer.

## Modules are Dead on Arrival

C++ modules were standardized with the C++20 standard.
3 years later, C++ modules are still not usable in general; [many third-party libraries don't provide them](https://arewemodulesyet.org/) and even compiler support is incomplete.

Also, at this point I don't entirely understand what's the point:

- Compile times are not really an issue as outlined above.
- Preprocessor macros leaking from third-party headers is annoying but we've been dealing with that for decades now.
  Prefixing macros with a project specific identifier is an adequate solution.
    - Client code that is macro dependent (e.g. code using win32 API) still needs these macros, C++ modules can't change that.
    - Certain operations still cannot be implemented in the base language, but require the use of macros (e.g. [TRY](https://boostorg.github.io/outcome/tutorial/essential/result/try.html)).
- Module names are not coupled to namespaces like it's done in most other languages.
- Modules are completely unrelated to any form of dependency management or packaging structure.
- An internal (i.e. un-exported) type can be exposed by an exported function.
  [In Rust, this is an error.](https://play.rust-lang.org/?version=stable&mode=debug&edition=2021&gist=30516d465ebea8c8dbb7dec5b8ef1cf8)

Overall, modules appear to just add a ton of complexity to the C++ infrastructure (compilers, IDEs, package managers, etc.) without solving a real issue.
