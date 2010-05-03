/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Thread dump server of J9 VM in S60,
*
*/


#ifndef THREADDUMPER_H
#define THREADDUMPER_H

#include <memory>
#include <memory.h>

#include "logger.h"
#include "dynamiclibloader.h"

namespace java
{
namespace util
{

class ThreadDump
{
public:
    static void doDump();
    static void closeDumper();
};

} // namespace util
} // namespace java



const char* const THREAD_DUMP_LIB = "javathreaddumper";

// Keep this in sync with doThreadDump() method.
typedef void (*DoThreadDump)();

// Keep this in sync with closeThreadDump() method.
typedef void (*CloseThreadDump)();



//START OF INLINE METHODS

inline void java::util::ThreadDump::doDump()
{
    try
    {
        std::auto_ptr<java::util::DynamicLibLoader>loader
        (new DynamicLibLoader("javathreaddumper"));
        DoThreadDump doThreadDump =
            (DoThreadDump)loader->getFunction("doThreadDump");
        doThreadDump();
    }
    catch (...)
    {
        // Ignoring all exceptions
    }
}

inline void java::util::ThreadDump::closeDumper()
{
    try
    {
        std::auto_ptr<java::util::DynamicLibLoader>loader
        (new DynamicLibLoader("javathreaddumper"));
        CloseThreadDump closeThreadDump =
            (CloseThreadDump)loader->getFunction("closeThreadDump");
        closeThreadDump();
    }
    catch (...)
    {
        // Ignoring all exceptions
    }
}

#endif // THREADDUMPER_H
