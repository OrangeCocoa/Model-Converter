#pragma once

constexpr uint64_t MMD_SUCCESS = static_cast<uint64_t>(true);
constexpr uint64_t MMD_FAIL = static_cast<uint64_t>(false);

inline constexpr bool succeeded(uint64_t hr) { return static_cast<int>(hr) > 0; }
inline constexpr bool failed(uint64_t hr) { return static_cast<int>(hr) <= 0; }