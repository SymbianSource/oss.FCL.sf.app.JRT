/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaFile
*
*/

#include "logger.h"
#include "mjavafile.h"
#include "javafile.h"

using namespace javaruntime;

//*****************************************************************************
CJavaFile::CJavaFile(): iJavaFileMode(EJavaFileClosed)
{
    JELOG2(EJVM);
}

//*****************************************************************************
CJavaFile::~CJavaFile()
{
    JELOG2(EJVM);
    Close();
}

//*****************************************************************************
void CJavaFile::OpenCafL(const TDesC& aFileName)
{
    JELOG2(EJVM);
    if (NULL == iCafContent)
    {
        iCafContent = ContentAccess::CContent::NewL(aFileName);
    }
    iCafData = iCafContent->OpenContentL(ContentAccess::EPeek);
    User::LeaveIfError(iCafData->ExecuteIntent(ContentAccess::EPeek));
}

//*****************************************************************************
TInt CJavaFile::Open(RFs& aFs,const TDesC& aFileName, TUint aFileMode)
{
    JELOG2(EJVM);
    if (iJavaFileMode != EJavaFileClosed)
    {
        // Not allowed if some file is already opened and not properly closed.
        return KErrInUse;
    }
    mFileName.assign((wchar_t*)aFileName.Ptr(), aFileName.Length());
    TInt status;
    if (isDrmFile(aFileName))
    {
        LOG1(EJVM, EInfo,"Opening file %S using CAF", mFileName.c_str());
        iJavaFileMode = EJavaFileOpenForCaf; //This is set before opening
        //in order to allow proper
        //cleanup if something fails.
        TRAP(status, OpenCafL(aFileName));
    }
    else
    {
        // Using RFile
        LOG1(EJVM, EInfo,"Opening file %S using RFile", mFileName.c_str());
        status = iNormalFile.Open(aFs, aFileName, aFileMode);
        if (status == KErrNone)
        {
            iJavaFileMode = EJavaFileOpenForNormalFile;
        }
    }
    if (status != KErrNone)
    {
        // Clean up and set the class ready for opening another file.
        Close();
    }
    return status;
}

//*****************************************************************************
void CJavaFile::Close()
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        LOG1(EJVM, EInfo,"Closing file %S using CAF", mFileName.c_str());
        delete iCafData;
        iCafData = NULL;
        delete iCafContent;
        iCafContent = NULL;
    }
    if (iJavaFileMode == EJavaFileOpenForNormalFile)
    {
        LOG1(EJVM, EInfo,"Closing file %S using RFile", mFileName.c_str());
        iNormalFile.Close();
    }
    iJavaFileMode = EJavaFileClosed;
}

//*****************************************************************************
TInt CJavaFile::Size(TInt& aSize) const
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Using CAF
        TInt status = KErrNone;
        TRAP(status, iCafData->DataSizeL(aSize));
        return status;
    }
    else
    {
        //Using RFile
        return iNormalFile.Size(aSize);
    }
}

//*****************************************************************************
TInt CJavaFile::SetSize(TInt aSize)
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.SetSize(aSize);
    }
}

//*****************************************************************************
TInt CJavaFile::Create(RFs& aFs,const TDesC& aName,TUint aFileMode)
{
    JELOG2(EJVM);
    if (iJavaFileMode != EJavaFileClosed)
    {
        //Not allowed if some file is already opened and not properly closed.
        return KErrInUse;
    }
    //Using RFile
    TInt status = iNormalFile.Create(aFs, aName, aFileMode);
    if (status == KErrNone)
    {
        iJavaFileMode = EJavaFileOpenForNormalFile;
    }
    return status;
}

//*****************************************************************************
TInt CJavaFile::Read(TDes8& aDes) const
{
    JELOG4(EJVM, EEntry & EInfoHeavyLoad);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Using CAF
        return iCafData->Read(aDes);
    }
    else
    {
        //Using RFile
        return iNormalFile.Read(aDes);
    }
}

//*****************************************************************************
TInt CJavaFile::Read(TDes8& aDes, TInt aLength) const
{
    JELOG4(EJVM, EEntry & EInfoHeavyLoad);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Using CAF
        return iCafData->Read(aDes, aLength);
    }
    else
    {
        //Using RFile
        return iNormalFile.Read(aDes, aLength);
    }
}

