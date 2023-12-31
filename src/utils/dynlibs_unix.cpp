// Copyright (C) 2020 Samsung Electronics Co., Ltd.
// See the LICENSE file in the project root for more information.

/// \file dynlibsi_unix.h  This file contains unix-specific function definitions
/// required to work with dynamically loading libraries.

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#include <crt_externs.h>
#endif

#include <dlfcn.h>
#include "utils/limits.h"
#include "utils/dynlibs.h"
#include "utils/logger.h"

namespace netcoredbg
{

// This functon load specified library and returns handle (which then
// can be passed to DLSym and DLCLose functions).
// In case of error function returns NULL.
DLHandle DLOpen(const std::string &path)
{
    void *tmpPointer = ::dlopen(path.c_str(), RTLD_GLOBAL | RTLD_NOW);
    if (tmpPointer == NULL)
    {
        char *err = ::dlerror();
        fprintf(stderr, "dlopen() error: %s\n", err);
        LOGE("dlopen() error: %s", err);
    }
    return reinterpret_cast<DLHandle>(tmpPointer);
}

// This function resolves symbol address within library specified by handle,
// and returns it's address, in case of error function returns NULL.
void* DLSym(DLHandle handle, string_view name)
{
    char str[LINE_MAX];
    if (name.size() >= sizeof(str))
        return {};

    name.copy(str, name.size());
    str[name.size()] = 0;

    ::dlerror(); // Clear any existing error

    void *tmpPointer = ::dlsym(handle, str);

    char *err = ::dlerror();
    if (err != NULL)
    {
        fprintf(stderr, "dlsym() error: %s\n", err);
        LOGE("dlsym() error: %s", err);
    }

    return tmpPointer;
}

/// This function unloads previously loadded library, specified by handle.
/// In case of error this function returns `false'.
bool DLClose(DLHandle handle)
{
    int ret = ::dlclose(handle);
    if (ret != 0)
    {
        char *err = ::dlerror();
        fprintf(stderr, "dlclose() error: %s\n", err);
        LOGE("dlclose() error: %s", err);
    }

    return ret;
}

}  // ::netcoredbg
#endif // __unix__
