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
* Description:  Functionality for reading extended bootclasspath in Symbian env
*
*/

#include <centralrepository.h>

#include "javaruntimeprivatecrkeys.h"
#include "logger.h"
#include "bootclasspath.h"


HBufC* GetCRepClasspathL()
{
    _LIT(KAllowedPathPrefix1, "c:\\resource\\");
    _LIT(KAllowedPathPrefix2, "c:/resource/");
    const TChar KClasspathSeparator(';');

    JELOG2(EJavaRuntime);
    CRepository* repository = CRepository::NewL(KCRUidJavaRuntime);
    CleanupStack::PushL(repository);

    HBufC* path = HBufC::NewL(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr ptr = path->Des();
    TInt err = repository->Get(KJavaRuntimeMIDPClasspath, ptr);
    CleanupStack::PopAndDestroy(repository);

    if ((err == KErrNotFound) || (ptr.Length() == 0))
    {
        LOG(EJavaRuntime, EInfo, "No KJavaRuntimeMIDPClasspath key in Cen Rep");
        delete path;
        return 0;
    }
    else if (err != KErrNone)
    {
        ELOG1(EJavaRuntime, "Cenral Repository error %d", err);
        delete path;
        return 0;
    }

    // Only extensions installed to C:/resource/ are accepted.
    // If KJavaRuntimeMIDPClasspath contains extensions installed
    // to some other (unsafe) place, remove them from classpath
    // and store the pruned classpath back to central repository
    // The classpath read from central repository may be for example
    // "C:/resource/java/lib/eswt.jar;;E:/Resource/java/lib/payment.jar
    // ;c:\\resource\\java\\lib\\mia.jar;  "

    TInt  cpStart(0);              // start position of class path in class paths
    TInt  cpLength(0);             // length of of class path in class paths
    TBool updateCR(EFalse);        // ETrue if cen rep must be updated

    // Go through all class paths
    while (cpStart < ptr.Length() && cpLength < ptr.Length())
    {
        // Length of the class path, class paths are separated ';'
        cpLength = ptr.MidTPtr(cpStart).LocateF(KClasspathSeparator);
        if (cpLength == KErrNotFound)
        {
            // The class path is last one without ';', cpLength is rest from start position to total length
            cpLength = ptr.Length() - cpStart;
        }
        else
        {
            // Include ';'
            cpLength++;
        }

        // Process class path
        TPtr tmpPtr = ptr.MidTPtr(cpStart, cpLength);

        if ((tmpPtr.Left(((TDesC)KAllowedPathPrefix1).Length()).CompareF(KAllowedPathPrefix1)) &&
                (tmpPtr.Left(((TDesC)KAllowedPathPrefix2).Length()).CompareF(KAllowedPathPrefix2)))
        {
            // Unsafe class path, delete it
            ptr.Delete(cpStart, cpLength);
            // Start of the next class path is start of the current deleted class path
            cpLength = 0;
            updateCR = ETrue;
        }
        else
        {
            // Allowed class path
            // Start of the next class path is end of the current class path
            cpStart = cpStart + cpLength;
        }
    }

    if (!updateCR)
    {
        // Classpath does not contain any dangerous paths
        return path;
    }

    // Store the cleaned classpath back to central repository (to optimize
    // next execution)
    WLOG(EJavaRuntime,
         "GetCRepClasspathL: Removing unsafe extension(s) from Cen Rep extension classpath");
    CleanupStack::PushL(path);
    repository = CRepository::NewL(KCRUidJavaRuntime);
    err = repository->Set(KJavaRuntimeMIDPClasspath, ptr);
    delete repository;
    if (err != KErrNone)
    {
        WLOG1(EJavaRuntime,
              "GetCRepClasspathL could not store cleaned classpath, err %d", err);
    }

    if (ptr.Length() > 0)
    {
        CleanupStack::Pop(path);
        return path;
    }
    else
    {
        WLOG(EJavaRuntime, "Cleaned classpath is empty");
        // nothing to return
        CleanupStack::PopAndDestroy(path);
        return 0;
    }
}

void java::runtime::getExtendedBootClassPath(std::wstring& bootClassPath)
{
    JELOG2(EJavaRuntime);
    HBufC* path = 0;
    TRAP_IGNORE(path = GetCRepClasspathL());
    if (path != 0)
    {
        LOG1(EJavaRuntime, EInfo, "  Length of the bcp %d", path->Length());
        if (path->Length() > 0)
        {
            bootClassPath.assign((wchar_t*)path->Ptr(),
                                 path->Length());
            LOG1(EJavaRuntime, EInfo, "  bcp '%S'", bootClassPath.c_str());
        }
        delete path;
    }
}
