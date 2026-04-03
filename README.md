# dynswap - Dynamic Library Swapping Module

`dynswap` is a module for performing dynamic hot-swapping of shared libraries on Android, designed for hooking and instrumentation projects.

## What does it do?

- Dynamically loads a shared library (e.g., `libil2cpp.so`) from a temporary or custom location
- Automatically calls `JNI_OnLoad` from the loaded library
- Prevents recursive swap loops with a process-level guard flag (`DSWAP_ACTIVE`)
- Manages paths and environment variables to facilitate library replacement

## Architecture

- `include/dynswap.h`: Main API header
- `src/config.cpp`: Path configuration and utilities
- `src/dynswap.cpp`: Core swap logic and helpers
- `src/env.cpp`: Environment variable management
- `src/loader.cpp`: Dynamic loading and `JNI_OnLoad` invocation

## Basic Usage

1. Include `dynswap.h` in your project
2. Call `dynswap::try_load(vm, path)` from your `JNI_OnLoad` to attempt the swap
3. Use configuration utilities to define custom paths if needed

## Install with FetchContent

You can vendor `dynswap` directly from CMake using `FetchContent`.

### 1) Declare and fetch

```cmake
include(FetchContent)

FetchContent_Declare(
  dynswap
  GIT_REPOSITORY <your-repo-url>
  GIT_TAG <tag-or-commit>
)

FetchContent_MakeAvailable(dynswap)
```

### 2) Add dynswap sources to your target

`dynswap` is source-based in this repo layout, so include its files explicitly:

```cmake
target_sources(YourTarget PRIVATE
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/dynswap.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/loader.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/config.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/env.cpp
)

target_include_directories(YourTarget PRIVATE
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/include
)
```

### 3) Ensure Android/system libs are linked

```cmake
target_link_libraries(YourTarget
  log
  android
)
```

### 4) Call from `JNI_OnLoad`

```cpp
extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    dynswap::try_load(vm, "/data/local/tmp/libnativebridge.so");
    return JNI_VERSION_1_6;
}
```

Notes:
- Replace `<your-repo-url>` and `<tag-or-commit>` with your own values.
- If you keep a custom swap filename/path, update `dynswap::config::swap_lib_path()` accordingly.

## Example

```cpp
#include <dynswap.h>

extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    std::string libPath = dynswap::get_temp_library_path();
    if (!libPath.empty() && libPath != dynswap::config::swap_lib_path()) {
        dynswap::try_load(vm, libPath.c_str());
    }
    // ... rest of your initialization ...
    return JNI_VERSION_1_6;
}
```

## Key Features

- **Path Management**: Handles temporary and permanent library paths
- **JNI Integration**: Properly calls JNI_OnLoad with correct JavaVM reference
- **Loop Prevention**: Detects and prevents recursive library loading
- **Environment Variables**: Uses LD_LIBRARY_PATH and custom environment variables for path configuration
- **Error Handling**: Graceful failures with detailed logging

## Notes

- Ensure the swap path (`swap_lib_path`) points to the correct temporary library location
- This module is designed specifically for Android (uses `dlopen`, `JNI_OnLoad`, etc.)
- When modifying swap logic, carefully test to prevent infinite loading loops
- The original library path should be saved before swap for reference

## API Reference

### Core Functions

```cpp
// Attempt to dynamically load and swap a library
void dynswap::try_load(JavaVM *vm, const char *path);

// Get the temporary library path
std::string dynswap::get_temp_library_path();

// Get the original library path
std::string dynswap::get_original_lib_path();

// Check if swap is already in progress
bool dynswap::is_swapping();
```

### Configuration

```cpp
// Get/Set the swap library path
const char* dynswap::config::swap_lib_path();
void dynswap::config::set_swap_lib_path(const char *path);

// Get/Set the original library path
const char* dynswap::config::original_lib_path();
void dynswap::config::set_original_lib_path(const char *path);
```

### Environment Variables

```cpp
// Set environment variable
void dynswap::env::set_env(const char *key, const char *value);

// Get environment variable
std::string dynswap::env::get_env(const char *key);
```

## Integration Example

In a host project, dynswap can be used in the main JNI_OnLoad:

```cpp
extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    // Use dynswap to replace libil2cpp.so with a swapped library
    dynswap::try_load(vm, "/data/local/tmp/libnativebridge.so");
    
    // Continue with normal initialization
    // ...
}
```

This allows transparent library replacement without modifying the APK.

## Troubleshooting

### Library not loading
- Verify the library exists at the specified path
- Check file permissions: `adb shell ls -la /data/local/tmp/libnativebridge.so`
- Ensure the library has correct ARM64 architecture

### JNI_OnLoad not called
- Verify JavaVM is correctly passed to dynswap
- Check if the library export functions is defined correctly

### Recursive swap / loop behavior
- The module uses `DSWAP_ACTIVE=1` as a process guard to avoid recursive reload attempts.
- If swap loading fails, the guard is cleared so a later retry is still possible.

---

**Author:** Isaac
