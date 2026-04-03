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
        return "IL2CPP_ORIG_PATH";
    }
} // namespace dynswap::config
