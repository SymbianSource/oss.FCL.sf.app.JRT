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
* Description:  Scans for matching jad & jar in Inbox
*
*/


#ifndef JADJARMATCHERSCANFROMINBOX_H
#define JADJARMATCHERSCANFROMINBOX_H

#include <e32base.h>
#include <msvapi.h>

namespace java
{

/**
*  Scanner class. Searches matching Jad or Jar files in Inbox.
*/
NONSHARABLE_CLASS(CJadJarMatcherScanFromInbox) : public CBase, public MMsvSessionObserver
{
public:

    /**
     * Construct new CJadJarMatcherScanJarFilesFromInbox.
     */
    CJadJarMatcherScanFromInbox();

    /**
     * Destruct CJadJarMatcherScanJarFilesFromInbox.
     */
    virtual ~CJadJarMatcherScanFromInbox();

    /**
     * Execute Inbox scan.
     *
     * @param aStartFilename - Jad or Jar file from which the scan is started.
     * @param aResultArray - Files found from Inbox.
     * @param aJarScan - a scanning flag (Jad/Jar = EFalse/ETrue).
     * @return None.
     */
    virtual void ScanFromInboxL(HBufC16* aStartFilename,
                                RArray<RFile>& aResultArray,
                                TBool aJarScan);

    // From MMsvSessionObserver
    virtual void HandleSessionEventL(TMsvSessionEvent aEvent,
                                     TAny* /* aArg1 */,
                                     TAny* /* aArg2 */,
                                     TAny* /* aArg3 */);

    /**
     * Check if an attachment is a possible Jad file.
     *
     * Definition. Operation returns false if at least one of the following
     * statements is true:
     *
     * 1) The type of the attachment is neither 'a file' nor 'a linked file'.
     * 2) The file extension is not '.jad'.
     * 3) The MIME type is defined and it is not MIME for Jad file.
     *
     * Otherwise operation returns true.
     *
     * @param aAttaInfo - an attachment to be checked.
     * @return False if file is not Jad file.
     */
    TBool IsAttachmentPossibleJadFileL(const CMsvAttachment& aAttaInfo);

    /**
     * Check if an attachment is possible Jar file.
     *
     * Definition. Operation returns false if at least one of following
     * statements is true:
     *
     * 1) The type of the attachment is neither 'a file' nor 'a linked file'.
     * 2) The file extension is not '.jar'.
     * 3) The MIME type is defined and it is not MIME for Jar file.
     *
     * Otherwise operation returns true.
     *
     * @param aAttaInfo - an attachment to be checked.
     * @return False if file is not Jar file.
     */
    TBool IsAttachmentPossibleJarFileL(const CMsvAttachment& aAttaInfo);

private:

    /**
     * Seek root entries from Inbox.
     */
    void SeekRootEntriesL(RArray<TMsvId>& aRootEntryArray);

    /**
     * Seek entries from Inbox.
     */
    void SeekEntriesL(TMsvId aId, RArray<TMsvId>& aEntryArray);

    /**
     * Open attachment manager
     */
    TInt OpenAttachmentManagerL(TMsvId& aId);

    /**
     * Do clean-up for next scan round.
     */
    void Cleanup();

    /**
     * Delete scan objects owned by this object.
     */
    void InitAttachmentScan();

private:
    CMsvSession* iSession;
    CMsvStore* iStore;
    CMsvEntry* iEntry;
    MMsvAttachmentManager* iAttaMan;
};

} // namespace java

#endif // JADJARMATCHERSCANFROMINBOX_H
