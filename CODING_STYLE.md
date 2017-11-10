First of all, use common scene.

Code has to be written for humans to read, do not overenginner nor overcomplicate.

## Exceptions
All rules are soft enough to be lifted upon violation of common sense, but before breaking any rule, please discuss the case, since the rule itself might have an error on its own. Explicitly mark the exception with "CODING STYLE EXCEPTION" in comments.

Following does not apply to the code borrowed from other projects, like the dependencies in third-party.
That is required to simplify merging when bumping version of dependency and\or updating the snippet.

## Formatting
Use clang-format with enclosed clang-format.txt style on modified files before pushing them upstream.
**Notice:** Sometime clang-format (as of version 3.8) produces very ugly results. In this case please don't just blindly commit your changes, but revert pieces that are utterly ugly. Notable examples are C++11 lambdas and array initializers.

## Global
C++11, not C-with-classes.

Why C++11 but not 14, 17? This project aim is to be core of mobile app, and some target toolchains\SDKs do not support C++14 and above (in fact, even C++11 is not fully supported).

That means you are encouraged to use:

- STL
- smart-pointers
- exceptions
- auto (with moderation)
- GLSL (TODO)

## Interfaces
We use C++11, however, due to ABI compatibility reasons and to simplify integration with other languages\platforms, core library has a C-like interface.
That means:

- API functions should be ```extern "C"```
- API functions take only primitive types and structs as argument, no C++ classes.
- API functions throw no exceptions
- Public headers do no include C++ headers (even standard ones).

General rules of thumb: 

- API functions should have an explanation comment, stating function purpose, expectation on arguments values and promises on produced values.
- If function is complicated enough, please put a comment at the very to, briefly describing what and why at the top-level.
- All data\objects allocated within the library should be freed in the library.
	- Preferably a data\objects allocated\created in the translation unit, should be deallocated from that same unit.
- Function arguments

## Naming

- All types, that is classes, structs, enums, typedefs are camel-case and begin with capital letter. 
- All functions use snake_case and NOT capitalized.
	- Function name should begin with verb. 
	- Function that creates new object should have `make_` prefix, like `make_acount()`.
- Variables (function parameters too) are all `snake_case` (all lower case), must be a noun.
- Struct members follow rules for variables, and should have no prefixes.
- Class members follow rules for variables, but also have a `m_` prefix.
- Global constants and enum values, are `SNAKE_CASE`.

```
class Foo
{
};

struct Bar
{
};

```

## Tests
Write code with tests in mind, code which is not tested can't be really used. Please write tests even for trivial functions. However, do not test libc, OS, network and\or other external stuff, unless that is required specifically.
We use gtest framework for unit-tests.
Every library shipped should have a corresponding test library, where all unit-tests reside. That allows to ship  test libraries to the target devices and verify correctness of the library. It is Ok to include in the test library specific tests for the dependencies of the shipped library.

## Dependencies
We would like to minimizer library footprint and hence tend to minimize dependency, please do not bring you super-huge-and-cool-framewok if you need only a string-to-hex convertion function.
Prefer C-libraries as dependencies over C++ counterparts, since those are generally more portable, easier to read and maintain.
Right now we would like to avoid dependency on boost.