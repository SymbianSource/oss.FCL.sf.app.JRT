/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef DYNAMICLIBLOADER_H
#define DYNAMICLIBLOADER_H

#include <string>
#include "javaosheaders.h"

namespace java
{
namespace util
{

//Opening dynamic library failed.
const int OPENING_LIBRARY_FAILED = -20000;

//Getting function from dynamic library failed.
const int CANNOT_FIND_FUNCTION_FAILURE = -20001;

class DynamicLibLoader
{
public:
    //Constructor(s) & destructor

    /**
     * Creates object for loading the given library dynamically.
     * @param libName The name of the lib to be loaded.
     * @param closeWhenDestructed Wheter to unload the library when the
     *        object is destroyed. If is false, will create a memory leak.
     */
    OS_IMPORT DynamicLibLoader(const char* libName);

    /**
     * Creates object for loading the given library dynamically.
     * @param libName The name of the lib to be loaded.
     * @param closeWhenDestructed Wheter to unload the library when the
     *        object is destroyed. If is false, will create a memory leak.
     */
    OS_IMPORT DynamicLibLoader(const char* libName, bool closeWhenDestructed);


    OS_IMPORT virtual ~DynamicLibLoader();

    /**
     * Opens the library
     * @param containsByteCode For some OS it is relevant to know if the dll
     *        contains rommized byte code.
     * @throw LibraryLoaderException with error code OPENING_LIBRARY_FAILED
     *         if loading a library failed.
     */
    OS_IMPORT void openLib();

    /**
     * @throw This operation does not throw exceptions.
     */
    OS_IMPORT void closeLib();

    /**
     * @throw LibraryLoaderException with following error codes:
     *         OPENING_LIBRARY_FAILED => loading a library failed.
     *         CANNOT_FIND_FUNCTION_FAILURE => function cannot be found from the library.
     */
    OS_IMPORT void* getFunction(const char* functionName, bool containsByteCode = false);

    /**
     * @throw See exceptions from getFunction(const char*) operation.
     */
    OS_IMPORT static DynamicLibLoader* loadAndGetFunction(void*&  functionPtr,
            const char* libName,
            const char* functionName);

private: //Methods
    DynamicLibLoader();                                     //No default constructor allowed
    DynamicLibLoader(const DynamicLibLoader&);              //No copy constructor allowed
    DynamicLibLoader& operator= (const DynamicLibLoader&);  //No Assignment operator allowed

private: //Members
    std::string  mLibName;
    void*        mHandle;
    bool         mCloseOnDestroy;
};

} //end namespace util
} //end namespace java

#endif // DYNAMICLIBLOADER_H
