# Error handling in `sn`

## API

Every format has exactly two calls per direction. For `std::string`:

```cpp
bool                      sn::to_string(const T &src, std::string *dst, sn::error *err, Tags... tags);
sn::expected<std::string> sn::to_string(const T &src, Tags... tags);

bool                      sn::from_string(std::string_view src, T *dst, sn::error *err, Tags... tags);
sn::expected<T>           sn::from_string<T>(std::string_view src, Tags... tags);
```

The first form returns whether the conversion succeeded:
- Pass `nullptr` as `err` if you don't need to know why it failed, e.g. for speculative parsing. Nothing is allocated
  on this path.
- Otherwise `*err` is written **only on failure**, like `errno`, so a single `sn::error` can be reused across calls
  without being cleared.
- `*dst` is unspecified on failure.

The second form returns `sn::expected`, which is what you want for `co_await`-style error propagation or when you just
need the value:

```cpp
int x = sn::from_string<int>(s).or_throw(); // Throws sn::bad_expected_access with the error message on failure.
std::string name = sn::to_string(MonsterType::Goblin).or_throw();

if (sn::expected<int> x = sn::from_string<int>(s)) {
    use(*x);
} else {
    log(x.error().what()); // E.g. "'zz' is not a number".
}
```

`sn::error` holds a message and, for errors in nested values, a path to the value that failed. `what()` returns both,
e.g. `points[2].y: 'zz' is not a number`. Messages are formatted when the error happens, which allocates. If you don't
need the message, pass `nullptr` as the `sn::error *` argument, then nothing is allocated.

There are no `try_` functions and no throwing out-parameter forms.


## `sn::expected`

`sn::expected<T>` derives from `std::expected<T, sn::error>`. The differences are:
- `or_throw()` is the same as `value()`, but reads better at call sites. Prefer it.
- `or_throw()` and `value()` throw `sn::bad_expected_access`, which derives from `std::bad_expected_access<sn::error>`,
  but its `what()` returns the error message. `std::bad_expected_access::what()` returns a generic string, e.g. on
  libc++ and libstdc++.
- Monadic operations return `sn::expected`, and `and_then` / `or_else` accept functions that return either
  `sn::expected` or `std::expected` with `sn::error` as the error type.

Going through a `std::expected` reference, or copying into a `std::expected`, brings back the standard `value()` and
its generic message. Use `auto` or `sn::expected` for the results of `sn` calls.

Code that integrates with `std::expected<T, E>` generically, e.g. a coroutine promise's `await_transform`, accepts
`sn::expected` as is.


## Writing extension points

Extension points have the same signature as the `bool` form above:

```cpp
SN_DECLARE_STRING_FUNCTIONS(point) // Declares the two functions below.

bool to_string(const point &src, std::string *dst, sn::error *err);
bool from_string(std::string_view src, point *dst, sn::error *err);
```

Rules:
- Return `false` on failure. If `err` is not null, write a description of the problem into `*err`.
- Don't touch `*err` on success.
- Pass `err` through when (de)serializing nested values. If a nested value fails, add your part of the path with
  `err->prepend_path_key(...)` or `err->prepend_path_index(...)`.
- For speculative attempts that you recover from, e.g. trying one format and then another, pass `nullptr`.

```cpp
bool from_string(std::string_view src, point *dst, sn::error *err) {
    std::size_t pos = src.find(',');
    if (pos == std::string_view::npos) {
        if (err)
            *err = sn::error("Expected 'x,y'");
        return false;
    }

    if (!sn::from_string(src.substr(0, pos), &dst->x, err)) {
        if (err)
            err->prepend_path_key("x");
        return false;
    }

    if (!sn::from_string(src.substr(pos + 1), &dst->y, err)) {
        if (err)
            err->prepend_path_key("y");
        return false;
    }

    return true;
}
```


## Strictness

Conversions are strict in both directions. For example, serializing an enum value that's not in the enum's table fails,
and so does deserializing a number into an enum. The value is never silently replaced with something that won't read
back.


## Why `sn::error` is declared in `sn::errors`

`sn::error` is declared in `sn::errors` and is brought into `sn` with a using-declaration. This is not cosmetic.

Extension points take `sn::error *`, so the namespace where `sn::error` is declared becomes an associated namespace for
argument-dependent lookup at every extension point call, including the ones `sn` makes internally. If that namespace
were `sn`, ADL would find the user-facing `sn::from_string(std::string_view, T *, sn::error *, Tags...)`, which has
exactly the signature of an extension point. For types without an extension point, `sn` would then call itself
recursively at runtime, and the compile-time checks that report unsupported types would pass for every type.

So no type that appears in extension point signatures may be declared directly in `sn`. The `check_unsupported` tests
in `sn/string/test/string_ut.cpp` stop compiling if this ever happens.

See also [extension_points.md](extension_points.md).
