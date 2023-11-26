# Opinions

**Trigger warning:** this documents contains my personal opinions.
Take everything here with a grain of salt and feel free to disagree!
Your milage may vary.

## `struct` vs. `class`

There's hardly any difference whether you use the keyword `struct` or `class` for defining a type.
My rule of thumb is to use `class` when the type has non-public members, or special copy / move semantics.
The keywords `protected`, `private`, `delete` are good indicators for using `class`.

Also, put the `public` part first, then `protected`, then `private`.

## Member function vs. regular function

Often you have the choice of implementing something as a member function or a regular function.
My rule of thumb is to create a member function whenever the functionality is related **primarily** to **one instance** of the type (e.g. `Vector2::length`)
If the implementation primarily deals with 2 (or more) instances of the same type, I prefer a regular function (e.g. `lerp` for `Vector2`).

Sometimes I opt for a static member function instead of a regular function when having the typename as prefix for the function call increases readability.
Consider `Vector2::dot` vs. just `dot`.

A common alternative to my rule of thumb is to focus on the public interface of a type.
If some functionality can be implemented by using just the public interface of a type, implement that functionality as a regular function. [[C++ Core Guidelines C.4]](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c4-make-a-function-a-member-only-if-it-needs-direct-access-to-the-representation-of-a-class)

## Naming conventions

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

File names do not use upper-case letters to avoid issues with Windows being case-insensitive.
Consider [Canonical Project Structure](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html).

## Where to put things

- If it can be `const`, it should be `const`
- Minimize variable scope
  - Prefer local variables
  - Prefer static locals to global variables
  - Hide global variables inside source files if possible
- Parameters
  - Take primitives by value
  - Take structs and classes by `const&`
  - Avoid output parameters
  - Use sensible return types
- Heap allocation
  - Avoid unnecessary heap allocations
  - Avoid using `new` / `delete` or `malloc` / `free`
  - Prefer `std::unique_ptr` over `std::shared_ptr`
  - Use containers when you have multiple elements
    - `std::array` and `std::vector` should be your default containers
    - Prefer `std::array` over native arrays

## On code duplication

The urge to minimize code duplication is often thrown around when people talk about *clean* code.
Code duplication **can** indeed be a source for bugs, but not every form of code duplication is equally problematic or avoidable.
I tend to distinguish between 3 different categories:

- **Avoidable code duplication:** is code that is duplicated because it was easier to copy-paste an already existing code snippet and modify it slightly without putting a little bit of thought into it.
  This form of code duplication is characterized by you **always** being better off removing the duplication by refactoring the code.

- **Unavoidable code duplication:** is a form of code duplication (or boilerplate code) that we cannot avoid because of technical reasons.
  This commonly happens when our programming language or framework is missing some crucial feature (e.g. reflection).
  While ways to work around these limitations may exist, they often create more problems and are commonly not worth the additional complexity.
  Here, the tradeoff is in favor of keeping the code duplication over introducing a new, intrusive mechanism that mitigates the duplication.
  
  Note: when code duplication is unavoidable, you can still make an effort to minimize the actual lines of code needed for each duplication.
  I'd rather duplicate a single function call than a 30-line block of code, at each location.

- **Critical code duplication:** is code that is duplicated because the current architecture doesn't allow or support a from where less duplication is possible.
  Here, one has to inspect the situation thoroughly and weigh whether it'd be better to keep the current architecture and accept code duplication, or to change the architecture allowing the duplication to be removed.
  The bottom line is: code duplication is bad, but picking the wrong abstraction is far worse.

## On singletons

Singletons are a controversial design pattern that are often used without putting a lot of thought into them.
Usually they are used because of (one of) the following reasons:

- It doesn't make sense to have multiple instances of this class
- We need a way to access this component from multiple locations across the code-base

Furthermore, the canonical singleton pattern uses lazy initialization, meaning the instance is constructed on first use.
This way, the initialization order of singletons may not be defined statically.

Here's my idea of breaking these things apart into something that is more sensible:

- If multiple instances don't make sense, don't use a class.
  Use a dedicated namespace where functions mutate global variables hidden inside a source file.
  This way you just call regular functions to mutate global state.
  Provide an `initialize()` and `finalize()` function for well defined setup and teardown.
  Implementation details can be hidden inside the source file contrary to using a class where private members are still visible in the header file.

- When you only need easy access to an instance, but there is no real problem with having multiple instances, just provide a global default instance.
  I prefer to use `std::optional` for the default instance, because it forces me to initialize it explicitly and gives me the option to destroy default instances in a custom order.

Note that neither of these approaches are thread-safe — which is fine because you should initialize them at the start of your application before launching additional threads.

## Compile times ain't the issue

Suffering from long compile-times?
Use pre-compiled headers and avoid unnecessary uses of templates.

