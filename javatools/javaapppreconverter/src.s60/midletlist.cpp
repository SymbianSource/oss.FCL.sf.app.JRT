/*
* Copyright (c) 2008 - 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Java platform 2.0 javaapppreconverter process
*
*/

#include "midletlist.h"



CMidletInfo::CMidletInfo() :
        iMidletUid(TUid::Null()), iMidletId(KMaxTUint32), iSuiteId(KMaxTUint32),
        iMidletName(NULL), iGroupName(NULL), iDrive(EDriveE)
{
}

CMidletInfo::CMidletInfo(CMidletInfo* aMidletInfo)
{
    iMidletUid = aMidletInfo->GetMidletUid();
    iMidletId = aMidletInfo->GetMidletId();
    iSuiteId = aMidletInfo->GetSuiteId();
    iMidletName = aMidletInfo->GetMidletName().Alloc();
    iGroupName = aMidletInfo->GetGroupName().Alloc();
    iDrive = aMidletInfo->GetDrive();
    iIconFileName = aMidletInfo->GetIconFileName();
}


CMidletInfo::~CMidletInfo()
{
    delete iMidletName;
    delete iGroupName;
}


TUid CMidletInfo::GetMidletUid() const
{
    return iMidletUid;
}

TUint32 CMidletInfo::GetMidletId() const
{
    return iMidletId;
}

TUint32 CMidletInfo::GetSuiteId() const
{
    return iSuiteId;
}

const TDesC& CMidletInfo::GetMidletName()
{
    if (NULL == iMidletName)
    {
        return KNullDesC;
    }
    else
    {
        return *iMidletName;
    }
}

const TDesC& CMidletInfo::GetGroupName()
{
    if (NULL == iGroupName)
    {
        return KNullDesC;
    }
    else
    {
        return *iGroupName;
    }
}

TDriveNumber CMidletInfo::GetDrive()
{
    return iDrive;
}

const TFileName& CMidletInfo::GetIconFileName()
{
    return iIconFileName;
}




void CMidletInfo::SetMidletUid(const TUid aUid)
{
    iMidletUid = aUid;
}

void CMidletInfo::SetMidletId(const TUint32 aId)
{
    iMidletId = aId;
}

void CMidletInfo::SetSuiteId(const TUint32 aId)
{
    iSuiteId = aId;
}

void CMidletInfo::SetMidletName(const TDesC& aName)
{
    iMidletName = aName.Alloc();
}

void CMidletInfo::SetGroupName(const TDesC& aName)
{
    iGroupName = aName.Alloc();
}

void CMidletInfo::SetDrive(const TDriveNumber aDrive)
{
    iDrive = aDrive;
}

void CMidletInfo::SetIconFileName(HBufC16 *aIconFileName)
{
    iIconFileName = *aIconFileName;
}

void CMidletInfo::SetIconFileName(const TFileName &aIconFileName)
{
    iIconFileName = aIconFileName;
}

void CMidletInfo::ExternalizeL(RWriteStream& aStream) const
{
    aStream.WriteUint32L(iMidletUid.iUid);
    aStream.WriteUint32L(iMidletId);
    aStream.WriteUint32L(iSuiteId);

    TInt32 nLen = 0;
    if (iMidletName)
    {
        nLen = iMidletName->Length();
    }
    aStream.WriteInt32L(nLen);
    if (nLen > 0)
    {
        aStream.WriteL(*iMidletName);
    }

    nLen = 0;
    if (iGroupName)
    {
        nLen = iGroupName->Length();
    }
    aStream.WriteInt32L(nLen);
    if (nLen > 0)
    {
        aStream.WriteL(*iGroupName);
    }

    aStream.WriteUint32L(iDrive);

    nLen = iIconFileName.Length();
    aStream.WriteInt32L(nLen);
    if (nLen > 0)
    {
        aStream.WriteL(iIconFileName);
    }

}

