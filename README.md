# Dynswap - Dynamic Library Swapping Module

`Dynswap` is a module for performing dynamic hot-swapping of shared libraries on Android, designed for hooking and instrumentation projects.

## Behavior

- Uses only the configured swap path (`dynswap::config::swap_lib_path()`).
- If the swap file does not exist, it skips swap and continues normal flow.
- Prevents recursive reload loops with `DSWAP_ACTIVE=1`.
- Does **not** delete the swap library file after loading.
- Stores original `libil2cpp.so` path in `LIB_ORIG_PATH`.

## Default config

- Swap path: `/data/local/tmp/libnativebridge.so`
- Original-path env: `LIB_ORIG_PATH`
- Active-guard env: `DSWAP_ACTIVE`

Configured in `src/lib/dynswap/src/config.cpp`.

## Install with FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  dynswap
  GIT_REPOSITORY <your-repo-url>
  GIT_TAG <tag-or-commit>
)

FetchContent_MakeAvailable(dynswap)

target_sources(YourTarget PRIVATE
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/dynswap.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/loader.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/config.cpp
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/src/env.cpp
)

target_include_directories(YourTarget PRIVATE
  ${dynswap_SOURCE_DIR}/src/lib/dynswap/include
)

target_link_libraries(YourTarget
  log
  android
)
```

## Minimal usage

```cpp
#include <dynswap.h>

#include <dlfcn.h>
#include <libgen.h>
#include <string>

extern "C" jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    Dl_info dlInfo{};
    if (!dladdr(reinterpret_cast<void *>(JNI_OnLoad), &dlInfo))
        return JNI_VERSION_1_6;

    // 1) If we are inside swap, dynswap already stored original lib path in env.
    // 2) If not, fallback to current-dir/libil2cpp.so.
    std::string libFullPath = dynswap::get_library_path();
    if (libFullPath.empty())
    {
        std::string currentDir = dirname(dlInfo.dli_fname);
        libFullPath = currentDir + "/libil2cpp.so";
    }

    if (dynswap::try_load(vm, libFullPath.c_str()))
        return JNI_VERSION_1_6;

    // continue normal init using libFullPath
    return JNI_VERSION_1_6;
}
```

## API

- `bool dynswap::try_load(JavaVM *vm, const char *original_lib_path);`
- `bool dynswap::load_original_jni_onload(JavaVM *vm);`
- `std::string dynswap::get_library_path();`

Config namespace:

- `const char *dynswap::config::swap_lib_path();`
- `const char *dynswap::config::env_var_name();`
- `const char *dynswap::config::swap_active_env_var_name();`


## Troubleshooting

- Verify swap file exists at configured path.
- Verify ABI matches target process.
- Check logs for `DSWAP_ACTIVE` guard behavior.
- If you see `/data/local/tmp/libil2cpp.so` in logs, your caller is not using `dynswap::get_library_path()` first.
