# Error Handling

Error handling is one of my personal pain points in C++.
There is no consistent mechanism that is used across different libraries and platforms.
Each of the available mechanisms comes with some drawbacks.

## Exceptions

Exceptions are likely the most suggested form of error handling in C++.
At the same time, it's also my least preferred way of doing error handling:

- A function signature does not tell you whether (or which) exceptions are thrown by the function.
  There is the `throws` keyword, but this is considered bad practice as it is not enforced at all.
  `noexcept` only tells you that a function is not supposed to throw an exception; if that function does throw, the program terminates immediately.

- The compiler cannot tell whether all possible exceptions, a function can throw, are handled.
  To be safe, one has to employ catch-all (`catch (...)`) everywhere.

- In C++, anything can be thrown.
  While derivatives of `std::exception` are highly encouraged, `throw 42` is valid C++.

- The guidelines say, exceptions are for _exceptional_ behavior — without giving a clear definition of what is _exceptional_ and what is not.
  Either way, this means that there should be some other mechanism to communicate errors, apart from throwing exceptions.

- Exceptions can be turned off with a compiler flag, which can lead to undefined behavior.

The primary benefit of exceptions is that they can be thrown in a constructor to indicate construction failure.
This cannot be mimicked with status codes or output parameters, making by-the-book RAII a bit trickier to implement.

Note that even with exceptions there's no guarantee that an object will not end up in an invalid state.
While an object may have been constructed successfully, one of its member functions could still throw at some point, leaving the object in an invalid state.

A second benefit that should be pointed is that exceptions can carry arbitrary data.

Over all, exceptions do not appear to be the right tool for building robust, predictable systems.
Furthermore, they likely increase code complexity as they only cover _exception_ error cases.

## Status Code

Returning a status code to indicate success or failure is a common practice in C and has been shown to _scale_ well.
The main downside here is that we need another way to return the actual result if a function's return value is used for the status code.
Output parameters are probably the easiest way to solve this, as they do not need any additional language features.

You can get a long way by just using `bool` as return type, where `true` indicates success and `false` indicates failure.
Although, it may come in handy to communicate the kind of error that occurred.
Some APIs use an integer or dedicated enum for this directly, but this has the disadvantage of `0` being used for success and values `!= 0` indicating failure — which is inverted boolean logic.

To implement status codes, I prefer a project-wide enum listing all relevant errors, which is then wrapped for convenience.

```c++
enum StatusCode : u8 {
    Ok = 0,
    UnknownError,
    NotImplementedError,
    InvalidArgumentError,
    ReadError,
    WriteError,
    FormatError,
    // …
};

struct [[nodiscard]] Status {
    constexpr Status() = default;
    constexpr Status(StatusCode code) : code(code) {}

    constexpr explicit operator bool() const { return code == Ok; }

    // <other convenience functions>    

    StatusCode code = Ok;

    friend constexpr bool operator==(const Status&, const Status&) = default;
};
```

The main reason for `Status` is to invert the boolean evaluation; success being `true`, failure being `false`.
Other convenience functions may include `toString` to make errors human readable.

Note that the whole struct is marked `[[nodiscard]]`.
This way, any call to a function that returns a `Status` emits a warning if the return value is not handled.

Depending on how we proceed with error reporting (see below), we may want to include a `std::string` here for additional context.

Here's how this mechanism is used.

```c++
Status IDataLoaderSource::load(ByteBuffer&, const fs::path&) const;

Status DataLoader::load(ByteBuffer& outBuffer, const fs::path& filepath) const
{
    for (IDataLoaderSource* source : m_sources) {
        if (source->exists(filepath)) {
            return source->load(outBuffer, filepath);
        }
    }
    return ReadError;
}

Status AudioTrack::load(std::string_view identifier)
{
    ByteBuffer buffer;
    if (Status status = g_dataLoader.load(buffer, std::string(identifier) + ".opus"); !status) {
        return status;
    }

    // Use audio track data loaded into buffer.

    return Ok;
}
```

In the last function we see a heavily used pattern: continue on success, bail on failure.
Some people may even opt for abstracting this behind a macro.

```c++
#define TRY(expr) \
    do { \
        if (auto status = (expr); !status) [[unlikely]] { \
            return status; \
        } \
    while (0)

Status AudioTrack::load(std::string_view identifier)
{
    ByteBuffer buffer;
    TRY(g_dataLoader.load(buffer, std::string(identifier) + ".opus"));

    // Use audio track data loaded into buffer.

    return Ok;
}
```

Some people may find it irritating that the output parameter is not immediately recognizable on the call-site.
My convention is to have (non-optional) output parameters at the beginning.
Alternatively one can introduce a minimal wrapper for the reference to explicitly annotate the output parameter.

