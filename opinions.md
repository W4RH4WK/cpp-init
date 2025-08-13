# Opinions

**Trigger warning:** this document contains my personal opinions.
Take everything here with a grain of salt and feel free to disagree!
Your milage may vary.

## Naming Conventions

Different naming conventions exist in the C/C++ world.
Chances are that you'll be working on a personal project, where the convention doesn't, or you'll be working on an existing project, where a convention is already in place.
Anyway, here are my personal preferences in case you are interested:

- Use *camel-case* as default (e.g. `configFile`, `loadConfig`)
- Capitalize non-local constants, enum members, types, type parameters, concepts, and namespaces (e.g. `ConfigManager`, `ConfigPath`)
- Use *all-caps* for defines / macros along with a project specific prefix (e.g. `ANKER_DEBUG_BREAK`)
- Function names should start with a verb as they *do* something (e.g. `loadConfig`, `calcBoundingBox`)
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

- Prefer local variables and parameters to global variables
- Prefer global variables to static locals
  - Hide global variables inside source files **iff** the API communicates clearly that there is internal state.
    This is commonly the case for platform abstractions.
- Parameters
  - Take primitives by value
  - Take structs and classes by `const&`
  - Prefer return values to output parameters
- Heap allocation
  - Avoid unnecessary heap allocations
  - Avoid using `new` / `delete` or `malloc` / `free` for single objects
  - Prefer `std::unique_ptr` over `std::shared_ptr` if you use smart pointers
  - Use containers when you have multiple elements
    - `std::array` and `std::vector` should be your default containers
    - Prefer `std::array` over native arrays

## Constness

In general, the fewer things are mutable, the better.
Especially long-living variables should be marked `const` whenever possible.

I prefer to omit the `const` for parameters and short-lived variables to ease readability.
However, marking them `const` as well is technically safer.

`const` should not be used when returning values or for non-static member variables (consider a getter instead).

I currently prefer east-const (`Foo const&`) over west-const (`const Foo&`) since west-const is incompatible with abbreviated function templates using concepts — the `auto` keyword has to follow the concept identifier.

## `struct` vs. `class`

There's hardly any difference whether you use the keyword `struct` or `class` for defining a type in C++.
My rule of thumb is to use `class` when the type has non-public members, or special copy / move semantics.
The keywords `protected`, `private`, `delete` are good indicators for using `class`.

Also, put the `public` part first, then `protected`, then `private`.

## Object Oriented Programming

Object oriented programming (OOP) is a controversial programming paradigm that is often imposed on novices.
While OOP appears simple and straightforward, there are many pitfalls and code typically becomes less maintainable and efficient than what OOP advocates make you believe.
In OOP, you translate your application's semantics into objects, which fulfill the application's purpose by interacting with each other.

The problematic part here is the *oriented* part, not the *object* part.
An object is defined as some data (i.e. state), think of a C `struct`, with some functions (i.e. methods) attached.
This, in itself, is not an issue, grouping data that belongs together is a fundamental principle and there will always be functions that stand in close relation to this data.

Having the core logic of your application distributed across multiple objects, which then call each other to do something, almost always ends up in a big pile of spaghetti.
OOP often wants you to add **indirections** that seem like a good idea at first, but are actually not needed and only make the code more complex.
Think, how many applications that use a database have a database abstraction layer just to *be able to* swap out the underlying database.
Hardly any of those applications actually need this feature, nor does it work when its actually needed.

Talking about state, with objects, you already split your application state across multiple, probably thousands of objects, all of which potentially mutate their and other object's state during runtime.
Tracking state changes here reliably is practically impossible.
While the program may still be deterministic, it's very hard to reason about such programs.

OOP encourages **encapsulation**, which effectively hides mutable state inside objects, such that it is not directly observable from outside the object.
This is needed to maintain invariants when employing an object oriented architecture, but obscures the application state and state changes even further.
Effectively, many method calls become unpredictable as they depend on internal, invisible state of their and other, referenced objects.
This is descent into madness.