void CMidletInfo::InternalizeL(RReadStream& aStream)
{
    iMidletUid.iUid = aStream.ReadUint32L();
    iMidletId = aStream.ReadUint32L();
    iSuiteId = aStream.ReadUint32L();

    TInt32 nLen = 0;
    nLen = aStream.ReadInt32L();
    if (nLen > 0)
    {
        if (!iMidletName)
        {
            iMidletName = HBufC16::NewL(nLen + 8);
        }

        TPtr16 tmpDes = iMidletName->Des();
        aStream.ReadL(tmpDes, nLen);
    }
    else
    {
        if (!iMidletName)
        {
            // empty midlet name
            iMidletName = HBufC16::NewL(8);
        }
    }

    nLen = aStream.ReadInt32L();
    if (nLen > 0)
    {
        if (!iGroupName)
        {
            iGroupName = HBufC16::NewL(nLen + 8);
        }
        TPtr16 tmpDes = iGroupName->Des();
        aStream.ReadL(tmpDes, nLen);
    }
    else
    {
        if (!iGroupName)
        {
            // empty group name
            iGroupName = HBufC16::NewL(8);
        }
    }

    iDrive = (TDriveNumber)aStream.ReadUint32L();

    nLen = aStream.ReadInt32L();
    if (nLen > 0)
    {
        aStream.ReadL(iIconFileName, nLen);
    }
}

void CMidletInfo::ToString8(TDes8& aDescription)
{
    aDescription.Append(_L8("Uid: "));
    aDescription.AppendNumFixedWidth(iMidletUid.iUid, EHex, 8);
    aDescription.Append(_L8("\nMidlet id: "));
    aDescription.AppendNumFixedWidth(iMidletId, EDecimal, 8);
    aDescription.Append(_L8("\nSuite id: "));
    aDescription.AppendNumFixedWidth(iSuiteId, EDecimal, 8);
    aDescription.Append(_L8("\nName: "));
    // Midlet name len is not limited. Make sure that buffer does not overflow.
    aDescription.Append((*iMidletName).Left(256));
    aDescription.Append(_L8("\nGroup: "));
    // Group name is max 32 characters
    aDescription.Append(*iGroupName);
    aDescription.Append(_L8("\nDrive: "));
    aDescription.AppendNumFixedWidth(iDrive, EDecimal, 2);
}

/* ---------------------------------------------------------------------------- */




CMidletList::CMidletList() :
        iMidletArray(RPointerArray<CMidletInfo>()), iGetNextIndex(0),
        iMidletArrayDestroyed(EFalse)
{

}

CMidletList::~CMidletList()
{
    // Free the memory used by the RPointerArray.
    // Do not destroy objects itself.
    if (!iMidletArrayDestroyed)
    {
        iMidletArray.Close();
    }
}

void CMidletList::ResetAndDestroy()
{
    iMidletArray.ResetAndDestroy();
    iMidletArrayDestroyed = ETrue;
}

TInt CMidletList::Append(const CMidletInfo* aMidletInfo)
{
    if (NULL != Find(aMidletInfo->GetMidletUid()))
    {
        // midlet is already in list, do nothing
        return KErrNone;
    }

    return iMidletArray.Append(aMidletInfo);
}

void CMidletList::Remove(const CMidletInfo* aMidletInfo)
{
    TInt nInd = iMidletArray.Find(aMidletInfo);
    if (nInd < 0)
    {
        // The object is not in list, do nothing
        return;
    }
    else if (nInd < iGetNextIndex)
    {
        // Removing one of the objects already returned by
        // getNext(), the indexes of the objects not yet returned by
        // getNext() are decreased by one. Must decrease iGetNextIndex too.
        iGetNextIndex--;
    }

    iMidletArray.Remove(nInd);
}

TInt CMidletList::Count() const
{
    return iMidletArray.Count();
}

CMidletInfo* CMidletList::GetFirst()
{
    iGetNextIndex = 1;

    if (iMidletArray.Count() > 0)
    {
        return iMidletArray[0];
    }
    else
    {
        return NULL;
    }
}

CMidletInfo* CMidletList::GetNext()
{
    if (iMidletArray.Count() > iGetNextIndex)
    {
        iGetNextIndex++;
        return iMidletArray[iGetNextIndex-1];
    }
    else
    {
        return NULL;
    }
}

CMidletInfo *CMidletList::Find(const TUid aMidletUid)
{
    TInt nCount = iMidletArray.Count();

    for (TInt nInd = 0; nInd < nCount; nInd++)
    {
        if (iMidletArray[nInd]->GetMidletUid() == aMidletUid)
        {
            return iMidletArray[nInd];
        }
    }

    return NULL;
}

/**
 * Return the first midlet that has name specified by aName and
 * for which the midlet id and suite id have not yet been set.
 * If midlet id and suite id have been set for all midlets with
 * name specified by aName, returns the first midlet with name
 * specified by aName
 */
