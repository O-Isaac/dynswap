#include <dlfcn.h>
#include <unistd.h>
#include <logger.h>
#include <jni.h>
#include <stdlib.h>
#include "utils.h"

namespace dynswap::loader
{
    bool load_and_call_jni_onload(JavaVM *vm, const char *path)
    {
        void *handle = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
        if (!handle)
        {
            LOGE("Failed to load library: %s", dlerror());
            return false;
        }

        using JNI_OnLoad_t = jint (*)(JavaVM *, void *);
        auto jni_onload = reinterpret_cast<JNI_OnLoad_t>(dlsym(handle, "JNI_OnLoad"));

        if (jni_onload)
        {
            LOGI("Calling JNI_OnLoad from %s", path);
            jni_onload(vm, nullptr);
        }
        else
        {
            LOGI("No JNI_OnLoad found in %s", path);
            return false;
        }

        return true;
    }

} // namespace dynswap::loader
