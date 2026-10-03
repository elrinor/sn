# Code Style
## Naming
* Use `stl_snake_case` for everything, even for Qt-related functions.
* Use `_field` for private fields. That's right, with the leading underscore.
* Use `SNAKE_CASE_ALL_CAPS` for enums.
* Use `SNAKE_CASE_ALL_CAPS` for macros and for macro parameters.
* For implementation macros, use `_MACRO_I`, `_MACRO_II`, etc. `_UPPERCASE` is reserved in C++, but we don't care. Besides, this is obviously better than boost's way of naming stuff where your autocompletion list is full of implementation macros that you're not even supposed to know about.

## Using C++ features
* Use `enum class`es for enumerations followed by `using enum`. Don't use plain `enum`s.
* Don't include the heavy stuff, especially from the headers. So, no `<ranges>`.
* We're targeting C++23 right now, but will likely switch to C++26 once compilers catch up. This is not 100% decided yet, but it looks like we'll need `pack...[indexing]` for tag sorting. Thus, the current target standard for sn 1.0 is C++26.

## Code organization
* One of the goals of `sn` is to be an IDE-friendly library. This means, for example, that when the user types `sn::`, he should see the actual user-facing `sn` functions and nothing else. When he types `tn::`, he should see user-facing tags and nothing else. If we're offering some facility, then there should be little need to consult documentation on how to use it.
* Yes, I can hear you macro haters. There is very little that we can do right now to make our macros better, but that's an area of active research.