```c++
ByteBuffer buffer;
TRY(g_dataLoader.load(out(buffer), std::string(identifier) + ".opus"));
```

Let's talk about the disadvantages of status codes.

- Return value is occupied with status ⟶ need to use output parameters.
    - This means that an instance of the _actual_ result (`ByteBuffer` in the above example) has to be constructed before the function is called.
    - That constructed value continues to exist even when the function fails.
      It may even be in an invalid state!

- This mechanism cannot be used to communicate object construction failure.
  There are some possible workarounds though:
    - Static factory function that returns `Status` and initializes a *null-able* type (e.g. `std::optional`, `std::unique_ptr`) on success via output parameter.
    - Static factory function that returns an object as *null-able* type.
      `Status` is provided via (optional) output parameter.
    - Static factory function that returns a result type (e.g. `std::expected`, see below).
    - Object is default-constructable, but has an `initialize` member function function which returns `Status`.
    - Object is always constructed, but can be in an *invalid* state (e.g. `std::ifstream`).

While the drawbacks are not insignificant, it's worth pointing out that this mechanism:
- does not suffer from the same issues as exceptions do;
- can be used for _exceptional_ and _regular_ failure cases; and
- does not need any recent C++ language addition or third-party library.

## Result Type

Result types are often suggested as the _modern_ solution to error handling.
Yet, I've always found the devil to be in the detail.

The idea is to have a template type that can contain either, the actual return value, or an error object.
Technically `std::variant` would be sufficient, but it lacks ergonomics to its alternatives (e.g. `std::expected`) — which still lack certain ergonomics as well.

```c++
enum ErrorCode : u8 {
    EmptyResult,         // ⟵ No success entry
    UnknownError,
    NotImplementedError,
    InvalidArgumentError,
    ReadError,
    WriteError,
    FormatError,
    // …
};

std::expected<ByteBuffer, ErrorCode> IDataLoaderSource::load(const fs::path&) const;

std::expected<ByteBuffer, ErrorCode> DataLoader::load(const fs::path& filepath) const
{
    for (IDataLoaderSource* source : m_sources) {
        if (source->exists(filepath)) {
            return source->load(outBuffer, filepath);
        }
    }
    return std::unexpected(ReadError);
}

std::expected<void, ErrorCode> AudioTrack::load(std::string_view identifier)
{
    auto result = g_dataLoader.load(buffer, std::string(identifier) + ".opus");
    if (!result) {
        return result;
    }

    // Use audio track data located through result.value() or *result or result->

    return {}; // ⟵ default constructed 'void', indicating success
}
```

This doesn't look too bad, but getting a value out of the result type can be a bit annoying.

```c++
std::expected<void, ErrorCode> AudioTrack::load(std::string_view identifier)
{
    std::expected<ByteBuffer, ErrorCode> result = g_dataLoader.load(std::string(identifier) + ".opus");
    if (!result) {
        return result;
    }

    // Moving the value out of the result, leaves the result in an undefined state.
    // result must not be accessed after this.
    ByteBuffer buffer = std::move(result).value();

    // A better alternative is to use std::exchange.
    // This still moves the value, but the result is set to ErrorCode::EmptyResult, making this safer.
    ByteBuffer buffer = std::exchange(result, std::unexpected(EmptyResult)).value();

    // …
}
```

While the `ByteBuffer` object doesn't have to be constructed beforehand, a lot more moves (or copies) are happening here.
This makes it harder to understand what exactly is happening compared to using an output parameter.

Other programming languages that feature result types typically offer a _try operator_ to unwrap on success and bail on failure.
This can be mimicked with in C++ with a macro, but a temporary is left in the scope.

```c++
#define TRY(assignment, expr) \
    auto result_temporary = (expr); \
    if (!result_temporary) { \
        return std::unexpected(result_temporary.error()); \
    } \
    assignment = std::exchange(result, std::unexpected(EmptyResult)).value()

std::expected<void, ErrorCode> AudioTrack::load(std::string_view identifier)
{
    TRY(ByteBuffer buffer, g_dataLoader.load(std::string(identifier) + ".opus"));
    // …
}
```

There's also an alternative implementation using statement-expressions; however, this is solution is not portable — MSVC doesn't support statement expressions.
The assignment being inside the macro invocation lessens readability.

Think about how to write a function in such a way that (N)RVO kicks in.

Furthermore, implementations like `std::expected` may not be marked `[[nodiscard]]`, which means that every function has to be annotated accordingly.

