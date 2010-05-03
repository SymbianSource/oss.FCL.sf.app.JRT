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
* Description:  DynamicLibLoader
*
*/


#include <memory>

#include "logger.h"
#include "javaoslayer.h"
#include "libraryloaderexception.h"
#include "dynamiclibloader.h"

using namespace java::util;

OS_EXPORT DynamicLibLoader::DynamicLibLoader(const char* libName) :
        mHandle(0), mCloseOnDestroy(true)
{
    JELOG2(EUtils);
    JavaOsLayer::getOsSpecificLibName(mLibName, libName);
}

OS_EXPORT DynamicLibLoader::DynamicLibLoader(const char* libName,
        bool closeWhenDestructed) :
        mHandle(0), mCloseOnDestroy(closeWhenDestructed)
{
    JELOG2(EUtils);
    JavaOsLayer::getOsSpecificLibName(mLibName, libName);
}

OS_EXPORT DynamicLibLoader::~DynamicLibLoader()
{
    JELOG2(EUtils);
    if (mCloseOnDestroy)
    {
        closeLib();
    }
}

OS_EXPORT void DynamicLibLoader::openLib()
{
    if (mHandle == 0)
    {
        LOG1(EUtils, EInfo, "Opening lib %s", mLibName.c_str());
        mHandle = JavaOsLayer::dlopen(mLibName.c_str());
        if (mHandle == 0)
        {
            std::string errorStr("Error opening lib.");
            errorStr.append(mLibName);
            throw LibraryLoaderException(OPENING_LIBRARY_FAILED,errorStr,
                                         __FILE__, __FUNCTION__, __LINE__);
        }
    }
}

OS_EXPORT void DynamicLibLoader::closeLib()
{
    if (mHandle)
    {
        LOG1(EUtils, EInfo, "Closing lib %s", mLibName.c_str());
        JavaOsLayer::dlclose(mHandle);
        mHandle = 0;
    }
}

OS_EXPORT void* DynamicLibLoader::getFunction(const char* functionName,
        bool containsByteCode)
{
    JELOG2(EUtils);
    LOG1(EUtils, EInfo, "Getting function %s", functionName);
    openLib(); //Ensure that the library has been opened.
    void* func = JavaOsLayer::dlsym(mHandle, functionName, containsByteCode);
    if (func == 0)
    {
        std::string errorStr("Error getting function ");
        errorStr.append(functionName);
        errorStr.append(" from library ");
        errorStr.append(mLibName);
        throw LibraryLoaderException(CANNOT_FIND_FUNCTION_FAILURE,errorStr,
                                     __FILE__, __FUNCTION__, __LINE__);
    }
    return func;
}

OS_EXPORT DynamicLibLoader*
DynamicLibLoader::loadAndGetFunction(void*&      functionPtr,
                                     const char* libName,
                                     const char* functionName)
{
    JELOG2(EUtils);

    std::auto_ptr<DynamicLibLoader> libLoader(
        new DynamicLibLoader(libName));

    //Throws exception if fails
    functionPtr = (libLoader->getFunction(functionName));

    //everything went ok so we must not free the libLoader.
    DynamicLibLoader* loader = libLoader.get();
    libLoader.release();
    return loader;
}