For reference, the source code for [The Legend of Heroes: Trails through Daybreak](https://store.steampowered.com/app/2138610/The_Legend_of_Heroes_Trails_through_Daybreak/) is around 1M lines of modern C++ code. The game code + engine code + dependencies compile on my AMD Ryzen 9 7950X in around 30 seconds — no cache, with optimizations enabled.
Recompilation, when changing only a single translation unit, is typically under 2 seconds.

I therefore believe that long compile times are not inherent to using C++.

## Build systems suck

Yes, they do.
[CMake](https://cmake.org/) sucks, [Premake](https://premake.github.io/) maybe a bit less.
However, even CMake sucks less than ruling your own, custom build system because most C++ programmers and maintainers know the oddities and issues of CMake.
People don't have any experience with your custom build system and don't know about its pitfalls and surprising behavior.

> Better the devil you know than the one you don't.

## Dependency management sucks

To be fair, I do like that not every C++ projects comes with a million third-party dependencies like your typical JavaScript project.
But granted, given how many basic use-cases are not covered by the standard library, it's hard to go without any third-party library.

While [vcpkg](https://vcpkg.io/) and [Conan](https://conan.io/) are the go-to package managers for C++ nowadays, I've encountered issues with both of them.
Most specifically it is a hassle to develop a patch for one of your dependencies and have your project use the modified dependency instead of the original version (see [Conan#13743](https://github.com/conan-io/conan/issues/13743#issuecomment-1794414576) for reference).

My current approach, as stupid as it sounds, is to include the sources of third-party dependencies along with the project (i.e. vendoring) and compile them along with the project using custom build instructions.
This works surprisingly well for most dependencies and allows me to define exactly how the dependencies are built.
A major downside of this approach is that you have no way to upgrade a dependency automatically, rendering this approach impractical for security critical projects.
For dependencies that are super complicated to build, I build them separately with the original build system and add the build outputs to my project.

I may come back to vcpkg and Conan at some point in the future though.

## The standard library sucks

Well, that depends, but yes, some parts are certainly problematic.
Still, it's a better idea to pick the parts that do work than completely ruling your own.
Like with build systems, most C++ programmers are aware of the standard library's features and shortcomings.
And chances are pretty high that your custom re-implementation will have similar, or even worse, shortcomings thanks to C++ being such a complex language.

So, unless you have a *really* good point not to use the standard library, just use the standard library!

## What about C-style C++?

People often throw around the term C-Style C++ or [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) when talking about which language features to use or to avoid.
To me, these discussions often seem like a straw man argument that come from a lack of understanding.
Sure, there are lots of issues with C++, but many features can improve code quality.

For instance, I often hear people recommending `printf` over `std::cout` because it's *more natural* or more similar to other language's printing mechanism.
What's often missing from this discussion is `printf`'s lack of type-safety; `printf` completely trusts the format string you provide and expects arguments to match up with the format specifiers (e.g. `%s`).
Accidentally mixing up the argument list often results in undefined behavior, crashing the program — if you are lucky!
With `std::cout` this mistake cannot happen; the compiler enforces that every type you feed into `std::cout` is printable, otherwise you get an error **at compile-time**.

Similar problems arise when using `malloc` / `free` over smart pointers.
Yes, smart pointers are not a silver bullet; you can still break your program with them.
But it's typically harder to do so for the average C++ programmer, compared to doing manual memory management.
Rust adopting the concept of RAII is a good indicator that it's worth wrapping your head around.

So, when you decide for or against using certain language features, don't blindly accept someone else's recommendation.
Base your decision on a clear understand of the benefits and drawbacks.
If you think you are missing something, ask your peers why, or why not to use the feature and really pressure for a **technical** answer.

## Modules are dead on arrival

C++ modules were standardized with the C++20 standard.
3 years later, C++ modules are still not usable in general; many third-party libraries don't provide them and even compiler support is incomplete.

Also, at this point I don't entirely understand what's the point:

- Compile times are not really an issue as outlined above.
- Preprocessor macros leaking from third-party headers is annoying but we've been dealing with that for decades now.
  Prefixing macros with a project specific identifier is an adequate solution.
    - Client code that is macro dependent (e.g. code using win32 API) still needs these macros, C++ modules can't change that.
    - Certain operations still cannot be implemented in the base language, but require the use of macros (e.g. [TRY](https://boostorg.github.io/outcome/tutorial/essential/result/try.html)).
- Module names are not coupled to namespaces like it's done in most other languages.
- Modules are completely unrelated to any form of dependency management or packaging structure.
- An internal (i.e. un-exported) type can be exposed by an exported function.
  [In Rust, this is an error.](https://play.rust-lang.org/?version=stable&mode=debug&edition=2021&gist=5561de88a30de336b6727bd15497645a)

Overall, modules appear to just add a ton of complexity to the C++ infrastructure (compilers, IDEs, package managers, etc.) without solving a real issue.