Where OOP encourages having lots of mutable state, and splitting it apart, and hiding it behind encapsulation, **functional programming** tries to minimize mutable state and makes the remaining state explicit and visible.
**Data oriented programming** organizes state in an explicit and more efficient way, where invariants are maintained by operations rather than through encapsulation.

This does not mean that you should not use basic utilities like `std::string` or `std::vector` just because they encapsulate some data.
But, you should re-think your overall state management if you are using OOP principles in your code.
Look into other paradigms, like the mentioned functional programming and data oriented programming, and understand their benefits over OOP.

I suspect the best mix is state organized in a data oriented manner, with operations (i.e. functions, ideally pure) doing the bulk of your program's work, where each function is implemented in a **procedural style**.
Objects (state with functions attached) should be reserved for utilities, which clearly communicate their invariants, but don't hide them completely, e.g. by suffixing members with an `_` instead of making them private.

In general, you should go for simpler code that does just what needs to be done, written in *code* rather than through *gluing objects together*.
Compare [`example2`](example2/) to [`example3`](example3/) for an illustration.

## Code Duplication

The urge to minimize code duplication is often thrown around when people talk about *clean code*.
Code duplication **can** indeed be a source of bugs, but not every form of code duplication is equally problematic or avoidable.
I tend to distinguish between 3 different categories:

- **Avoidable code duplication:** is code that got duplicated primarily because of the author's laziness.
  Copy-paste was easier than putting a little more thought into it.
  This form of code duplication is characterized by you **always** being better off refactoring the code, removing the duplication.

- **Unavoidable code duplication:** is a form of code duplication (or boilerplate code) that we cannot avoid because of technical reasons.
  This commonly happens when our programming language or framework is missing some crucial feature (e.g. reflection).
  While ways to work around these limitations may exist, they can create more problems and are often not worth the additional complexity.
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

## Singletons

I dislike the canonical singleton pattern in C++ where the singleton instance is created lazily on first use (typically from a static local).
This gives you barely any control over the construction and destruction of the instance.
Instead, I prefer to make the singleton instance accessible directly by exposing it as a global variable.

However, I also think that the singleton pattern is overused in general and that the common requirements are orthogonal:

- it doesn't make sense to have multiple instances of this class; vs.
- we need a way to access this component from multiple locations across the code base.

In [`example2`](example2/) you can find a logger and platform abstraction mechanism.
The logger mechanism is not a singleton, there's just a default instance.
The platform abstraction is realized as a singleton with dedicated setup and teardown functions, as well as the ability to use a mock implementation for testing.

Thread-safety is generally not an issue here since all such components should be set up at program start, by the main thread, in a well defined order.

## Dynamic Initialization

Dynamic initialization is a C++ feature that looks neat at first glance but creates a tremendous amount of issues, especially for portable code.
My recommendation is to avoid it entirely!
C++20 introduced the `constinit` specifier, which enforces static initialization on global variables, which is strongly preferred.
Wrapping globals into a null-able type (e.g. `std::optional` or `std::unique_ptr`) is an acceptable alternative.

Like with singletons, global object construction should happen in a well defined order at an explicit point during program initialization.

### Dynamic De-Initialization

While not as problematic as dynamic initialization, destructors called in *on-exit* handlers can certainly be problematic as well.
Here, neither `constinit` nor conventional null-able C++ types work — `std::optional`'s destructor will destroy the held object if there is one.
To work around this, either dynamically allocate the instance and reference it using a raw pointer.
Or create yourself a wrapper type, similar to `std::optional`, but one that does not destroy the internal object on destruction automatically.

The goal is to destroy global instances in a well defined order, explicitly during program shutdown (i.e. de-initialization).
Leaking them on unexpected application shutdown (e.g. `std::abort`) is typically less problematic.

## Exceptions

Check out [Error Handling](error_handling.md) for my rambling about exceptions.

## Compile Times ain't the Issue

Suffering from long compile-times?
Use pre-compiled headers and avoid unnecessary uses of templates.

