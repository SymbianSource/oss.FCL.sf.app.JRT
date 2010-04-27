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
* Description:  FileManager - Provides file management facility
 *
*/


#include <f32file.h>

#include "logger.h"
#include "s60commonutils.h"

#include "filemanager.h"

using namespace java::fileutils;
using namespace java::util;

void ReplaceCharacters(TPtr& aPtr, TUint8 aReplacedChar, TUint8 aNewChar,
                       TBool aOnlyFirstMatch)
{
    for (TInt i = 0; i < aPtr.Length(); ++i)
    {
        if (aReplacedChar == aPtr[i])
        {
            aPtr[i] = aNewChar;
            if (aOnlyFirstMatch)
                return;
        }
    }
}

void ReplaceCharacters(TPtr& aPtr, TUint8 aReplacedChar, TUint8 aNewChar)
{
    ReplaceCharacters(aPtr, aReplacedChar, aNewChar, EFalse);
}

bool FileManager::copy(std::wstring aSource, std::wstring aDest, bool override)
{
    JELOG2(EJavaFile);

    LOG1(EJavaFile, EInfo, "FileManager::copy: Source     : %S",
         aSource.c_str());
    LOG1(EJavaFile, EInfo, "FileManager::copy: Destination: %S", aDest.c_str());

    HBufC* source = S60CommonUtils::wstringToDes(aSource.c_str());
    HBufC* dest = S60CommonUtils::wstringToDes(aDest.c_str());

    TPtr sourcePtr(source->Des());
    TPtr destPtr(dest->Des());

    ReplaceCharacters(sourcePtr, '/', '\\');
    ReplaceCharacters(destPtr, '/', '\\');

    RFs fsSession;
    int error = fsSession.Connect();
    bool retVal = false;

    if (KErrNone == error)
    {
        // Create file management object
        CFileMan* fileMan = 0;
        TRAP(error, fileMan = CFileMan::NewL(fsSession));

        if (KErrNone == error)
        {
            TRAP(error,
                 CleanupStack::PushL(fileMan);

                 // Do copy (here synchronously)
                 TInt option = CFileMan::ENone;
                 if (override)
        {
            option = CFileMan::EOverWrite;
        }

        TInt success = fileMan->Copy(sourcePtr, destPtr, option);

        if (KErrNone == success)
        {
            retVal = true;

        }
        else
        {
            WLOG1(EJavaFile, "Copy Failed: Error: %d", success);
            }
            // Clean up
            CleanupStack::PopAndDestroy();
            // close file server session
            fsSession.Close();
                );
        }
    }
    delete source;
    delete dest;
    return retVal;
}

bool FileManager::move(std::wstring aSource, std::wstring aDest, bool override)
{
    JELOG2(EJavaFile);

    LOG1(EJavaFile, EInfo, "FileManager::move: Source     : %S",
         aSource.c_str());
    LOG1(EJavaFile, EInfo, "FileManager::move: Destination: %S", aDest.c_str());

    HBufC* source = S60CommonUtils::wstringToDes(aSource.c_str());
    HBufC* dest = S60CommonUtils::wstringToDes(aDest.c_str());

    TPtr sourcePtr(source->Des());
    TPtr destPtr(dest->Des());

    ReplaceCharacters(sourcePtr, '/', '\\');
    ReplaceCharacters(destPtr, '/', '\\');

    RFs fsSession;
    int error = fsSession.Connect();
    bool retVal = false;

    if (KErrNone == error)
    {
        // Create file management object
        CFileMan* fileMan = 0;
        TRAP(error, fileMan = CFileMan::NewL(fsSession));

        if (KErrNone == error)
        {
            TRAP(error,
                 CleanupStack::PushL(fileMan);

                 // Do copy (here synchronously)
                 TInt option = CFileMan::ENone;
                 if (override)
        {
            option = CFileMan::EOverWrite;
        }

        TInt success = fileMan->Move(sourcePtr, destPtr, option);
        if (KErrNone == success)
        {
            retVal = true;

        }
        else
        {
            WLOG1(EJavaFile, "Move Failed: Error: %d", success);
            }
            // Clean up
            CleanupStack::PopAndDestroy();
            // close file server session
            fsSession.Close();
                );
        }
    }
    delete source;
    delete dest;
    return retVal;
}

