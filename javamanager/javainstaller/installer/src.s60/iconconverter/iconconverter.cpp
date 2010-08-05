/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java Installer component iconconverter.
*                Reads icon and .jar file and stores it a temp file in a
*                format that can be shown by the S60 UI shell.
*
*/


#include <zipfile.h>
#include <time.h>
#include <bitmaptransforms.h>

#include "javacommonutils.h"
#include "logger.h"
#include "mifconverter.h"
#include "iconconverter.h"
#include "iconsizeutils.h"      // TIconSizes 
#include "iconsizenotifier.h"   // KUidIconSizeNotifPlugin

namespace java
{

/**
 * The icon in S60 temporary drive
 */
_LIT(KTempIconName1, "D:\\micon1.mbm");
_LIT(KTempMaskName1, "D:\\mmask1.mbm");

#ifdef RD_JAVA_S60_RELEASE_9_2
_LIT(KTempIconName2, "D:\\micon2.mbm");
_LIT(KTempMaskName2, "D:\\mmask2.mbm");
#endif // RD_JAVA_S60_RELEASE_9_2


// MIF file constants
const TInt KMifFileHeaderUid = 0x34232342;
const TInt KMifFileHeaderVersion = 2;
const TInt KMifFileHeaderLength = 2;

const TInt KMifIconHeaderUid = 0x34232343;
const TInt KMifIconHeaderVersion = 1;
const TInt KMifIconHeaderType = 1;
const TInt KMifIconHeaderAnimated = 0;


CIconConverter* CIconConverter::NewL(RFs& aRFs)
{
    return new(ELeave) CIconConverter(aRFs);
}

CIconConverter::CIconConverter(RFs &aRFs)
{
    iRFs = aRFs;
    iImageDecoder = NULL;

    iBitmap = NULL;
    iBitmapMask = NULL;

    // In case there is no UI on the device ( as when installing an app or
    // MIDlet suite from PC ) make sure there is an instance of FBS running
    RFbsSession::Connect();

    iActiveListener = NULL;
}


CIconConverter::~CIconConverter()
{
    // Do NOT close file server session
    delete iBitmapMask;
    delete iBitmap;
    delete iBitmapMaskScaledCopy;
    delete iBitmapScaledCopy;
    delete iImageDecoder;

    RFbsSession::Disconnect();

    delete iActiveListener;
}


int CIconConverter::Convert(
    const TDesC &aJarFile,
    const TDesC &aIconFile,
    const TDesC &aOutputFile,
    TBool *apWasMbm)
{
    int ret = 0;

    CActiveScheduler* pScheduler = NULL;

    if (NULL == CActiveScheduler::Current())
    {
        // Must create active scheduler for this JNI thread
        // to be able to make asynchronous calls
        pScheduler = new CActiveScheduler();
        CActiveScheduler::Install(pScheduler);
    }

    TRAPD(err, ret = ConvertL(aJarFile, aIconFile, aOutputFile, apWasMbm));

    delete pScheduler;

    if (KErrNone != err)
    {
        return err;
    }

    return ret;
}

int CIconConverter::ConvertL(
    const TDesC &aJarFile,
    const TDesC &aIconFile,
    const TDesC &aOutputFile,
    TBool *apWasMbm)
{
    // open jar file
    CZipFile *pZipFile = CZipFile::NewL(iRFs, aJarFile);
    CleanupStack::PushL(pZipFile);

    // try to open the icon inside the jar file
    CZipFileMember *pZipMember = NULL;
    // Does the icon name start with '/'
    if (aIconFile.Locate('/') == 0)
    {
        // remove the leading '/' character
        TFileName tempName = aIconFile.Right(aIconFile.Length() - 1);
        pZipMember = pZipFile->MemberL(tempName);
    }
    else
    {
        pZipMember = pZipFile->MemberL(aIconFile);
    }

    if (NULL == pZipMember)
    {
        LOG(EJavaInstaller, EInfo, "Cannot find icon inside .jar file");
        User::Leave(KErrNotFound);
    }
    CleanupStack::PushL(pZipMember);

    // reserve buffer for uncompressed icon
    TUint32 iconSize = pZipMember->UncompressedSize();
    HBufC8* pIconBuf = HBufC8::NewL(iconSize);
    CleanupStack::PushL(pIconBuf);
    TPtr8 ptrIconBuf(NULL, 0);
    ptrIconBuf.Set(pIconBuf->Des());

    RZipFileMemberReaderStream *pReader = NULL;
    User::LeaveIfError(pZipFile->GetInputStreamL(pZipMember, pReader));
    CleanupStack::PushL(pReader);

    // read icon to buffer
    User::LeaveIfError(pReader->Read(ptrIconBuf, iconSize));
    CleanupStack::PopAndDestroy(pReader);

    // free unnecessary objects
    CleanupStack::Pop(pIconBuf); // this is still needed
    CleanupStack::PopAndDestroy(pZipMember);
    CleanupStack::PopAndDestroy(pZipFile);

    CleanupStack::PushL(pIconBuf);

    // Icon buffer contains SVG icon if the content of the buffer
    // starts with _L8("<?xml")
    if ((iconSize > 5) &&
            (ptrIconBuf[0] == 0x3c) &&
            (ptrIconBuf[1] == 0x3f) &&
            (ptrIconBuf[2] == 0x78) &&
            (ptrIconBuf[3] == 0x6d) &&
            (ptrIconBuf[4] == 0x6c))
    {
        *apWasMbm = EFalse;
        return ConvertScalableIconL(pIconBuf, aOutputFile);
    }
    else
    {
        *apWasMbm = ETrue;
        return ConvertNormalIconL(pIconBuf, ptrIconBuf, aOutputFile);
    }
}

int CIconConverter::ConvertNormalIconL(
    HBufC8* apIconBuf,
    TPtr8& aptrIconBuf,
    const TDesC &aOutputFile)
{
    // convert icon in buffer to bitmap
    if (NULL == iImageDecoder)
    {
        // create image decoder if not yet created
        iImageDecoder = CBufferedImageDecoder::NewL(iRFs);
    }

    // Try to create a decoder implementation for the image in the buffer
    TRAPD(err, iImageDecoder->OpenL(aptrIconBuf));
    if (KErrNotFound == err)
    {
        // If the format is unrecognised AND the first 2 bytes are 0 then
        // we assume it is a WBMP.
        // The first two bytes of other formats (e.g. ico) may also begin with 0, 0
        // but they should have already been recognised by the first call to OpenL.
        if (((*apIconBuf)[0] == 0) && ((*apIconBuf)[1] == 0))
        {
            delete iImageDecoder;
            iImageDecoder = NULL;
            iImageDecoder = CBufferedImageDecoder::NewL(iRFs);
            WLOG(EJavaInstaller, "CIconConverter::ConvertNormalIconL - Format "
                 "can't be fully identified, assuming this is WBMP");
            _LIT8(KWBMPMimeType, "image/vnd.wap.wbmp");
            iImageDecoder->OpenL(aptrIconBuf, KWBMPMimeType);
            err = KErrNone;
        }
    }
    // Leave if we could not recover from error
    User::LeaveIfError(err);

    // get info concerning the icon
    TFrameInfo frameInfo = iImageDecoder->FrameInfo(0);

    // create bitmap for icon
    iBitmap = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmap->Create(frameInfo.iOverallSizeInPixels, EColor16M));

