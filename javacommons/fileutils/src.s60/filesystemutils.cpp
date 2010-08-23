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
* Description:  FileSystemUtilities - Wrapper to S60 Specific implementation
 *
*/


#include <memory>
#include <f32file.h>
#include "logger.h"
#include "jstringutils.h"
#include "s60commonutils.h"
#include "fileutilities.h"
#include "s60filesystemutilities.h"
#include "javasymbianoslayer.h"

#define BUF_LEN 255
using namespace std;
using namespace java::util;
using namespace java::fileutils;

/*
 * Implementation of FileUtilities.h for C++ methods
 *
 */
OS_EXPORT long long FileUtilities::availableSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    TInt64 size = -1;

    //In S60, first character is the drive letter.
    TChar driveChar(aDrive.at(0));
    TRAPD(err, S60FileSystemUtilities::GetAvailableSizeL(size, driveChar));

    if (KErrNone != err)
    {
        ELOG1(EJavaFile, "  FileUtilities::availableSize: Error: %d", err);
        return -1;
    }
    return (size);
}

OS_EXPORT long long FileUtilities::usedSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    TInt64 size = 0;
    TChar driveChar(aDrive.at(0));
    TRAPD(err, S60FileSystemUtilities::GetUsedSizeL(size, driveChar));

    if (KErrNone != err)
    {
        ELOG1(EJavaFile, "  FileUtilities::usedSize: Error: %d", err);
        return -1;
    }
    return (size);
}

OS_EXPORT long long FileUtilities::totalSize(const std::wstring& aDrive)
{
    JELOG2(EJavaFile);
    TInt64 size = 0;
    TChar driveChar(aDrive.at(0));
    TRAPD(err, S60FileSystemUtilities::GetTotalSizeL(size, driveChar));

    if (KErrNone != err)
    {
        ELOG1(EJavaFile, "  FileUtilities::totalSize: Error: %d", err);
        return -1;
    }
    return (size);
}

OS_EXPORT bool FileUtilities::isHidden(const std::wstring& aFile)
{
    JELOG2(EJavaFile);
    HBufC* name = S60CommonUtils::wstringToDes(aFile.c_str());
    //TDesC* str = &name;
    TBool hidden = EFalse;
    TRAP_IGNORE(S60FileSystemUtilities::IsHiddenL(hidden, name->Des()));
    delete name;
    return *reinterpret_cast<bool*>(&hidden);
}

OS_EXPORT std::wstring FileUtilities::listRoots()
{
    JELOG2(EJavaFile);
    TBuf<BUF_LEN> buf;
    buf.Copy(S60FileSystemUtilities::GetSupportedDrives());    
    std::wstring retString((wchar_t*)buf.Ptr());
    return retString;
}

OS_EXPORT int FileUtilities::setReadable(const std::wstring& /*aFile*/, bool /*aReadable*/)
{
    // Note: In Symbian, we do not set the readable attribute to no access.
    // it is just ignored and canRead will always return true
    return 0;
}

OS_EXPORT int FileUtilities::setWritable(const std::wstring& aFile,
        bool aWritable)
{
    JELOG2(EJavaFile);
    HBufC* name = S60CommonUtils::wstringToDes(aFile.c_str());
    TPtr uriBufPtr(name->Des());
    S60FileSystemUtilities::ReplaceCharacters(uriBufPtr, '/', '\\');
    RFs iRFs;
    int err = iRFs.Connect();
    if (KErrNone != err)
    {
        throw err;
        //return -1;
    }

    if (aWritable)
        err = iRFs.SetAtt(uriBufPtr, 0, KEntryAttReadOnly);
    else
        err = iRFs.SetAtt(uriBufPtr, KEntryAttReadOnly, 0);

    iRFs.Close();
    delete name;
    if (KErrNone != err)
    {
        throw err;
    }
    return 0;
}


OS_EXPORT std::wstring FileUtilities::getDirContents(const std::wstring& dName,
        const bool aIncludeHidden)
{
    JELOG2(EJavaFile);
    LOG(EJavaFile, EInfo, "+FileUtilities::getDirContents()");

    std::wstring dirName = dName + L"/";
    auto_ptr<HBufC> name(0);
    name.reset(S60CommonUtils::wstringToDes(dirName.c_str()));

    TPtr uriBufPtr(name->Des());
    S60FileSystemUtilities::ReplaceCharacters(uriBufPtr, '/', '\\');
    std::wstring iFileListWrapper;

    RFs rFileServer;
    TInt error = rFileServer.Connect();

    LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContents: RFs connect: %d",
         error);

    if (KErrNone == error)
    {
        CDir* fileAndDirList = NULL;

        // Include normal and system files
        TInt attributeList = KEntryAttNormal|KEntryAttSystem|KEntryAttDir;
        if (aIncludeHidden)
        {
            attributeList |= KEntryAttHidden;
        }

        error = rFileServer.GetDir(uriBufPtr, attributeList, ESortByName,
                                   fileAndDirList);

        LOG1(EJavaFile, EInfo, "  FileUtilities::getDirContents: RFs getdir: %d",
             error);

        if (KErrNone == error)
        {
            for (TInt index =0; index< fileAndDirList->Count(); index++)
            {
                TEntry entry = (*fileAndDirList)[index];
                TPtr16 TName = entry.iName.Des();
                std::wstring retString((wchar_t*)desToWstring(TName));
                LOG1(EJavaFile, EInfo, " FileUtilities::getDirContents:getDir: %S", retString.c_str());

                iFileListWrapper += retString;

                if (entry.IsDir())
                {
                    // Append slash to indicate directory
                    iFileListWrapper += L"/*";
                }
                else
                {
                    iFileListWrapper += L"*";
                }
            }
            delete fileAndDirList;
        }
    }
    LOG(EJavaFile, EInfo, "-FileUtilities::getDirContents()");
    return iFileListWrapper;
}