bool FileManager::copyAll(std::wstring aSource, std::wstring aDest,
                          bool override)
{
    JELOG2(EJavaFile);

    LOG1(EJavaFile, EInfo, "FileManager::copyAll: Source     : %S",
         aSource.c_str());
    LOG1(EJavaFile, EInfo, "FileManager::copyAll: Destination: %S",
         aDest.c_str());

    HBufC* source = S60CommonUtils::wstringToDes(aSource.c_str());
    HBufC* dest = S60CommonUtils::wstringToDes(aDest.c_str());

    TPtr sourcePtr(source->Des());
    TPtr destPtr(dest->Des());

    ReplaceCharacters(sourcePtr, '/', '\\');
    ReplaceCharacters(destPtr, '/', '\\');

    RFs fsSession;
    int error = fsSession.Connect();
    bool retVal = false;

    if (KErrNone == error)
    {
        // Create file management object
        CFileMan* fileMan = 0;
        TRAP(error, fileMan = CFileMan::NewL(fsSession));

        if (KErrNone == error)
        {
            LOG(EJavaFile, EInfo, "FileManager::copyAll Creating FileMan ok");
            TRAP(error,
                 CleanupStack::PushL(fileMan);

                 // Do copy (here synchronously)
                 TInt option = CFileMan::ERecurse;
                 // First recurse through sub directories.
                 TInt success = fileMan->Copy(sourcePtr, destPtr, option);
                 LOG1(EJavaFile, EInfo,
                      "FileManager::copyAll First Copy: %d", success);

                 if (override)
        {
            LOG(EJavaFile, EInfo,
                "FileManager::copyAll Setting option to overwrite");
                option = CFileMan::EOverWrite;

                // now overwrite.
                success = fileMan->Copy(sourcePtr, destPtr, option);
                LOG1(EJavaFile, EInfo,
                     "FileManager::copyAll Second Copy: %d", success);
            }

            // It will already be ensured that source and destination
            // exist. So, KErrNotFound will only be returned if the
            // folders are empty. In which case, it is still a success.
            if ((KErrNone == success) || (KErrNotFound == success))
        {
            retVal = true;
        }
        else
        {
            WLOG1(EJavaFile,
                  "FileManager::copyAll Failed: Error: %d",
                  success);
                retVal = false;
            }

            // Clean up
            CleanupStack::PopAndDestroy();
            // close file server session
            fsSession.Close();
                );
        }
    }
    delete source;
    delete dest;
    LOG1(EJavaFile, EInfo, "FileManager::copyAll: returns: %d", retVal);
    return retVal;
}

bool FileManager::deleteAll(std::wstring aSource)
{
    JELOG2(EJavaFile);

    LOG1(EJavaFile, EInfo, "FileManager::deleteAll: Source     : %S",
         aSource.c_str());

    HBufC* source = S60CommonUtils::wstringToDes(aSource.c_str());
    TPtr sourcePtr(source->Des());
    ReplaceCharacters(sourcePtr, '/', '\\');

    RFs fsSession;
    int error = fsSession.Connect();
    bool retVal = false;

    if (KErrNone == error)
    {
        // Create file management object
        CFileMan* fileMan = 0;
        TRAP(error, fileMan = CFileMan::NewL(fsSession));

        if (KErrNone == error)
        {
            TRAP(error,
                 CleanupStack::PushL(fileMan);

                 LOG(EJavaFile, EInfo,
                     "FileManager::deleteAll Creating FileMan ok");
                 // First, lets clear all read-only attributes from files in
                 // the folder
                 TInt success = fileMan->Attribs(sourcePtr, 0,
                                                 KEntryAttReadOnly,
                                                 0, CFileMan::ERecurse);

                 // Ignoring return value of attribs()
                 LOG1(EJavaFile, EInfo,
                      "FileManager::deleteAll() Changing attribs: Returns %d",
                      success);

                 success = fileMan->RmDir(sourcePtr);

                 if (KErrNone == success)
        {
            retVal = true;
        }
        else
        {
            WLOG1(EJavaFile, "Delete All Failed: Error: %d",
                  success);
            }
            // Clean up
            CleanupStack::PopAndDestroy();
            // close file server session
            fsSession.Close();
                );
        }
    }
    delete source;
    return retVal;
}
