#include <string>

namespace dynswap::config
{
    const char *swap_lib_path()
    {
        // Primary path where the temporary swap library is stored
        return "/data/local/tmp/libnativebridge.so";
    }

    const char *env_var_name()
    {
        // Environment variable name to store the original library path
        return "LIB_ORIG_PATH";
    }

    const char *swap_active_env_var_name()
    {
        // Process-wide guard flag to prevent recursive swap attempts
        return "DSWAP_ACTIVE";
    }
} // namespace dynswap::config
