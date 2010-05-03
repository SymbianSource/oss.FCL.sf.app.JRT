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
* Description:  This main entry point of Java processes.
*
*/


#include <unistd.h>
#include <string.h>
#include <memory>

#include "logger.h"
#include "javaoslayer.h"
#include "exceptionbase.h"
#include "dynamiclibloader.h"

#include "javaprocessconstants.h"

using namespace java::util;

typedef int (*DllmainFunc)(int argc, const char *argv[]);


const char* allowedDlls[] = {java::runtime::JAVA_MIDP_STARTER_DLL,
                             java::runtime::JAVA_INSTALLER_STARTER_DLL,
                             java::runtime::JAVA_PREINSTALLER_STARTER_DLL,
                             java::runtime::JAVA_SIZE_HELPER_SERVER_DLL
                            };

const int tableSize = sizeof(allowedDlls) / sizeof(char*);
int main(int argc, const char *argv[])
{
    JELOG(EJavaRuntime, "Java main()");
    LOG(EJavaRuntime, EInfo, "JAVA MAIN STARTED");
    JavaOsLayer::startUpTrace("Java main", -1, -1);

    int result = -1;
    try
    {
        //Verifying that the dll name has been provided
        if (argc > 1)
        {
            const char* libName = argv[1];
            for (int i = 0; i<tableSize; ++i)
            {
                if (strcmp(libName, allowedDlls[i]) == 0) // codescanner::accessArrayElementWithoutCheck2
                {
                    result = 0;
                    break;
                }
            }
            if (result != 0)
            {
                ELOG1(EJavaRuntime, "Tried to create Java process with "
                      "illegal dll: %s ", libName);
                return -1;
            }

            //Loading the defined library
            std::auto_ptr<DynamicLibLoader>
            lib(new DynamicLibLoader(libName)); // codescanner::nonleavenew
            DllmainFunc dllMain = (DllmainFunc)(lib->getFunction("dllMain"));

            argv[1] = argv[0];

            //Calling the main entry point.
            result = dllMain(argc-1, &argv[1]); // codescanner::accessArrayElementWithoutCheck2
            lib->closeLib();
        }
        else
        {
            ELOG(EJavaRuntime, "Not enough arguments for loading the runtime "
                 "dll.");
        }
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "Java main() ExceptionBase catched: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {

        ELOG1(EJavaRuntime, "Java main() Exception %s catched", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "JAVA MAIN EXIT = %d", result);
    return result;
}
