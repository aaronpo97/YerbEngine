#pragma once

#include <filesystem>

namespace YerbEngine {

typedef std::filesystem::path Path;

namespace EnginePaths {

/**
 * Picks the directory a game should read from.
 *
 * @param preferred The directory the game asked for.
 * @param fallback Where to look when the preferred one is absent.
 * @param label How to name this directory in messages (e.g. "Assets").
 * @returns preferred when it exists, otherwise fallback.
 * @throws std::runtime_error when neither directory exists.
 */
Path resolveDirectory(Path const& preferred, Path const& fallback,
                      char const* label);

}  // namespace EnginePaths

}  // namespace YerbEngine
