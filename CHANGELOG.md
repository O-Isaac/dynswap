# Changelog

All notable changes to `dynswap` are documented in this file.

## 2026-04-03

### Changed
- Replaced swap-loop prevention based on file deletion with a process guard flag: `DSWAP_ACTIVE`.
- Updated swap loading flow to skip recursive `try_load` calls when the guard is active.
- Updated default swap filename and env key names to remove project-identifying strings.

### Fixed
- Removed automatic `unlink` of the swap library after `dlopen`.
- Removed `atexit` cleanup that deleted the swap library file.

### Why
- Avoid recursive loading loops without deleting `/data/local/tmp/hook.so`.
- Preserve on-disk library path for cleaner crash diagnostics and easier symbol/backtrace analysis.
