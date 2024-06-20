#pragma once

#include <cstdio>

inline constexpr size_t CHUNK_SIZE = BUFSIZ;
inline constexpr size_t MAX_EVENTS = 1;
inline constexpr size_t WORKER_BUF_SIZE = 2 * CHUNK_SIZE;