# Compilation

Compiling C/C++ is a multi-stage process involving multiple different tools.
Ignoring the build system, these are the main stages when invoking the compiler (e.g. GCC, Clang, MSVC).

![Toolchain](images/toolchain.png)

- The **preprocessor** takes in the source file and resolves preprocessor directives (`#if`, `#include`, `#define`, …).

- The **compiler** receives the output of the preprocessor and translates the C/C++ code to _object code_ (binary).
  This object code cannot be executed directly as it's missing information needed by the operating system to create a process.

- The **linker** combines multiple object files with existing libraries into an executable (or library).
  Executables can be run directly; libraries are consumed by other C/C++ projects.

The linker is concerned with **symbols**, which are identifiers for functions, variables, etc.
Symbols can have different visibility, which is also referred to as **linkage**.

## Static vs. Dynamic Library

Libraries commonly come in two different types.

- **Static** libraries are just packed object files.
  Like any object file, the linker combines them into the target executable.
  Since all code of static libraries is located inside the executable, the libraries themselves are not needed for execution.

- **Dynamic** libraries are a bit more complex.
  Contrary to static libraries, code remains inside the library, hence the library must be present during execution.
  However, this mechanism allows the library to be replaced without having to re-link the executable.
  The **loader** is in charge of finding the dynamic libraries needed by an executable.

### Linux

In Linux-like operating systems static libraries typically use `.a` (archive) as file extension (e.g. `libcrypt.a`); dynamic libraries typically use `.so` (shared object).

The `ar` utility can be used to create, inspect, and modify static libraries.

```
$ ar t /usr/lib/x86_64-linux-gnu/libcrypt.a
libcrypt_la-alg-des-tables.o
libcrypt_la-alg-des.o
libcrypt_la-alg-gost3411-2012-core.o
libcrypt_la-alg-gost3411-2012-hmac.o
libcrypt_la-alg-hmac-sha1.o
libcrypt_la-alg-md4.o
libcrypt_la-alg-md5.o
…
```

The `nm` utility can be used to inspect which _symbols_ are provided by a library
This works for static and dynamic libraries.

```
$ nm /usr/lib/x86_64-linux-gnu/libcrypt.a

libcrypt_la-alg-des-tables.o:
0000000000002000 R _crypt_comp_maskl
0000000000001000 R _crypt_comp_maskr
0000000000007000 R _crypt_fp_maskl
0000000000005000 R _crypt_fp_maskr
000000000000b000 R _crypt_ip_maskl
…

$ nm --dynamic  lib/x86_64-linux-gnu/libcrypt.so
                 U arc4random_buf@GLIBC_2.36
                 U __assert_fail@GLIBC_2.2.5
0000000000015da0 T crypt@GLIBC_2.2.5
0000000000015da0 T crypt@@XCRYPT_2.0
0000000000016c70 T crypt_checksalt@@XCRYPT_4.3
0000000000013460 T crypt_gensalt@@XCRYPT_2.0
0000000000016ac0 T crypt_gensalt_r@XCRYPT_2.0
…
```

The `ldd` utility can be used to list the dynamic libraries required by an executable.
It also tries to resolve these requirements, stating where the dynamic library has been found.

```
$ ldd /usr/bin/ssh
linux-vdso.so.1 (0x00007ffda6906000)
libselinux.so.1 => /lib/x86_64-linux-gnu/libselinux.so.1 (0x00007f7d6d5d0000)
libgssapi_krb5.so.2 => /lib/x86_64-linux-gnu/libgssapi_krb5.so.2 (0x00007f7d6d57e000)
libcrypto.so.3 => /lib/x86_64-linux-gnu/libcrypto.so.3 (0x00007f7d6d0fc000)
libz.so.1 => /lib/x86_64-linux-gnu/libz.so.1 (0x00007f7d6d0dd000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7d6cefc000)
libpcre2-8.so.0 => /lib/x86_64-linux-gnu/libpcre2-8.so.0 (0x00007f7d6ce60000)
…
```

The loader searches in directories stated in the `LD_LIBRARY_PATH` environment variable in addition to it's system-wide configuration (typically `/etc/ld.so.conf`).
Additionally, an executable can provide an direct path to a dynamic library; this mechanism is referred to as [rpath](https://en.wikipedia.org/wiki/Rpath).

### Windows

In Windows, static libraries use `.lib` as file extension, where dynamic libraries use `.dll`.
However, there's a slight twist.
In Linux, you directly tell the linker which dynamic libraries to link the executable against.
However, in Windows you give the linker a _DLL import library_ which is used during compile-time to prepare the executable for linking with corresponding `.dll` at runtime.
This DLL import library usually uses `.lib` or `.dll.a` as file extension.

For listing symbols of a shared library, there's the `dumpbin` utility.

```
$ dumpbin /SYMBOLS /EXPORTS C:\Windows\System32\ntprint.dll
…
122    1 0000E980 PSetupAssociateICMProfiles
123    2 00023AA0 PSetupBuildDriverList
124    3 00023680 PSetupBuildDriversFromPath
108    4 00017CD0 PSetupCheckForDriversInDriverStore
125    5 00030540 PSetupCopyDriverPackageFiles
…
```

For inspecting an executables dynamic library dependencies there's [Dependency Walker](https://www.dependencywalker.com/).

Important differences to Linux:
- The directory containing the executable file is part of the loader's DLL search path.
- There's no rpath equivalent.

Because of this, DLLs are typically placed right next to the executable.
During development this is commonly automated using a _post build action_.

### Name Mangling (C++ only)

For C code, symbol names will be identical to function and variable names.
In C++, symbol names are _mangled_ to support features like namespaces, templates, and function overloading.

A function signature like this:

    Map<StringName, Ref<GDScript>, Comparator<StringName>, DefaultAllocator>::has(StringName const&) const

gets turned into:

    _ZNK3MapI10StringName3RefI8GDScriptE10ComparatorIS0_E16DefaultAllocatorE3hasERKS0_

You can use the `c++filt` (Linux) / `undname` (Windows) tools to translate a mangled name back to its signature.
Note that name mangling is **not** standardized.

`extern "C"` can be used in C++ to disable name mangling for specific symbols.
