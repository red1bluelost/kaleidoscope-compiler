#ifndef KALEIDOSCOPE_LOG_H
#define KALEIDOSCOPE_LOG_H

#include <fmt/core.h>

#include <memory>
#include <string_view>

namespace kaleidoscope {

/// logError<T> - These are little helper functions for error handling.
template <typename T>
inline static std::unique_ptr<T> logError(std::string_view Str) {
  fmt::print(stderr, "Error: {}\n", Str);
  return nullptr;
}

/// logErrorR<T> - These are little helper functions for error handling.
template <typename T> inline static T *logErrorR(std::string_view Str) {
  fmt::print(stderr, "Error: {}\n", Str);
  return nullptr;
}

} // namespace kaleidoscope

#endif // KALEIDOSCOPE_LOG_H
