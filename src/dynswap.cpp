#include <cstring>
#include <cstdlib>
#include "utils.h"
#include <dynswap.h>
#include <logger.h>

namespace dynswap
{
    constexpr const char *kSwapActiveEnv = "DSWAP_ACTIVE";

    namespace
    {
    const char *kLegacySwapPath = "/data/local/tmp/libAvalonHook.so";
    }

    // Forward declarations
    namespace config
    {
        const char *swap_lib_path();
        const char *env_var_name();
    }

    namespace env
    {
        void save_original_path(const char *env_var_name, const char *original_lib_path);
        std::string load_original_path(const char *env_var_name);
    }

    namespace loader
    {
        bool load_and_call_jni_onload(JavaVM *vm, const char *path);
    }

    // Core function implementations
    bool try_load(JavaVM *vm, const char *libPath)
    {
        const char *swapActive = std::getenv(kSwapActiveEnv);
        if (swapActive && std::strcmp(swapActive, "1") == 0)
        {
            LOGI("Dynswap already active in this process. Skipping recursive load.");
            return false;
        }

        const char *swapPath = config::swap_lib_path();

        if (!file_exists(swapPath))
        {
            if (file_exists(kLegacySwapPath))
            {
                LOGI("Primary swap path missing, falling back to legacy path: %s", kLegacySwapPath);
                swapPath = kLegacySwapPath;
            }
            else
            {
                LOGI("No swap library found at %s. Skipping load.", swapPath);
                return false;
            }
        }

        LOGI("Swap library found at %s. Preparing to load.", swapPath);

        if (libPath && strncmp(libPath, "/data/app", 9) == 0)
        {
            env::save_original_path(config::env_var_name(), libPath);
            LOGI("Original libil2cpp.so path saved to env: %s", libPath);
        }

        setenv(kSwapActiveEnv, "1", 1);
        const bool loaded = loader::load_and_call_jni_onload(vm, swapPath);
        if (!loaded)
            unsetenv(kSwapActiveEnv);
        return loaded;
    }

    std::string get_library_path()
    {
        return env::load_original_path(config::env_var_name());
    }

    bool load_original_jni_onload(JavaVM *vm)
    {
        std::string origPath = env::load_original_path(config::env_var_name());

        if (origPath.empty())
        {
            LOGE("Original library path not found in environment.");
            return false;
        }

        if (!file_exists(origPath.c_str()))
        {
            LOGE("Original library not found at: %s", origPath.c_str());
            return false;
        }

        LOGI("Loading original library from: %s", origPath.c_str());
        return loader::load_and_call_jni_onload(vm, origPath.c_str());
    }
} // namespace dynswap
