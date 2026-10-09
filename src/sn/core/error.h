#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility> // For std::move.
#include <variant>
#include <vector>

#include "error_fwd.h"

namespace sn::errors {

/**
 * Error reported by `sn` conversion functions.
 *
 * Holds a human-readable message describing what went wrong, and for errors in nested values, the path to the value
 * that failed, e.g. `points[2].y`. Use `what()` to get both as a single string, e.g.
 * `points[2].y: 'zz' is not a number`.
 *
 * Note that this class is declared in `sn::errors` and is brought into `sn` with a using-declaration. This is
 * intentional. Extension points take an `sn::error *` argument, which makes the namespace where `sn::error` is declared
 * an associated namespace for argument-dependent lookup at every extension point call. If that namespace were `sn`, ADL
 * would find the user-facing `sn` functions, which have the same signatures as extension points. See
 * `docs/error_handling.md` for details.
 */
class error {
public:
    error() = default;

    explicit error(std::string message) : _message(std::move(message)) {}

    /**
     * @return                          Message describing what went wrong, without the path.
     */
    [[nodiscard]] const std::string &message() const {
        return _message;
    }

    /**
     * @return                          Path to the value that failed, e.g. `points[2].y`. Empty string if the error
     *                                  happened at the top level.
     */
    [[nodiscard]] std::string path() const {
        std::string result;
        for (auto pos = _reversed_path.rbegin(); pos != _reversed_path.rend(); ++pos) {
            if (const std::size_t *index = std::get_if<std::size_t>(&*pos)) {
                result += '[';
                result += std::to_string(*index);
                result += ']';
            } else {
                if (!result.empty())
                    result += '.';
                result += std::get<std::string>(*pos);
            }
        }
        return result;
    }

    /**
     * @return                          Path and message as a single string, e.g. `points[2].y: 'zz' is not a number`,
     *                                  or just the message if the path is empty.
     */
    [[nodiscard]] std::string what() const {
        std::string result = path();
        if (!result.empty())
            result += ": ";
        result += _message;
        return result;
    }

    /**
     * Adds a key segment, e.g. a struct field name or a map key, to the front of the path.
     *
     * In extension points, use `sn::prepend_error_path` instead, it handles `nullptr`.
     *
     * @param key                       Key to add.
     */
    void prepend_path(std::string_view key) {
        _reversed_path.emplace_back(std::in_place_type<std::string>, key);
    }

    /**
     * Same as above, but adds an index segment, e.g. an array element index.
     *
     * @param index                     Index to add.
     */
    void prepend_path(std::size_t index) {
        _reversed_path.emplace_back(std::in_place_type<std::size_t>, index);
    }

    friend bool operator==(const error &l, const error &r) = default;

private:
    std::string _message;
    std::vector<std::variant<std::string, std::size_t>> _reversed_path; // Innermost segment first.
};

} // namespace sn::errors

namespace sn {

/**
 * Adds a key segment, e.g. a struct field name or a map key, to the front of the path of `*err`. To be used in
 * extension points for composite types when a nested value fails.
 *
 * Does nothing if `err` is `nullptr`.
 *
 * @param err                           Error output, can be `nullptr`.
 * @param key                           Key to add.
 */
inline void prepend_error_path(sn::error *err, std::string_view key) {
    if (err)
        err->prepend_path(key);
}

/**
 * Same as above, but adds an index segment, e.g. an array element index.
 *
 * @param err                           Error output, can be `nullptr`.
 * @param index                         Index to add.
 */
inline void prepend_error_path(sn::error *err, std::size_t index) {
    if (err)
        err->prepend_path(index);
}

} // namespace sn
