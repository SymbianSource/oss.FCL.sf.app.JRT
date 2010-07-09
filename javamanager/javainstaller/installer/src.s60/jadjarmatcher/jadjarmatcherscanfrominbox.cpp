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
* Description:  This class is used to find matching JAR file in
/                the Inbox. This class is derived from CActive.
*
*/


#include <cmsvattachment.h>
#include <cmsvmimeheaders.h>
#include <e32std.h>
#include <miutset.h>
#include <mmsvattachmentmanager.h>
#include <msvids.h>

#include "jadjarmatcherscanfrominbox.h"
#include "javacommonutils.h"
#include "logger.h"

namespace java
{

_LIT(KJadExt, ".jad");
_LIT(KJarExt, ".jar");
_LIT(KDcfExt, ".dcf");
_LIT(KDmExt, ".dm");
_LIT8(KJadMimeType, "text");
_LIT8(KJadMimeSubType, "vnd.sun.j2me.app-descriptor");
_LIT8(KJarMimeType, "application");
_LIT8(KJarMimeSubType1, "java-archive");
_LIT8(KJarMimeSubType2, "java");
_LIT8(KJarMimeSubType3, "x-java-archive");
const TInt KExtLength = 4;


// ---------------------------------------------------------------------------
//    To construct new CJadJarMatcherScanJarFilesFromInbox.
// -----------------------------------------------------------------------------
CJadJarMatcherScanFromInbox::CJadJarMatcherScanFromInbox()
{
}


// ---------------------------------------------------------------------------
//    To destruct CJadJarMatcherScanJarFilesFromInbox. Cancel
//    any outstanding request. Delete all C-objects owned by this object.
// -----------------------------------------------------------------------------
CJadJarMatcherScanFromInbox::~CJadJarMatcherScanFromInbox()
{
    Cleanup();
}


// ---------------------------------------------------------------------------
//  To execute Inbox scan. This method starts long running task,
//  which seeks Jad/Jar files from Inbox.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::ScanFromInboxL(
    HBufC16* aStartFilename, RArray<RFile>& aResultArray, TBool aJarScan)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::ScanFromInboxL()");
    Cleanup();
    if (aJarScan)
    {
        LOG1WSTR(EJavaInstaller, EInfo, "Inbox Jar scan started for %s",
                 (wchar_t *)(aStartFilename->Des().PtrZ()));
    }
    else
    {
        LOG1WSTR(EJavaInstaller, EInfo, "Inbox Jad scan started for %s",
                 (wchar_t *)(aStartFilename->Des().PtrZ()));
    }

    // Setup ActiveScheduler, CMsvSession needs it.
    CActiveScheduler* scheduler = NULL;
    if (NULL == CActiveScheduler::Current())
    {
        // Must create active scheduler for this JNI thread
        scheduler = new(ELeave) CActiveScheduler();
        CActiveScheduler::Install(scheduler);
    }
    CleanupStack::PushL(scheduler);

    // Open CMsvSession and search through Inbox.
    iSession = CMsvSession::OpenSyncL(*this);
    LOG(EJavaInstaller, EInfo, "CMsvSession opened");
    RArray<TMsvId> rootEntryArray;
    CleanupClosePushL(rootEntryArray);
    SeekRootEntriesL(rootEntryArray);
    LOG1(EJavaInstaller, EInfo, "Number of root entries: %d", rootEntryArray.Count());
    RArray<TMsvId> entryArray;
    CleanupClosePushL(entryArray);
    for (TInt rootIndex = 0; rootIndex < rootEntryArray.Count(); rootIndex++)
    {
        SeekEntriesL(rootEntryArray[rootIndex], entryArray);
        LOG1(EJavaInstaller, EInfo, "Number of entries: %d", entryArray.Count());
    }
    InitAttachmentScan();
    RFile startAtta; // RFile for aStartFilename
    TBool startAttaFound = EFalse;
    for (TInt entryIndex = 0; entryIndex < entryArray.Count(); entryIndex++)
    {
        TInt attaCount = 0;
        TRAPD(err, attaCount = OpenAttachmentManagerL(entryArray[entryIndex]));
        if (KErrNone == err)
        {
            LOG1(EJavaInstaller, EInfo, "Number of attachments: %d", attaCount);
            for (TInt attaIndex = 0; attaIndex < attaCount; attaIndex++)
            {
                CMsvAttachment* attaInfo = NULL;
                TRAP(err, attaInfo = iAttaMan->GetAttachmentInfoL(attaIndex));
                if (KErrNone == err && !startAttaFound)
                {
                    if (attaInfo->FilePath().CompareF(aStartFilename->Des()) == 0)
                    {
                        TRAP(err, startAtta =
                                 iAttaMan->GetAttachmentFileL(attaInfo->Id()));
                        if (KErrNone == err)
                        {
                            startAttaFound = ETrue;
                        }
                    }
                }
                if (KErrNone == err)
                {
                    if (aJarScan)
                    {
                        if (CMsvAttachment::EMsvMessageEntry != attaInfo->Type() &&
                                IsAttachmentPossibleJarFileL(*attaInfo))
                        {
                            RFile atta;
                            TRAP(err, atta = iAttaMan->GetAttachmentFileL(attaInfo->Id()));
                            if (KErrNone == err)
                            {
                                aResultArray.InsertL(atta, 0);
                            }
                        }
                    }
                    else
                    {
                        if (CMsvAttachment::EMsvMessageEntry != attaInfo->Type() &&
                                IsAttachmentPossibleJadFileL(*attaInfo))
                        {
                            RFile atta;
                            TRAP(err, atta = iAttaMan->GetAttachmentFileL(attaInfo->Id()));
                            if (KErrNone == err)
                            {
                                aResultArray.Insert(atta, 0);
                            }
                        }
                    }
                    delete attaInfo;
                }
            }
        }
    }

    if (startAttaFound)
    {
        LOG(EJavaInstaller, EInfo, "Found handle for startFilename");
        aResultArray.Insert(startAtta, 0);
    }
    else
    {
        ELOG(EJavaInstaller, "Inbox scan did not find startFilename");
        User::Leave(KErrPathNotFound);
    }

    CleanupStack::PopAndDestroy(&entryArray);
    CleanupStack::PopAndDestroy(&rootEntryArray);
    Cleanup();
    LOG(EJavaInstaller, EInfo, "End of Inbox scan");

    CleanupStack::PopAndDestroy(scheduler);
}


