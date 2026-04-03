#pragma once

#include <jni.h>
#include <string>

namespace dynswap
{

    namespace config
    {
        /**
         * Retrieves the configured path for the swap library.
         *
         * @return A constant character pointer to the swap library path.
         */
        const char *swap_lib_path();

        /**
         * Retrieves the name of the environment variable used to store the original library path.
         *
         * @return A constant character pointer to the environment variable name.
         */
        const char *env_var_name();

        /**
         * Retrieves the name of the environment variable used as swap-active guard.
         *
         * @return A constant character pointer to the guard environment variable name.
         */
        const char *swap_active_env_var_name();
    } // namespace config

    /**
     * Attempts to load and swap a native shared library at runtime.
     *
     * @param vm Pointer to the JavaVM instance.
     * @param original_lib_path The original path of the library to be swapped.
     * @return true if the swap was successful, false otherwise.
     */
    bool try_load(JavaVM *vm, const char *original_lib_path);

    /**
     * Loads and calls the JNI_OnLoad from the original library.
     * Used by the swap to delegate to the original library initialization.
     *
     * @param vm Pointer to the JavaVM instance.
     * @return true if successful, false otherwise.
     */
    bool load_original_jni_onload(JavaVM *vm);

    /**
     * Retrieves the path of the of original library.
     *
     * @return A string containing the original library path, or an empty string if not set.
     */
    std::string get_library_path();
}
