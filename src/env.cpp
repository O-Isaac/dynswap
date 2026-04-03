#include <cstdlib>
#include <string>
#include <logger.h>

namespace dynswap::env
{
    // Saves the original library path to the specified environment variable.
    void save_original_path(const char *env_var_name, const char *original_lib_path)
    {
        if (!getenv(env_var_name))
        {
            setenv(env_var_name, original_lib_path, 1);
            LOGI("Original library path saved to env: %s", original_lib_path);
        }
    }

    // Loads the original library path from the specified environment variable.
    std::string load_original_path(const char *env_var_name)
    {
        const char *value = getenv(env_var_name);

        if (value && value[0] != '\0')
        {
            LOGI("Original library path retrieved from env: %s", value);
            return std::string(value);
        }

        return std::string();
    }

} // namespace dynswap::env