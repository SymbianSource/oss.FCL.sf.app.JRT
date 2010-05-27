/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <string>
#include <javastarter.h>
#include "logger.h"

int main()
{
    const char* const fileName = "c:\\java\\jvmargs.txt";

    int res = 0;
    struct stat buf;

    if (stat(fileName, &buf) == 0)
    {
        PLOG(EJavaRuntime, "javatest: using arguments from file");
        res = java::start(fileName);
    }
    else
    {
        PLOG(EJavaRuntime, "javatest: using default arguments");
        const char* argv[] = { "-profile=standalonemidlet",
                               "-conf=cldc",
                               "-jar",
                               "c:\\java\\midlet.jar" };
        int argc = sizeof(argv) / sizeof(argv[0]);
        res = java::start(argc, argv);
    }

    return res;
}
