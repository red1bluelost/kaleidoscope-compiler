#ifndef KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
#define KALEIDOSCOPE_UNITTESTS_TESTUTIL_H

#include <functional>
#include <string>

inline auto makeGetCharWithString(std::string S) -> std::function<int()> {
  S += static_cast<char>(EOF);
  return [S, I = static_cast<std::size_t>(0)]() mutable -> int {
    return static_cast<int>(S[I++]);
  };
}

#endif // KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