// ---------------------------------------------------------------------------
//    To seek root entries.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::SeekRootEntriesL(RArray<TMsvId>& aRootEntryArray)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::SeekRootEntriesL()");

    CMsvEntry* entry = iSession->GetEntryL(KMsvRootIndexEntryIdValue);
    CleanupStack::PushL(entry);

    const TInt count = entry->Count();

    // Check mail-boxes.
    for (TInt loop = 0; loop < count; loop++)
    {
        if ((*entry)[loop].iType.iUid == KUidMsvServiceEntryValue)
        {
            TMsvEntry index = (*entry)[loop];
            if (KUidMsgTypePOP3 == index.iMtm || KUidMsgTypeIMAP4 == index.iMtm)
            {
                aRootEntryArray.InsertL((*entry)[loop].Id(), 0);
            }
        }
    }

    CleanupStack::PopAndDestroy(entry);

    aRootEntryArray.Insert(KMsvGlobalInBoxIndexEntryId, 0);
}


// ---------------------------------------------------------------------------
//    To seek entries recursively.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::SeekEntriesL(TMsvId aId, RArray<TMsvId>& aEntryArray)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::SeekEntriesL()");
    LOG1(EJavaInstaller, EInfo, "Entry Id: %d", aId);

    CMsvEntry* entry = iSession->GetEntryL(aId);
    CleanupStack::PushL(entry);

    const TInt count = entry->Count();

    // If current entry fulfils all defined constrains it is added into
    // the array.
    {
        TBool store = EFalse;
        TRAPD(err, store = entry->HasStoreL());
        if (KErrNone == err && store)
        {
            TMsvEntry index = entry->Entry();

            if (index.iType.iUid == KUidMsvMessageEntryValue ||
                    index.iType.iUid == KUidMsvAttachmentEntryValue)
            {
                if (index.Visible() && index.Complete() &&
                        !index.InPreparation() && !index.Deleted())
                {
                    aEntryArray.InsertL(aId, 0);
                }
            }
        }
    }

    // Check all child entries.
    for (TInt loop = 0; loop < count; loop++)
    {
        if ((*entry)[loop].iType.iUid == KUidMsvFolderEntryValue ||
                (*entry)[loop].iType.iUid == KUidMsvMessageEntryValue ||
                (*entry)[loop].iType.iUid == KUidMsvAttachmentEntryValue)
        {
            SeekEntriesL((*entry)[loop].Id(), aEntryArray);
        }
    }

    CleanupStack::PopAndDestroy(entry);
}