`std::expected` was introduced with C++23 and has therefore relatively low adoption.
Other result type implementations, like [Boost Outcome](https://www.boost.org/doc/libs/1_85_0/libs/outcome) or [`absl::StatusOr`](https://abseil.io/docs/cpp/guides/status) have been around for a while, but didn't manage to become the go to solution for error handling.

It might also be worth taking a peek at these implementations, they are not trivial!

# Error Reporting

While _Error Handling_ covers the mechanisms to communicate and react programmatically to errors, _Error Reporting_ is about communicating an error, with additional context, to a human being (commonly a developer).

Most end-users do not care about the details of the error, or need contextual information, like in which source file an error originated from.
However, a simple message like `No such file or directory: sfx/explosion.opus` does indeed help the end-user to investigate or resolve the issue.
Still, it's worthwhile to divide error reporting for end-users and developers.
Developers need a lot of technical information when an error happens, where end-users typically do not.

## Attaching Error Messages

This approach works well for exceptions and result type error handling.
Exceptions typically carry a string that contains information about the error, where the error type used for a result type could also contain a string.

This string should be filled immediately when the error occurs with all relevant information present at the location where the error originates from.
Apart from the actual error description, adding source location information (filename, function name, line number) and values of nearby variables is a good idea.

When that exception (and therefore the error message) is passed up the stack, additional information can be appended.
While this is possible, it's a little cumbersome to do.

However, at some point, the exception should be handled and the error message needs to be forwarded somewhere, typically to some logging facility.
Sometimes, that error is communicated to the end-user via a message box, but this is not recommended as outlined earlier.

The approach of attaching error messages to your exceptions or result type is sometimes preferred when creating libraries as the user of the library has direct control over how the error message is handled.

## Logging Directly

As I've discovered how beneficial good logging output can be for fixing bugs, I do prefer to directly log every error the moment it happens.

Whenever an error value is instantiated (e.g. `StatusCode::FormatError`, or an exception is created), I will immediately log an error message.
Logging facilities typically attach source location information automatically, so I only have to worry about the error description and variable values.

While the error is passed up the stack, I may still emit additional log messages to add contextual information.
Technically, in a multi-threaded application these additional log messages could be interleaved with other log messages; however, if your application does not encounter errors regularly (which it shouldn't), this is not a problem.
One could also add the thread ID to the log entry — this way you'd see which log messages belong together.

Since we may already employ a `TRY` macro for the _bail on failure_ paradigm, we can emit log messages here as well.
I use this approach in some command-line utilities to get stack traces on errors without any third-party library or compiler extension.

```c++
#define TRY(expr) \
    do { \
        if (auto status = (expr); !status) [[unlikely]] { \
            ERROR("%s failed with %s", #expr, status.toString()); \
            return status; \
        } \
    while (0)

Status AudioTrack::load(std::string_view identifier)
{
    ByteBuffer buffer;
    TRY(g_dataLoader.load(buffer, std::string(identifier) + ".opus"));
    // …
}
```

This would emit the following log messages on error, here only printing the function name and line number as source location.

```
2024-07-04 11:00:47 Error [DataLoaderSourceFilesystem::load:19] ReadError: Permission denied: "sfx/explosion.opus"
2024-07-04 11:00:47 Error [DataLoader::load:45] source->load(outBuffer, filepath) failed with ReadError
2024-07-04 11:00:47 Error [AudioTrack::load:34] g_dataLoader.load(buffer, std::string(identifier) + ".opus") failed with ReadError
```

### Debug Severity

Log severity levels _Error_, _Warning_, and _Info_ are straightforward, but going beyond _Info_ can be tricky.
In my experience, global _Trace_ (or sometimes named _Debug_ or _Verbose_) logs rarely give you what you need.
Chances are the specific code that you want to investigate doesn't even emit _Trace_ logs.
Additionally, turning _Trace_ logs on globally causes a lot of noise and may even have a significant performance impact as string interpolation is slow.

Instead, I choose to apply _Trace_ logs much more precisely when I need to track down bugs, or to debug obscure program behavior.
While there is a _Trace_ severity in the logger module, _Trace_ logs are always emitted through a _topic specific_ preprocessor macro.
This allows me to enable only verbose logging for the facilities that I need.

```c++
#define EMIT_LOG(severity, ...) ::Log::log(severity, __FUNCTION__, __LINE__, __VA_ARGS__)

#define INFO(...) EMIT_LOG(::Log::Severity::Info, __VA_ARGS__)
#define WARN(...) EMIT_LOG(::Log::Severity::Warning, __VA_ARGS__)
#define ERROR(...) EMIT_LOG(::Log::Severity::Error, __VA_ARGS__)
// no TRACE macro here

#define TRACE_TEXTURE_LOAD(...) EMIT_LOG(::Log::Severity::Error, __VA_ARGS__)
#define TRACE_SFX_LOAD(...) // EMIT_LOG(::Log::Severity::Error, __VA_ARGS__)
#define TRACE_BGM_LOAD(...) // EMIT_LOG(::Log::Severity::Error, __VA_ARGS__)
```

This code shows the definition of some logging macros where _Trace_ logs are split into different topics.
Here _Trace_ logs for texture loading are enabled, but sound effects and background music _Trace_ logs remain disabled.

Additionally one can adjust the macros to automatically add a topic specific tag to the emitted log message.

# Assertions

Assertions are runtime checks placed in the code by programmers, for programmers.
They are commonly used to check pre- and post-conditions, like ensuring that a function is not called with a `nullptr` where it is not supported.

While adding assertions is considered good practice, I don't like the conventional implementation (`assert` found in `<assert.h>`) for the following reasons:

- `assert` can be disabled by defining `NDEBUG`, which is commonly done in release builds.
  This is likely a performance consideration, but most assertions don't have a significant performance impact such that I want to outright disable all of them.

- Triggering an assertion will immediately terminate the application.
  For most applications, I prefer to handling assertion failure a lot more gracefully.

Generally, I only want my application to terminate during initialization or when an error is encountered that is practically impossible to recover from.

My alternative to assertion is called _check_ and has the following properties:

- Checks are not disabled in release build.
  There may be certain specific checks (similar to topical tracing explained earlier) which can be disabled if they are too costly.
  However, you can rely on the `CHECK` macro being always enabled.

- When a check triggers:
    - it will emit an error log message;
    - execute a runtime configurable handler; and
    - return with a given value.

The handler can be set to trigger a debugger breakpoint or terminate the application if that is desired.
It can also not do anything.
One could even swap out the handler after initialization.

Returning a value here facilitates the bail-on-failure paradigm.
Usually some `StatusCode` is returned, but it could also be a sensible default value.

Additionally, the `[[unlikely]]` attribute is used to guide compiler optimizations.

Here's an implementation for an overloaded `CHECK` macro.
The second parameter is the return value, which is omitted if the return-type is `void`.

```c++
#define EXPAND(x) x

#define CHECK(...) EXPAND(CHECK_OVERLOAD(__VA_ARGS__, CHECK2, CHECK1)(__VA_ARGS__))
#define CHECK_OVERLOAD(_1, _2, NAME, ...) NAME

#define CHECK1(condition) \
    do { \
        if (!(condition)) [[unlikely]] { \
            ERROR("%s", "Check failed: " #condition); \
            if (auto* handler = ::g_checkHandler.load()) { \
                handler(); \
            } \
            return; \
        } \
    } while (0)

#define CHECK2(condition, ret) \
    do { \
        if (!(condition)) [[unlikely]] { \
            ERROR("%s", "Check failed: " #condition); \
            if (auto* handler = ::g_checkHandler.load()) { \
                handler(); \
            } \
            return ret; \
        } \
    } while (0)

std::atomic<void(*)()> g_checkHandler;
```

Example usage:

```c++
void AudioSystem::playEffect(AssetPtr<AudioTrack> effect, float volume)
{
    CHECK(effect); // ⟵ bail, skipping any operation
    //…
}

Status RenderDevice::createTexture(Texture& texture, std::span<const TextureInit> inits)
{
    CHECK(texture.info.size != Vec2u(0), InvalidArgumentError); // ⟵ bail w/ error
    // …
}

std::string toIdentifier(fs::path filepath)
{
    CHECK(filepath.is_relative(), ""); // ⟵ bail w/ default value
    // …
}
```

# Defer

The defer utility (sometimes referred to as scope-exit) comes in handy when following the bail-on-failure paradigm.
While many c++ objects follow the RAII idiom, we sometimes need to do things before bailing, which are not covered by any of the C++ object's destructor.

In such a case, the defer utility will execute code at the end of its scope:

```c++
#define CONCAT(x, y) x##y
#define XCONCAT(x, y) CONCAT(x, y)
#define TEMPORARY(prefix) XCONCAT(prefix, __COUNTER__)

#define DEFER(code) const auto TEMPORARY(deferer) = ::Deferer([&]() { code; });

template <typename T>
class Deferer {
  public:
    explicit Deferer(T&& function) : m_function(std::move(function)) {}
    ~Deferer() noexcept { m_function(); }

    Deferer(const Deferer&) = delete;
    Deferer& operator=(const Deferer&) = delete;
    Deferer(const Deferer&&) noexcept = delete;
    Deferer& operator=(const Deferer&&) noexcept = delete;

  private:
    T m_function;
};
```

Example usage:

```c++
u8* data = texture->map();
DEFER(texture->unmap());

// …

CHECK(success); // ⟵ even if this bails, we still unmap the texture!
```