//*****************************************************************************
TInt CJavaFile::Replace(RFs& aFs,const TDesC& aName,TUint aFileMode)
{
    JELOG2(EJVM);
    //Using RFile
    if (iJavaFileMode != EJavaFileClosed)
    {
        // Not allowed if some file is already opened and not properly closed.
        return KErrInUse;
    }
    TInt status = iNormalFile.Replace(aFs, aName, aFileMode);
    if (status == KErrNone)
    {
        iJavaFileMode = EJavaFileOpenForNormalFile;
    }
    else
    {
        //Clean the environment just in case.
        Close();
    }
    return status;
}



//*****************************************************************************
TInt CJavaFile::Write(const TDesC8& aDes)
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Write(aDes);
    }
}

//*****************************************************************************
TInt CJavaFile::Write(const TDesC8& aDes,TInt aLength)
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Write(aDes, aLength);
    }
}

//*****************************************************************************
TInt CJavaFile::Write(TInt aPos,const TDesC8& aDes)
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Write(aPos, aDes);
    }
}

//*****************************************************************************
TInt CJavaFile::Write(TInt aPos,const TDesC8& aDes,TInt aLength)
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Write(aPos, aDes, aLength);
    }
}

//*****************************************************************************
TInt CJavaFile::Lock(TInt aPos, TInt aLength) const
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Lock(aPos, aLength);
    }
}

//*****************************************************************************
TInt CJavaFile::UnLock(TInt aPos, TInt aLength) const
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.UnLock(aPos, aLength);
    }
}

//*****************************************************************************
TInt CJavaFile::Seek(TSeek aMode,TInt& aPos) const
{
    JELOG4(EJVM, EEntry & EInfoHeavyLoad);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Using CAF
        return iCafData->Seek(aMode, aPos);
    }
    else
    {
        //Using RFile
        return iNormalFile.Seek(aMode, aPos);
    }
}

//*****************************************************************************
TInt CJavaFile::Flush()
{
    JELOG2(EJVM);
    if (iJavaFileMode == EJavaFileOpenForCaf)
    {
        //Not supported for files accessed via CAF.
        return KErrNotSupported;
    }
    else
    {
        //Using RFile
        return iNormalFile.Flush();
    }
}

//*****************************************************************************
TBool CJavaFile::isDrmFile(const TDesC& aFileName)
{
    TParse filename;
    filename.Set(aFileName, NULL, NULL);
    TPtrC filenameExt = filename.Ext();

    _LIT(KDmExtension, ".dm");
    _LIT(KDcfExtension, ".dcf");
    if (filenameExt.CompareF(KDmExtension) == 0 ||
            filenameExt.CompareF(KDcfExtension) == 0)
    {
        // Filename extension indicates this is a DRM protected file.
        return ETrue;
    }

    _LIT(KJarExtension, ".jar");
    if (filenameExt.CompareF(KJarExtension) != 0)
    {
        // Filename extension indicates this is not a jar file,
        // assume it is not DRM protected.
        return EFalse;
    }

    RProcess thisProcess;
    TName thisProcessName = thisProcess.Name();
    _LIT(KInstallerName, "Installer");
    if (KErrNotFound == thisProcessName.FindF(KInstallerName))
    {
        // We are not running in JavaInstaller process,
        // do not check if file is DRM protected from CAF
        // but assume that file is not DRM protected.
        return EFalse;
    }

    // Check from CAF if file is DRM protected.
    TBool drmFile = EFalse;
    TRAP_IGNORE(iCafContent = ContentAccess::CContent::NewL(aFileName));
    if (NULL != iCafContent)
    {
        TInt value = 0;
        TInt err = iCafContent->GetAttribute(
                       ContentAccess::EIsProtected, value);
        if (KErrNone == err && value)
        {
            // This is a DRM file.
            drmFile = ETrue;
        }
        else
        {
            // Not a DRM file, delete iCafContent instance.
            delete iCafContent;
            iCafContent = NULL;
        }
    }
    return drmFile;
}