// ---------------------------------------------------------------------------
//    To open an attachment manager.
// -----------------------------------------------------------------------------
TInt CJadJarMatcherScanFromInbox::OpenAttachmentManagerL(TMsvId& aId)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::OpenAttachmentManagerL");
    iEntry = iSession->GetEntryL(aId);
    iStore = iEntry->ReadStoreL();
    iAttaMan = &iStore->AttachmentManagerL();
    return iAttaMan->AttachmentCount();
}


// ---------------------------------------------------------------------------
//    To indicate that a message server event has occured. This
//    method handles events coming from the messaging server. If
//    EMsvServerReady has come, session is established and MTM client can be
//    instantiated. Otherwise the scanning is stopped with general error code.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::HandleSessionEventL(
    TMsvSessionEvent /*aEvent*/, TAny */*aArg1*/,
    TAny */*aArg2*/, TAny */*aArg3*/)
{
    //LOG1(EJavaInstaller, EInfo, "CJadJarMatcherScanFromInbox::HandleSessionEventL: %d", aEvent);
}


// ---------------------------------------------------------------------------
//    To check whether an attachment is possible JAD file or not.
//    Operation returns false if at least one of the following statements is
//    true:
//    1) The type of the attachment is neither 'a file' nor 'a linked
//    file'.
//    2) The file extension is not '.jad'.
//    3) The MIME type is defined and it is not MIME for JAD file.
//    Otherwise operation returns true.
//    Preconditions. The following conditions must be true prior to calling
//    this method;
//    otherwise, its operation is undefined. - None. Postconditions. The
//    following condition is true immediately after returning from this method.
//    - None.
//    @param aAttaInfo - an attachment to be checked.
//    @return False if file is not JAD file.
// -----------------------------------------------------------------------------
TBool CJadJarMatcherScanFromInbox::IsAttachmentPossibleJadFileL(const CMsvAttachment&  aAttaInfo)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::IsAttachmentPossibleJadFileL");

    // 1
    CMsvAttachment::TMsvAttachmentType type = aAttaInfo.Type();

    if (CMsvAttachment::EMsvFile != type &&
            CMsvAttachment::EMsvLinkedFile != type)
    {
        LOG(EJavaInstaller, EInfo,
            "IsAttachmentPossibleJadFileL: EFalse - an attachment is not a file");
        return EFalse;
    }

    // 2
    TParsePtrC name(aAttaInfo.FilePath());

    if (name.Ext().Length() > KExtLength)
    {
        LOG(EJavaInstaller, EInfo,
            "IsAttachmentPossibleJadFileL: EFalse - file extension is too long");
        return EFalse;
    }
    else
    {
        TBuf<KExtLength> ext(name.Ext());

        // Convert the content to lower case.
        ext.LowerCase();

        // Compare extensions.
        if (0 != ext.Compare(KJadExt))
        {
            LOG(EJavaInstaller, EInfo,
                "IsAttachmentPossibleJadFileL: EFalse - file extension is not .jad");
            return EFalse;
        }
    }

    // 3
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL(mimeHeaders);

    // Restore MIME headers.
    mimeHeaders->RestoreL(const_cast<CMsvAttachment&>(aAttaInfo));

    if (NULL != mimeHeaders)
    {
        HBufC8* mt = mimeHeaders->ContentType().AllocL();
        CleanupStack::PushL(mt);

        HBufC8* ms = mimeHeaders->ContentSubType().AllocL();
        CleanupStack::PushL(ms);

        // Check whether MIME is not for JAD file.
        if ((mt->Length() > 0 || ms->Length() > 0) && (
                    0 != mt->Compare(KJadMimeType) || 0 != ms->Compare(
                        KJadMimeSubType)))
        {
            LOG(EJavaInstaller, EInfo,
                "IsAttachmentPossibleJadFileL: EFalse - mime is not for jad");
            CleanupStack::PopAndDestroy(ms);
            CleanupStack::PopAndDestroy(mt);
            CleanupStack::PopAndDestroy(mimeHeaders);
            return EFalse;
        }
        CleanupStack::PopAndDestroy(ms);
        CleanupStack::PopAndDestroy(mt);
    }

    LOG(EJavaInstaller, EInfo,
        "IsAttachmentPossibleJadFileL: ETrue - file is likely jad file");
    CleanupStack::PopAndDestroy(mimeHeaders);
    return ETrue;
}


