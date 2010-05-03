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
* Description:  Main program for the JavaInstaller process
*
*/


#include <memory>
#include <sstream>
#include <unistd.h>

#include "exceptionbase.h"
#include "javaoslayer.h"
#include "javaprocessconstants.h"
#include "logger.h"
#include "runtimeexception.h"

#ifdef __SYMBIAN32__
#include "javasymbianoslayer.h"
#endif // __SYMBIAN32__

using namespace java::runtime;
using namespace java::util;

int main(int argc, char *argv[])
{
    JELOG(EJavaRuntime, "INSTALLEREXE main()");
    JavaOsLayer::startUpTrace("INSTALLEREXE main() start", -1, -1);

    int result = -1;
    try
    {
#ifdef __SYMBIAN32__
        std::auto_ptr<HBufC> installerProcess(
            stringToDes(java::runtime::JAVA_PROCESS));

        std::ostringstream params;
        params << java::runtime::JAVA_INSTALLER_STARTER_DLL << " ";
        for (int i = 1; i < argc; i++)
        {
            params << argv[i] << " "; // codescanner::accessArrayElementWithoutCheck2
        }
        std::auto_ptr<HBufC> installerArgs(stringToDes(params.str().c_str()));

        LOG1(EJavaRuntime, EInfo, "INSTALLEREXE main() command line: %s",
             params.str().c_str());

        RProcess rp; // codescanner::resourcenotoncleanupstack
        result = rp.Create(TPtr(installerProcess->Des()), TPtr(installerArgs->Des()));
        if (KErrNone == result)
        {
            // Wait for the exit status of JavaInstaller.
            TRequestStatus status;
            rp.Logon(status);
            rp.Resume();
            User::WaitForRequest(status);
            result = status.Int();
            rp.Close(); // free process kernel object
        }
        else
        {
            ELOG1(EJavaRuntime, "INSTALLEREXE main(): starting JavaInstaller failed, err=%d", result);
        }
#else // __SYMBIAN32__
        const char* av[argc + 2];
        av[0] = java::runtime::JAVA_PROCESS;
        av[1] = java::runtime::JAVA_INSTALLER_STARTER_DLL;
        for (int i = 1; i < argc; i++)
        {
            av[i+1] = argv[i];
        }
        av[argc+1] = NULL; // codescanner::accessArrayElementWithoutCheck2
        result = execvp(av[0], (char*const*)av);
#endif // __SYMBIAN32__
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "INSTALLEREXE main() RuntimeException caught: %s ",
              e.toString().c_str());
    }
    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "INSTALLEREXE main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "INSTALLEREXE main() Exception %s caught", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "INSTALLEREXE main() exit = %d", result);
    JavaOsLayer::startUpTrace("INSTALLEREXE main() end", -1, -1);
    return result;
}
