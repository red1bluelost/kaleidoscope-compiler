#ifndef KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
#define KALEIDOSCOPE_UNITTESTS_TESTUTIL_H

#include <functional>
#include <string>

inline auto makeGetCharWithString(std::string S) -> std::function<int()> {
  return [S, I = 0]() mutable -> int { return static_cast<int>(S[I++]); };
}

#endif // KALEIDOSCOPE_UNITTESTS_TESTUTIL_H
