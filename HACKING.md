# Code Style
## Naming
* Use `stl_snake_case` for everything, even for Qt-related functions.
* Use `_field` for private fields. That's right, with the leading underscore.
* For implementation macros, use `_MACRO_I`, `_MACRO_II`, etc. `_UPPERCASE` is reserved in C++, but we don't care. Besides, this is obviously better than boost's way of naming stuff where your autocompletion list is full of implementation macros that you're not even supposed to know about.

## Using C++ features
* We're targeting C++20 right now, but will switch to C++23 once compilers are mature enough. Thus, the target standard for sn 1.0 is C++23.
* Don't include the heavy stuff, especially from the headers. So, no `<ranges>`.