    // create bitmap for mask
    iBitmapMask = new(ELeave) CFbsBitmap;
    User::LeaveIfError(iBitmapMask->Create(frameInfo.iOverallSizeInPixels, EGray256));

    // Convert icon to bitmap and possible mask.
    // Wait until the asynch conversion has been done.
    iActiveListener = new(ELeave) CActiveListener();
    iActiveListener->InitialiseActiveListener();

    // Note that this does not generate mask if the image does not contain it
    iImageDecoder->Convert(&(iActiveListener->iStatus),
                           *iBitmap,
                           *iBitmapMask);

    CActiveScheduler::Start();
    User::LeaveIfError(iActiveListener->iStatus.Int());

    // Scale icons
    TIconSizes sizes(GetIdealIconSizes());
    CBitmapScaler* scaler = CBitmapScaler::NewL();
    CleanupStack::PushL(scaler);
    scaler->SetQualityAlgorithm(CBitmapScaler::EMaximumQuality);

    // Scale for Menu
    ILOG2(EJavaInstaller, "Scaling Menu icon to (%d, %d)", 
        sizes.iMenuIconSize.iWidth, sizes.iMenuIconSize.iHeight);
    iBitmapScaledCopy = new(ELeave) CFbsBitmap;
    iBitmapMaskScaledCopy = new(ELeave) CFbsBitmap;
    ScaleL(*scaler, sizes.iMenuIconSize);
    User::LeaveIfError(iBitmapScaledCopy->Save(KTempIconName1));
    User::LeaveIfError(iBitmapMaskScaledCopy->Save(KTempMaskName1));
    iBitmapScaledCopy->Reset();
    iBitmapMaskScaledCopy->Reset();

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Scale for App Manager
    ILOG2(EJavaInstaller, "Scaling App Mgr icon to (%d, %d)", 
        sizes.iMenuIconSize.iWidth, sizes.iMenuIconSize.iHeight);
    ScaleL(*scaler, sizes.iAppMgrIconSize);
    User::LeaveIfError(iBitmapScaledCopy->Save(KTempIconName2));
    User::LeaveIfError(iBitmapMaskScaledCopy->Save(KTempMaskName2));
    iBitmapScaledCopy->Reset();
    iBitmapMaskScaledCopy->Reset();
#endif // RD_JAVA_S60_RELEASE_9_2

