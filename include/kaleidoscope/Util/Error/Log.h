#ifndef KALEIDOSCOPE_UTIL_ERROR_LOG_H
#define KALEIDOSCOPE_UTIL_ERROR_LOG_H

#include <fmt/core.h>

#include <string_view>

namespace kaleidoscope {

/// logError - These are little helper functions for error handling.
inline static auto logError(std::string_view Str) -> std::nullptr_t {
  fmt::print(stderr, "Error: {}\n", Str);
  return nullptr;
}

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_UTIL_ERROR_LOG_H