For reference, the source code for [The Legend of Heroes: Trails through Daybreak](https://store.steampowered.com/app/2138610/The_Legend_of_Heroes_Trails_through_Daybreak/) is around 1M lines of modern C++ code.
The game code + engine code + dependencies compile on my AMD Ryzen 9 7950X in around 30 seconds — no cache, with optimizations enabled.
Recompilation, when changing only a single translation unit, is typically under 2 seconds.

I therefore believe that long compile times are not inherent to using C++.

## Build Systems suck

Yes, they do.
[CMake](https://cmake.org/) sucks, [Premake](https://premake.github.io/) maybe a bit less.
However, even CMake sucks less than ruling your own, custom build system because most C++ programmers and maintainers know the oddities and issues of CMake.
People don't have any experience with your custom build system and don't know about its shortcomings and surprising behavior.

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
Like with build systems, most C++ programmers are aware of the standard library's problems.
And chances are pretty high that your custom re-implementation will have similar, or even worse, shortcomings thanks to C++ being such a complex language.

So, unless you have a *really* good point not to use the standard library, of which there are many, just use the standard library!

## What about C-style C++?

Generally, you should only use the parts of a language you have a good understanding of -- ideally, only the parts you could implement yourself.
For C++, you should have a good understanding of a feature's benefits and drawbacks.
If you don't want to put up with that, just stick to C.

People often throw around the term C-Style C++ or [Orthodox C++](https://gist.github.com/bkaradzic/2e39896bc7d8c34e042b) when talking about which language or standard library features to use or avoid.
Yet, they often neglect a meaningful justification or do not present the actual trade-off adequately.

As an example, some people recommend `printf` over `std::cout` because it's *more natural* or more similar to other language's printing mechanism.
And while C++ streams' API sucks, what's overlooked is `printf`'s lack of type-safety; `printf` completely trusts the format string you provide and expects arguments to match up with the format specifiers (e.g. `%s`).
Accidentally mixing up the argument list often results in undefined behavior, crashing the program — if you are lucky!
With `std::cout` this mistake cannot happen; the compiler enforces that every type you feed into `std::cout` is printable, otherwise you get a **compilation error**.

Similar problems arise when using `malloc` / `free` over smart pointers.
Yes, smart pointers are not a silver bullet; they can still result in leaks and can degrade performance in certain situations, like releasing a chain of smart pointers.
However, the average C++ programmer causes more issues doing manual memory management for single objects; so you are likely better off using smart pointers.

Likewise, using `std::string` is usually less error-prone than fiddling around with the C standard library's string API.

So, when you decide for or against using certain language features, don't blindly accept someone else's recommendation.
Base your decision on a clear understanding of the benefits and drawbacks.
If you think you are missing something, ask your peers why, or why not to use the feature and demand a **technical** answer.

## Modules are Dead on Arrival

C++ modules were standardized with the C++20 standard.
4 years later, C++ modules are still not usable in general; [many third-party libraries don't provide them](https://arewemodulesyet.org/) and even compiler support is incomplete.

Also, at this point, what's the point even:

- Compile times are not really an issue as outlined above.
- Preprocessor macros leaking from third-party headers is annoying but we've been dealing with that for decades now.
  Prefixing macros with a project specific identifier is an adequate solution.
    - Client code that is macro dependent (e.g. code using win32 API) still needs these macros, C++ modules can't just do away with that.
    - Certain operations still cannot be implemented in the base language, but require the use of macros (e.g. [TRY](https://boostorg.github.io/outcome/tutorial/essential/result/try.html)).
- Module names are not coupled to namespaces like it's done in most other languages.
- Modules are completely unrelated to any form of dependency management or packaging structure.
- An internal (i.e. un-exported) type can be exposed by an exported function.
  [In Rust, this is an error.](https://play.rust-lang.org/?version=stable&mode=debug&edition=2021&gist=30516d465ebea8c8dbb7dec5b8ef1cf8)

Overall, modules appear to just add a ton of complexity to the C++ language and infrastructure (compilers, IDEs, build systems, package managers, etc.) without solving a real issue.
