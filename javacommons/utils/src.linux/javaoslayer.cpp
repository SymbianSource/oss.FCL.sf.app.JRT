/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  JavaOsLayer
*
*/


#include <stdlib.h>
#include <dlfcn.h>

#include "logger.h"
#include "javaoslayer.h"

using namespace java::util;

OS_EXPORT void JavaOsLayer::getOsSpecificLibName(std::string& result,
        const char* libName)
{
    JELOG2(EUtils);
    if (libName)
    {
        result += "lib";
        result += libName;
        result += ".so";
        LOG2(EUtils, EInfo,
             "JavaOsLayer::getOsSpecificLibName(), Name: %s, Result: %s",
             libName, result.c_str());
    }
    else
    {
        ELOG(EUtils, "JavaOsLayer::getOsSpecificLibName() libname was null");
    }
}

OS_EXPORT void JavaOsLayer::getOsSpecificJavaRootPath(std::string& path)
{
    JELOG2(EUtils);
    path = getenv("JAVA_BIN_ROOT");
    LOG1(EUtils, EInfo, "JavaOsLayer::getOsSpecificLibName() Path: %s",
         path.c_str());
}



void appendTrailingSlash(std::string& path)
{
    if (path[path.length()-1] != '/')
    {
        path += '/';
    }
}

OS_EXPORT std::string& JavaOsLayer::getBinRoot(std::string& path, bool /*append*/)
//Argument append is not relevant in Linux environment
{
    JELOG2(EUtils);
    path = getenv("JAVA_BIN_ROOT");
    appendTrailingSlash(path);
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getResRoot(std::string& path, bool /*append*/)
//Argument append is not relevant in Linux environment
{
    JELOG2(EUtils);
    path = getenv("JAVA_BIN_ROOT");
    appendTrailingSlash(path);
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getMidpRoot(std::string& path, bool /*append*/)
//Argument append is not relevant in Linux environment
{
    JELOG2(EUtils);
    path = getenv("JAVA_BIN_ROOT");
    appendTrailingSlash(path);
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getJavaCaptainRoot(std::string& path, bool /*append*/)
//Argument append is not relevant in Linux environment
{
    JELOG2(EUtils);
    path = getenv("JAVA_BIN_ROOT");
    appendTrailingSlash(path);
    return path;
}

OS_EXPORT DriveId JavaOsLayer::getMidpDrive()
{
//    JELOG2(EUtils);
    return DriveId();
}
















OS_EXPORT void* JavaOsLayer::dlopen(const char* libName)
{
    JELOG2(EUtils);
    void* lib = 0;
    if (libName)
    {
        lib = ::dlopen(libName, RTLD_LAZY);
        LOG2(EUtils, EInfo, "JavaOsLayer::dlopen(%s)  Handle: %X", libName,
             lib);
        if (lib == 0)
        {
            char* errstr;
            errstr = dlerror();
            if (errstr != 0)
            {
                /*
                ELOG2(EUtils, "JavaOsLayer::dlopen(%s) failed. Reason: %s",
                      libName, errstr);
                */
            }
        }
    }
    else
    {
        ELOG(EUtils, "JavaOsLayer::dlopen() libname was null");
    }
    return lib;
}

OS_EXPORT void* JavaOsLayer::dlsym(void* handle, const char* name, bool)
{
    JELOG2(EUtils);
    void* func = 0;
    if (handle && name)
    {
        func = ::dlsym(handle, name);
        LOG2(EUtils, EInfo, "JavaOsLayer::dlsym(%s) fPtr: %X", name, func);
        if (func == 0)
        {
            char* errstr;
            errstr = dlerror();
            if (errstr != 0)
            {
                ELOG2(EUtils, "JavaOsLayer::dlsym(%s) failed. Reason: %s",
                      name, errstr);
            }
        }
    }
    else
    {
        ELOG2(EUtils, "JavaOsLayer::dlsym() null argument handle: %X, name %X",
              handle, name);
    }
    return func;
}

OS_EXPORT int JavaOsLayer::dlclose(void* handle)
{
    JELOG2(EUtils);
    int result = -1;
    if (handle)
    {
        result = ::dlclose(handle);
        if (result != 0)
        {
            ELOG1(EUtils, "JavaOsLayer::dlclose() failed. Reason: %d", result);
        }
    }
    else
    {
        ELOG(EUtils, "JavaOsLayer::dlclose() null handle");
    }
    return result;
}

OS_EXPORT void JavaOsLayer::startUpTrace(const std::string& /*header*/,
        int /*freeMem*/, int /*totalMem*/)
{
    //NOP for Linux at the moment.
}