    CleanupStack::PopAndDestroy(scaler);
    delete iActiveListener;
    iActiveListener = NULL;

    // Construct multi bitmap file from bitmap and mask files (2 files)
#ifdef RD_JAVA_S60_RELEASE_9_2
    const TInt KBmpCount = 4;
    TInt32 sourceIds[] = {0, 0, 0, 0};
#else
    const TInt KBmpCount = 2;
    TInt32 sourceIds[] = {0, 0};
#endif // RD_JAVA_S60_RELEASE_9_2
    
    TFileName** filenames = new(ELeave) TFileName*[KBmpCount];
    CleanupStack::PushL(filenames);
    filenames[0] = new(ELeave) TFileName(KTempIconName1);
    CleanupStack::PushL(filenames[0]);
    filenames[1] = new(ELeave) TFileName(KTempMaskName1);
    CleanupStack::PushL(filenames[1]);
    
#ifdef RD_JAVA_S60_RELEASE_9_2
    filenames[2] = new(ELeave) TFileName(KTempIconName2);
    CleanupStack::PushL(filenames[2]);
    filenames[3] = new(ELeave) TFileName(KTempMaskName2);
    CleanupStack::PushL(filenames[3]);
#endif // RD_JAVA_S60_RELEASE_9_2
    
    CFbsBitmap::StoreL(aOutputFile, KBmpCount, (const TDesC**)filenames, sourceIds);

    // Now try to delete the temp icon and mask files,
    // ignore possible errors
    (void)iRFs.Delete(KTempIconName1);
    (void)iRFs.Delete(KTempMaskName1);
    
#ifdef RD_JAVA_S60_RELEASE_9_2
    (void)iRFs.Delete(KTempIconName2);
    (void)iRFs.Delete(KTempMaskName2);
    CleanupStack::PopAndDestroy(filenames[3]);
    CleanupStack::PopAndDestroy(filenames[2]);
#endif // RD_JAVA_S60_RELEASE_9_2
    
    CleanupStack::PopAndDestroy(filenames[1]);
    CleanupStack::PopAndDestroy(filenames[0]);
    CleanupStack::PopAndDestroy(filenames);
    CleanupStack::PopAndDestroy(apIconBuf);

    return KErrNone;
}

int CIconConverter::ConvertScalableIconL(
    HBufC8* apIconBuf,
    const TDesC &aOutputFile)
{
    TInt iconDataSize = apIconBuf->Length();

    // File header
    TMifFileHeader fileHeader;
    fileHeader.iUid = KMifFileHeaderUid;
    fileHeader.iVersion = KMifFileHeaderVersion;
    fileHeader.iOffset = sizeof(fileHeader);
    fileHeader.iLength = KMifFileHeaderLength;  // number of indexes

    // Icon offset element
    TMifIconOffset iconOffset;
    iconOffset.iIconOffset =
        sizeof(fileHeader) +
        sizeof(iconOffset) * KMifFileHeaderLength; // mif header + icon offset
    iconOffset.iIconLength =
        sizeof(TMifIconHeader) + iconDataSize; //icon header + icon data

    // Icon header
    TMifIconHeader iconHeader;
    iconHeader.iUid = KMifIconHeaderUid;
    iconHeader.iVersion = KMifIconHeaderVersion;
    iconHeader.iOffset = sizeof(iconHeader);  // dataOffset
    iconHeader.iLength = iconDataSize;  // dataLength
    iconHeader.iType = KMifIconHeaderType;  // svg
    iconHeader.iDepth = EColor16M;
    iconHeader.iAnimated = KMifIconHeaderAnimated;
    iconHeader.iMaskDepth = EColor16M;

    // Create MIFConverter class
    CMifConverter* mifConverter = CMifConverter::NewL(iRFs, aOutputFile);
    CleanupStack::PushL(mifConverter);

    // write mif file header
    mifConverter->WriteMifFileHeaderL(fileHeader);
    // insert 2 iconOffset elements: first for the image, the other for the mask
    mifConverter->WriteMifIconOffsetL(iconOffset);
    mifConverter->WriteMifIconOffsetL(iconOffset);
    mifConverter->WriteMifIconHeaderL(iconHeader);

    // write mif file body
    mifConverter->WriteMifBodyL(apIconBuf);

    // cleanup
    CleanupStack::PopAndDestroy(mifConverter);
    CleanupStack::PopAndDestroy(apIconBuf);

    return KErrNone;
}

