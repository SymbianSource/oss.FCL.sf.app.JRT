/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides container for message.
*
*/


/*

MISSING FEATURES:

Localized names for MIDlets
 The developer should be able to define localized names for MIDlets.
 The proposed format is described in RM-RIM CR 417-16269:
 Java: Support for localized application name.
 MIDP3 will define also format for the same purpose.

*/

#include <unistd.h>

#include "logger.h"
#include "javaoslayer.h"
#include "runtimeexception.h"
#include "exceptionbase.h"

#include "midpruntimestarter.h"

#ifdef __SYMBIAN32__
#include "javauids.h"
#endif

using namespace java::runtime;
using namespace java::util;


#ifndef __SYMBIAN32__
extern "C"
#endif //__SYMBIAN32__
int dllMain(int argc, char *argv[])
{
    JELOG(EJavaRuntime, "MIDP main()");
    LOG(EJavaRuntime, EInfo, "MipdRuntime started");
#ifdef __SYMBIAN32__
#ifndef _DEBUG
    // The only process allowed to start the MIDP is Java Captain.
    // Check is done only by release build.
    TSecureId sid = User::CreatorSecureId();
    if (sid != KJavaCaptainUid)
    {
        ELOG1(EJavaRuntime, "MipdRuntime main() creted by process having "
              "secure id: %X ", sid.iId);
        return -1;
    }
#endif
#endif

    JavaOsLayer::startUpTrace("Midp main", -1, -1);
    int result = -1;
    try
    {
        // MidpRuntimeStarter object is used to start the MIDP runtime.
        std::auto_ptr<MidpRuntimeStarter>
        starter(new MidpRuntimeStarter()); // codescanner::nonleavenew
        result = starter->start(argc, argv);
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "MipdRuntime main() RuntimeException catched: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "MipdRuntime main() ExceptionBase catched: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {

        ELOG1(EJavaRuntime, "MipdRuntime main() Exception %s catched", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "MipdRuntime exited with status %d", result);
    return result;
}


#ifdef __SYMBIAN32__
#include "javasymbianoslayer.h"

/**
 * Finds exported methods of this dll. This is used by the utility
 * DynamicLibLoader.
 * @param funcName Name of the method the be searched.
 * @param indetifier A string identifier for different runtimes. This
 *        identifier is passed to the JVM args modifier.
 * @return valid function pointer to the searched methodon succes case, 0
 *         on error case.
 */
EXPORT_C FuncPtr findDllMethod(const char* methodName)
{
    FuncPtr ptr = 0;
    if (strcmp(methodName, "dllMain") == 0)
    {
        ptr = (FuncPtr)dllMain;
    }
    return ptr;
}
#endif //__SYMBIAN32__