// ---------------------------------------------------------------------------
//    To check whether an attachment is possible JAR file or not.
//    Operation returns false if at least one of following statements is true:
//    1) The type of the attachment is neither 'a file' nor 'a linked file'.
//    2) The file extension is not '.jar'.
//    3) The MIME type is defined and it is not MIME for JAR file.
//    Otherwise operation returns true. Preconditions.
//    The following conditions must be true prior to calling this method,
//    otherwise, its operation is undefined. - None. Postconditions. The
//    following condition is true immediately after returning from this method.
//    - None.
//    @param aAttaInfo - an attachment to be checked.
//    @return False if file is not JAR file.
// -----------------------------------------------------------------------------
TBool CJadJarMatcherScanFromInbox::IsAttachmentPossibleJarFileL(
    const CMsvAttachment&  aAttaInfo)
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::IsAttachmentPossibleJarFileL()");

    // 1
    CMsvAttachment::TMsvAttachmentType type = aAttaInfo.Type();

    if (CMsvAttachment::EMsvFile != type &&
            CMsvAttachment::EMsvLinkedFile != type)
    {
        LOG(EJavaInstaller, EInfo,
            "IsAttachmentPossibleJarFileL: EFalse - an attachment is not a file");
        return EFalse;
    }

    // 2
    TParsePtrC name(aAttaInfo.FilePath());

    if (name.Ext().Length() > KExtLength)
    {
        LOG(EJavaInstaller, EInfo,
            "IsAttachmentPossibleJarFileL: EFalse - file extension is too long");
        return EFalse;
    }
    else
    {
        TBuf<KExtLength> ext(name.Ext());

        // Convert the content to lower case.
        ext.LowerCase();

        // Compare extensions.
        if (0 != ext.Compare(KJarExt) &&
                0 != ext.Compare(KDcfExt) &&
                0 != ext.Compare(KDmExt))
        {
            LOG(EJavaInstaller, EInfo,
                "IsAttachmentPossibleJarFileL: EFalse - file extension is not .jar, .dcf or .dm");
            return EFalse;
        }
    }

    // 3
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL(mimeHeaders);

    // Restore MIME headers.
    mimeHeaders->RestoreL(const_cast<CMsvAttachment&>(aAttaInfo));

    // Check whether MIME is not for JAR file.
    if (NULL != mimeHeaders)
    {
        HBufC8* mt = mimeHeaders->ContentType().AllocL();
        CleanupStack::PushL(mt);

        HBufC8* ms = mimeHeaders->ContentSubType().AllocL();
        CleanupStack::PushL(ms);
        if ((mt->Length() > 0 || ms->Length() > 0) &&
                (0 != mt->Compare(KJarMimeType) ||
                 (0 != ms->Compare(KJarMimeSubType1) &&
                  0 != ms->Compare(KJarMimeSubType2) &&
                  0 != ms->Compare(KJarMimeSubType3))))
        {
            LOG(EJavaInstaller, EInfo,
                "IsAttachmentPossibleJarFileL: EFalse - mime is not for jar");
            CleanupStack::PopAndDestroy(ms);
            CleanupStack::PopAndDestroy(mt);
            CleanupStack::PopAndDestroy(mimeHeaders);
            return EFalse;
        }
        CleanupStack::PopAndDestroy(ms);
        CleanupStack::PopAndDestroy(mt);
    }

    LOG(EJavaInstaller, EInfo,
        "IsAttachmentPossibleJarFileL: ETrue - file is likely jar file");
    CleanupStack::PopAndDestroy(mimeHeaders);
    return ETrue;
}


// ---------------------------------------------------------------------------
//    To do clean-up for next round.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::Cleanup()
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::Cleanup()");
    delete iStore;
    iStore = NULL;
    delete iEntry;
    iEntry = NULL;
    delete iSession;
    iSession = NULL;
}


// ---------------------------------------------------------------------------
//    To delete store objects.
// -----------------------------------------------------------------------------
void CJadJarMatcherScanFromInbox::InitAttachmentScan()
{
    JELOG(EJavaInstaller, "CJadJarMatcherScanFromInbox::InitAttachmentScan()");
    delete iStore;
    iStore = NULL;
    delete iEntry;
    iEntry = NULL;
}

} // namespace java