void CIconConverter:: LogAllSupportedMimeTypes()
{
    // List the file extensions that can be decoded and their corresponding MIME types

    RFileExtensionMIMETypeArray mimeTypes;
    CImageDecoder::GetFileTypesL(mimeTypes);
    TInt nTypes = mimeTypes.Count();

    for (TInt nInd = 0; nInd < nTypes; nInd++)
    {
        const TDesC& extension = mimeTypes[nInd]->FileExtension();
        // Reserve one char for null terminator
        HBufC* extBuf = HBufC::NewLC(extension.Length() + 1);
        TPtr extPtr(extBuf->Des());
        extPtr.Append(extension);
        LOG1WSTR(EJavaInstaller, EInfo,
                 "Icon converter supports file extension %s",
                 (wchar_t *)(extPtr.PtrZ()));
        CleanupStack::Pop(extBuf);

        const TDesC8& mime = mimeTypes[nInd]->MIMEType();
        HBufC8* mimeBuf = HBufC8::NewLC(mime.Length() + 1);
        TPtr8 mimePtr(mimeBuf->Des());
        mimePtr.Append(mime);
        LOG1(
            EJavaInstaller,
            EInfo,
            "Icon converter supports MIME type %s",
            mimePtr.PtrZ());

        CleanupStack::Pop(mimeBuf);
    }

    mimeTypes.ResetAndDestroy();
}

void CIconConverter::ScaleL(CBitmapScaler& aScaler, const TSize aSize)
{
    ASSERT(iBitmapScaledCopy);
    ASSERT(iBitmapMaskScaledCopy);

    User::LeaveIfError(iBitmapScaledCopy->Create(aSize, EColor16M));
    User::LeaveIfError(iBitmapMaskScaledCopy->Create(aSize, EGray256));

    iActiveListener->InitialiseActiveListener();
    aScaler.Scale(&iActiveListener->iStatus, *iBitmap, *iBitmapScaledCopy, ETrue);
    CActiveScheduler::Start();
    User::LeaveIfError(iActiveListener->iStatus.Int());

    iActiveListener->InitialiseActiveListener();
    aScaler.Scale(&iActiveListener->iStatus, *iBitmapMask, *iBitmapMaskScaledCopy, ETrue);
    CActiveScheduler::Start();
    User::LeaveIfError(iActiveListener->iStatus.Int());
}

TIconSizes CIconConverter::GetIdealIconSizes()
{
    TIconSizes fallbackSizes;
    fallbackSizes.iMenuIconSize = TSize(KIconInMenuFallbackSize, KIconInMenuFallbackSize);
    fallbackSizes.iAppMgrIconSize = TSize(KIconInAppMgrFallbackSize, KIconInAppMgrFallbackSize);
    
#ifdef RD_JAVA_S60_RELEASE_9_2
    RNotifier notifier;
    TInt err = notifier.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller, "CIconConverter::GetIdealIconSizes #1 err = %d", err);
        return fallbackSizes;
    }
    
    CleanupClosePushL(notifier);

    TPckgBuf<TIconSizes> des;
    iActiveListener->InitialiseActiveListener();
    notifier.StartNotifierAndGetResponse(iActiveListener->iStatus,
                                         KUidIconSizeNotifPlugin, des, des);
    CActiveScheduler::Start();
    notifier.CancelNotifier(KUidIconSizeNotifPlugin);
    err = iActiveListener->iStatus.Int();
    
    CleanupStack::PopAndDestroy(&notifier);
    
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller, "CIconConverter::GetIdealIconSizes #2 err = %d", err);
        return fallbackSizes;
    }
    else
    {
        return des();
    }
#else
    return fallbackSizes;
#endif
}

} // namespace java