CMidletInfo *CMidletList::Find(const TPtrC &aName)
{
    CMidletInfo *firstFound = NULL;
    TInt nCount = iMidletArray.Count();

    for (TInt nInd = 0; nInd < nCount; nInd++)
    {
        if (iMidletArray[nInd]->GetMidletName() == aName)
        {
            // If midlet id and suite id have already been set,
            // find next midlet with the same name if any.
            // If no such midlet, return this one.
            if (NULL == firstFound)
            {
                firstFound = iMidletArray[nInd];
            }
            if ((iMidletArray[nInd]->GetSuiteId() == KMaxTUint32) &&
                    (iMidletArray[nInd]->GetMidletId() == KMaxTUint32))
            {
                return iMidletArray[nInd];
            }
        }
    }

    return firstFound;
}

void CMidletList::SetIds(
    const TUint32 aUid,
    const TUint32 aMidletSuiteId,
    const TUint32 aMidletId)
{
    TUid uid;
    uid.iUid = aUid;
    CMidletInfo *midlet = Find(uid);

    if (NULL != midlet)
    {
        midlet->SetSuiteId(aMidletSuiteId);
        midlet->SetMidletId(aMidletId);
    }
}

void CMidletList::SetIds(
    const TPtrC &aName,
    const TUint32 aMidletSuiteId,
    const TUint32 aMidletId)
{
    CMidletInfo *midlet = Find(aName);

    if (NULL != midlet)
    {
        midlet->SetSuiteId(aMidletSuiteId);
        midlet->SetMidletId(aMidletId);
    }
}

void CMidletList::ExportListL(RFs& aFs, const TFileName &aDirectory)
{
    if (iMidletArrayDestroyed || (iMidletArray.Count() == 0))
    {
        // Trying to export empty list is an error
        User::Leave(KErrArgument);
    }

    // Create directory if it does not exist
    TInt err= aFs.MkDirAll(aDirectory);
    if ((KErrNone != err) && (KErrAlreadyExists != err))
    {
        User::Leave(err);
    }

    // Copy all icon files to directory
    CFileMan    *fm = CFileMan::NewL(aFs);
    CleanupStack::PushL(fm);
    CMidletInfo *midlet = GetFirst();
    TFileName    newIconFileName;
    TParse       nameParser;
    TUint32      nMidlets = 0;

    while (NULL != midlet)
    {
        User::LeaveIfError(fm->Copy(midlet->GetIconFileName(), aDirectory));

        // set new icon file name
        newIconFileName = aDirectory;
        User::LeaveIfError(nameParser.Set(midlet->GetIconFileName(), NULL, NULL));
        newIconFileName.Append(nameParser.NameAndExt());
        midlet->SetIconFileName(newIconFileName);

        nMidlets++;
        midlet = GetNext();
    }

    // Store all midlet info to a file in the directory
    TFileName dataFilename = aDirectory;
    dataFilename.Append(KMidletExportDataFileName);

    // Construct write stream so that new data file is created.
    // If the file already exists, leave
    RFileWriteStream writeStream;
    User::LeaveIfError(writeStream.Create(aFs, dataFilename, EFileWrite));
    CleanupClosePushL(writeStream);

    // Write the number of midlets
    writeStream.WriteUint32L(nMidlets);

    // write the midlet info one by one
    midlet = GetFirst();
    while (NULL != midlet)
    {
        midlet->ExternalizeL(writeStream);
        midlet = GetNext();
    }

    CleanupStack::PopAndDestroy();  // writeStream
    CleanupStack::PopAndDestroy(fm);
}

void CMidletList::ImportListL(RFs& aFs, const TFileName &aDirectory)
{
    CMidletInfo *midlet = NULL;

    // Form the data file name
    TFileName dataFilename = aDirectory;
    dataFilename.Append(KMidletExportDataFileName);

    // Open existing data file. If the file does not exist, leave
    RFileReadStream readStream;
    User::LeaveIfError(readStream.Open(aFs, dataFilename, EFileRead));
    CleanupClosePushL(readStream);

    // Read the number of midlets
    TUint32 nMidlets = readStream.ReadUint32L();

    for (TInt nInd = 0; nInd < nMidlets; nInd++)
    {
        // Read info of one midlet
        midlet = new(ELeave) CMidletInfo;
        CleanupStack::PushL(midlet);
        midlet->InternalizeL(readStream);

        // Add it to list
        Append(midlet);
        CleanupStack::Pop(midlet);
    }

    // close readStream
    CleanupStack::PopAndDestroy();
}

