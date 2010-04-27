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
* Description:  ?Description
 *
*/


#include "logger.h"
#include "s60commonutils.h"
#include "cmmsmessageattachment.h"

using namespace java::util;

namespace java
{
namespace wma
{
///////////////////////////////////////////////////////////////////////////////
// CMIDMessageAttachment class
///////////////////////////////////////////////////////////////////////////////
/* ------------------------------------------------------------------------
 * CMIDMessageAttachment(RFs& aRFs)
 *
 * ------------------------------------------------------------------------
 */
CMMSMessageAttachment::CMMSMessageAttachment(RFs& aRFs) : mRFs(aRFs)
{
}
/* ------------------------------------------------------------------------
 *
 *      const TDesC8& aContentId, const TDesC8& aLocation, TInt aValue)
 *
 * ------------------------------------------------------------------------
 */
CMMSMessageAttachment* CMMSMessageAttachment::NewL(RFs& aRFs,
        const TDesC8& aDataArray, const TDesC8& aAttachmentType,
        const TDesC8& aContentId, const TDesC8& aLocation,
        TInt aAttachmentNameOrData)
{
    JELOG2(EWMA);
    CMMSMessageAttachment* self = new(ELeave) CMMSMessageAttachment(aRFs);
    CleanupStack::PushL(self);
    self->ConstructL(aDataArray, aAttachmentType, aContentId, aLocation,
                     aAttachmentNameOrData);
    CleanupStack::Pop(self);
    return self;
}
/* ------------------------------------------------------------------------
 * ConstructL(const TDesC8& aDataArray, const TDesC8& aAttachmentType,
 *              const TDesC8& aContentId, const TDesC8& aLocation, TInt aValue)
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::ConstructL(const TDesC8& aDataArray,
                                       const TDesC8& aAttachmentType, const TDesC8& aContentId,
                                       const TDesC8& aLocation, TInt aAttachmentNameOrData)
{
    JELOG2(EWMA);
    mEncoding = -1;
    if (aAttachmentNameOrData)
    {
        S60CommonUtils::ConvertNarrowToWiderL(aDataArray,
                                              (TDesC*&) mAttachmentNameWithPath);
    }
    else
    {
        mAttachmentData8 = const_cast<TDesC8*>(&aDataArray);
    }
    SetAttchmentTypeLC(aAttachmentType); // copy the attachment type

    if ((NULL == &aContentId) || (aContentId.Length() < 1))
    {
        SetContentIdLC(KNullDesC8);
    }
    else
    {
        SetContentIdLC(aContentId); // set the content ID
    }

    SetContentLocationLC(aLocation); // set content location
    CleanupStack::Pop(mContantLocation8);
    CleanupStack::Pop(mContantId8);
    CleanupStack::Pop(mAttachmentTypeBuf8);
}
/* ------------------------------------------------------------------------
 * private SetAttchmentType
 *
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::SetAttchmentTypeLC(const TDesC8& aAttachmentType)
{
    JELOG2(EWMA);
    S60CommonUtils::CopyNarrowLC(aAttachmentType, (TDesC8*&)mAttachmentTypeBuf8);
}
/* ------------------------------------------------------------------------
 * private void SetContentIdLC(const TDesC8& aContentId)
 * set content location
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::SetContentIdLC(const TDesC8& aContentId)
{
    JELOG2(EWMA);
    S60CommonUtils::CopyNarrowLC(aContentId, (TDesC8*&) mContantId8);
}
/* ------------------------------------------------------------------------
 * void SetContentLocationLC(const TDesC8& aLocation)
 * private set contant location
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::SetContentLocationLC(const TDesC8& aLocation)
{
    JELOG2(EWMA);
    S60CommonUtils::CopyNarrowLC(aLocation, (TDesC8*&) mContantLocation8);
}
/* ------------------------------------------------------------------------
 * public SetEncoding
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::SetEncoding(TUint aEncoding)
{
    JELOG2(EWMA);
    mEncoding = aEncoding;
}
/* ------------------------------------------------------------------------
 * public TUint Encoding
 * ------------------------------------------------------------------------
 */
TInt CMMSMessageAttachment::Encoding()
{
    JELOG2(EWMA);
    return mEncoding;
}
/* ------------------------------------------------------------------------
 * TDesC& AttachmentName()
 * public get attchment name to add in the message attachment
 * ------------------------------------------------------------------------
 */
TDesC& CMMSMessageAttachment::AttachmentName()
{
    JELOG2(EWMA);
    return *mAttachmentNameWithPath;
}
/* ------------------------------------------------------------------------
 *
 *
 * ------------------------------------------------------------------------
 */
CMMSMessageAttachment::~CMMSMessageAttachment()
{
    JELOG2(EWMA);
    if (mAttachmentNameWithPath)
    {
        mRFs.Delete(*mAttachmentNameWithPath);
        delete mAttachmentNameWithPath;
        mAttachmentNameWithPath = NULL;
    }

    delete mAttachmentTypeBuf8;
    mAttachmentTypeBuf8 = NULL;
    delete mAttachmentData8;
    mAttachmentData8 = NULL;
    delete mContantId8;
    mContantId8 = NULL;
    delete mContantLocation8;
    mContantLocation8 = NULL;
    mEncoding = -1;
}
/* ------------------------------------------------------------------------
 * public TDesC8& AttachmentType
 * we are passing reference to the private member. It is not that bad as
 * this method is not exported and there is only one user of it.
 * ------------------------------------------------------------------------
 */
TDesC8& CMMSMessageAttachment::AttachmentType()
{
    JELOG2(EWMA);
    return *mAttachmentTypeBuf8;
}

/* ------------------------------------------------------------------------
 * public AttachmentData
 * get the attachment data
 * ------------------------------------------------------------------------
 */
TDesC8& CMMSMessageAttachment::AttachmentData()
{
    JELOG2(EWMA);
    return *mAttachmentData8;
}

/* ------------------------------------------------------------------------
 * public TInt IsStarterContentId
 * get starter Id, If true then this message part is started Id
 * ------------------------------------------------------------------------
 */
TInt CMMSMessageAttachment::IsStarterContentId()
{
    JELOG2(EWMA);
    return mIsStarterContentId;
}

/* ------------------------------------------------------------------------
 * public SetStarterContentId
 * by default it is true
 * ------------------------------------------------------------------------
 */
void CMMSMessageAttachment::SetStarterContentId(TInt aValue)
{
    JELOG2(EWMA);
    mIsStarterContentId = aValue;
}

/* ------------------------------------------------------------------------
 * TDesC8& ContentIdL()
 * ------------------------------------------------------------------------
 */
TDesC8& CMMSMessageAttachment::ContentIdL()
{
    JELOG2(EWMA);
    return *mContantId8;
}
/* ------------------------------------------------------------------------
 * TDesC8& ContentLocationL
 * ------------------------------------------------------------------------
 */
TDesC8& CMMSMessageAttachment::ContentLocationL()
{
    JELOG2(EWMA);
    return *mContantLocation8;
}

// end of the file
} //namespace wma
} //namespace java

