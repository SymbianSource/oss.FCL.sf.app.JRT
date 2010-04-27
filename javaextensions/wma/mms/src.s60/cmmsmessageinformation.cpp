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


// INCLUDE FILES
#include "logger.h"
#include "s60commonutils.h"
#include "cmmsmessageinformation.h"

using namespace java::util;

namespace java
{
namespace wma
{
const TInt KArrayGranularity = 2;

// ================= MEMBER FUNCTIONS =======================
//------------------------------------------------------------------------------

/*******************************************************************************
 ***************************   CMidMessageInformation  *************************
 ******************************************************************************/
/*------------------------------------------------------------------------------
 *    CMidMessageInformation
 *
 *------------------------------------------------------------------------------
 */
CMMSMessageInformation::CMMSMessageInformation()
{
    JELOG2(EWMA);
}

/*------------------------------------------------------------------------------
 *  NewL
 *
 *------------------------------------------------------------------------------
 */
CMMSMessageInformation* CMMSMessageInformation::NewL(const TDesC& aFromAddress,
        TInt64 aDate, const TDesC8& aApplicationID8, TMsvId aReceivedMsvId)
{
    JELOG2(EWMA);
    CMMSMessageInformation *self = new(ELeave) CMMSMessageInformation;
    CleanupStack::PushL(self);
    self->ConstructL(aFromAddress, aDate, aApplicationID8, aReceivedMsvId);
    CleanupStack::Pop(self);
    return self;
}
/*------------------------------------------------------------------------------
 *    ConstructL
 *
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::ConstructL(const TDesC& aFromAddress,TInt64 aDate,
                                        const TDesC8& aApplicationID8,
                                        TMsvId aReceivedMsvId)
{
    JELOG2(EWMA);
    S60CommonUtils::ConvertWiderToNarrowL(aFromAddress, (TDesC8*&)mFromAddress8);
    S60CommonUtils::CopyNarrowL(aApplicationID8, (TDesC8*&) mApplicationID8);
    mDateTime = aDate;
    mMessagePriority = EMmsPriorityHigh;
    mReceivedMsvId = aReceivedMsvId;
    mAttachmentArray
    = new(ELeave) CArrayPtrSeg<CMMSMessageAttachment> (KArrayGranularity);
    mToAddressArray = new(ELeave) CArrayPtrSeg<TDesC> (KArrayGranularity);
    mCcAddressArray = new(ELeave) CArrayPtrSeg<TDesC> (1);
    mBccAddressArray = new(ELeave) CArrayPtrSeg<TDesC> (1);
}
/*------------------------------------------------------------------------------
 *    ~CMidMessageInformation
 * This will delete the following
 * iFromAddress8;    // HBufC*
 * iApplicationID8;  // HBufC*
 * iReplyToApplicationID8;
 * iMessageSubject8;  //HBufC*
 * iToAddressArray   Arrays just not their contents
 * iBccAddressArray  Arrays just not their contents
 * iCcAddressArray   Arrays just not their contents
 *------------------------------------------------------------------------------
 */
CMMSMessageInformation::~CMMSMessageInformation()
{
    JELOG2(EWMA);
    delete mFromAddress8;
    mFromAddress8 = NULL; // HBufC*
    delete mApplicationID8;
    mApplicationID8 = NULL; // HBufC*
    delete mReplyToApplicationID8;
    mReplyToApplicationID8 = NULL;
    delete mMessageSubject8;
    mMessageSubject8 = NULL; //HBufC*
    //
    mDateTime = 0;
    mMessageSize = 0;

    if (mAttachmentArray)
    {
        mAttachmentArray->ResetAndDestroy();
        delete mAttachmentArray;
        mAttachmentArray = NULL;
    }

    if (mToAddressArray)
    {
        mToAddressArray->ResetAndDestroy();
        delete mToAddressArray;
        mToAddressArray = NULL;
    }
    if (mBccAddressArray)
    {
        mBccAddressArray->ResetAndDestroy();
        delete mBccAddressArray;
        mBccAddressArray = NULL;
    }
    if (mCcAddressArray)
    {
        mCcAddressArray->ResetAndDestroy();
        delete mCcAddressArray;
        mCcAddressArray = NULL;
    }
}
/*------------------------------------------------------------------------------
 * Attachment
 * return null if there is not more attachement
 *------------------------------------------------------------------------------
 */
CMMSMessageAttachment& CMMSMessageInformation::Attachment(TInt aIndex)
{
    JELOG2(EWMA);
    CMMSMessageAttachment* attachment = NULL;
    if (aIndex >= 0 && aIndex < mAttachmentArray->Count())
        attachment = mAttachmentArray->At(aIndex);
    return *attachment;
}
/*------------------------------------------------------------------------------
 * InsertAttachmentL
 * Insert Attachment, Attachment will be applended in the array
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::InsertAttachmentL(
    CMMSMessageAttachment* aMessageAttachment)
{
    JELOG2(EWMA);
    mAttachmentArray->AppendL(aMessageAttachment);
}
/*------------------------------------------------------------------------------
 * SetMessageSubjectL
 * set the subject of the message
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetMessageSubjectL(TDesC16& aSubject)
{
    JELOG2(EWMA);
    delete mMessageSubject8;
    mMessageSubject8 = NULL;
    S60CommonUtils::ConvertWiderToNarrowL(aSubject, (TDesC8*&) mMessageSubject8);
}
/*------------------------------------------------------------------------------
 * SetMessageSubjectL
 * set the subject of the message
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetMessageSubjectL(TDesC8& aSubject)
{
    JELOG2(EWMA);
    delete mMessageSubject8;
    mMessageSubject8 = NULL;
    S60CommonUtils::CopyNarrowL(aSubject, (TDesC8*&) mMessageSubject8);
}
/*------------------------------------------------------------------------------
 * MessageFromAddress
 *  Get from Message Address
 *------------------------------------------------------------------------------
 */
TDesC8& CMMSMessageInformation::MessageFromAddressL()
{
    JELOG2(EWMA);
    return *mFromAddress8;
}
/*------------------------------------------------------------------------------
 * SetApplicationIdL(const TDesC8& aAppId)
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetApplicationIdL(const TDesC8& aAppId)
{
    JELOG2(EWMA);
    delete mApplicationID8;
    mApplicationID8 = NULL;
    S60CommonUtils::CopyNarrowL(aAppId, (TDesC8*&) mApplicationID8);
}
/*------------------------------------------------------------------------------
 * SetApplicationIdL(const TDesC16& aAppId)
 *
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetApplicationIdL(const TDesC16& aAppId)
{
    JELOG2(EWMA);
    delete mApplicationID8;
    mApplicationID8 = NULL;
    S60CommonUtils::ConvertWiderToNarrowL(aAppId, (TDesC8*&) mApplicationID8);
}
/*------------------------------------------------------------------------------
 *  TDesC8& ApplicationID
 *  GetApplicationID
 *
 *------------------------------------------------------------------------------
 */
TDesC8& CMMSMessageInformation::ApplicationIdL()
{
    JELOG2(EWMA);
    return *mApplicationID8;
}
/*------------------------------------------------------------------------------
 * SetReplyToApplicationIdL(const TDesC16& aReplyAppId)
 * Set reply AppId
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetReplyToApplicationIdL(
    const TDesC16& aReplyAppId)
{
    JELOG2(EWMA);
    delete mReplyToApplicationID8;
    mReplyToApplicationID8 = NULL;
    S60CommonUtils::ConvertWiderToNarrowL(aReplyAppId,
                                          (TDesC8*&) mReplyToApplicationID8);
}
/*------------------------------------------------------------------------------
 * SetApplicationIdL(const TDesC8& aAppId)
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetReplyToApplicationIdL(
    const TDesC8& aReplyToAppId)
{
    JELOG2(EWMA);
    delete mReplyToApplicationID8;
    mReplyToApplicationID8 = NULL;
    S60CommonUtils::CopyNarrowL(aReplyToAppId, (TDesC8*&)mReplyToApplicationID8);
}
/*------------------------------------------------------------------------------
 * TDesC8& ReplyToApplicationIdL
 *------------------------------------------------------------------------------
 */
TDesC8& CMMSMessageInformation::ReplyToApplicationIdL()
{
    JELOG2(EWMA);
    return *mReplyToApplicationID8;
}
/*------------------------------------------------------------------------------
 * TDesC8& MessageSubject
 * Get the message subject
 *------------------------------------------------------------------------------
 */
TDesC8& CMMSMessageInformation::MessageSubjectL() const
{
    JELOG2(EWMA);
    return *mMessageSubject8;
}
/*------------------------------------------------------------------------------
 * TInt SetMessageSize
 * Set the message size
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetMessageSize(TInt aSize)
{
    JELOG2(EWMA);
    mMessageSize = aSize;
}
/*------------------------------------------------------------------------------
 * TInt AddressesCount
 *------------------------------------------------------------------------------
 */
TInt CMMSMessageInformation::AddressesCount(
    TMsvRecipientTypeValues aAddressType)
{
    JELOG2(EWMA);
    TInt count = 0;
    switch (aAddressType)
    {
    case EMsvRecipientTo:
    {
        count = mToAddressArray->Count();
        break;
    }
    case EMsvRecipientCc:
    {
        count = mCcAddressArray->Count();
        break;
    }
    case EMsvRecipientBcc:
    {
        count = mBccAddressArray->Count();
        break;
    }
    default:
        break;
    }
    return count;
}
/*------------------------------------------------------------------------------
 * TDesC& MessageAddress
 * return type is TDesC
 *------------------------------------------------------------------------------
 */
TDesC& CMMSMessageInformation::MessageAddressL(
    TMsvRecipientTypeValues aAddressType, TInt aIndex)
{
    JELOG2(EWMA);
    TDesC* address = NULL;

    if (aIndex < 0) // aIndex is less then zero return null;
        return *address;

    switch (aAddressType)
    {
    case EMsvRecipientTo:
    {
        if (aIndex < mToAddressArray->Count())
            address = mToAddressArray->At(aIndex);
        break;
    }
    case EMsvRecipientCc:
    {
        if (aIndex < mCcAddressArray->Count())
            address = mCcAddressArray->At(aIndex);
        break;
    }
    case EMsvRecipientBcc:
    {
        if (aIndex < mBccAddressArray->Count())
            address = mBccAddressArray->At(aIndex);
        break;
    }
    default:
        break;
    }
    return *address;
}

/*------------------------------------------------------------------------------
 * AddAddressL(TMsvRecipientTypeValues aAddressType, TDesC8& aAddress8)
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::AddAddressL(TMsvRecipientTypeValues aAddressType,
        TDesC8& aAddress8)
{
    JELOG2(EWMA);
    TDesC16* address = NULL;
    S60CommonUtils::ConvertNarrowToWiderLC(aAddress8, address);
    AddAddressL(aAddressType, address);
    CleanupStack::Pop(address);
}

/*------------------------------------------------------------------------------
 * AddAddressL(TMsvRecipientTypeValues aAddressType, TDesC16& aAddress)
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::AddAddressL(TMsvRecipientTypeValues aAddressType,
        TDesC16* aAddress)
{
    JELOG2(EWMA);
    if (!aAddress)
        return;

    switch (aAddressType)
    {
    case EMsvRecipientTo:
    {
        AddToAddressL(aAddress);
        break;
    }
    case EMsvRecipientCc:
    {
        AddCcAddressL(aAddress);
        break;
    }
    case EMsvRecipientBcc:
    {
        AddBccAddressL(aAddress);
        break;
    }
    default:
        break;
    }
}
/*------------------------------------------------------------------------------
 * AddToAddressL
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::AddToAddressL(TDesC* aAddress)
{
    JELOG2(EWMA);
    if (aAddress)
        mToAddressArray->AppendL(aAddress);
}
/*------------------------------------------------------------------------------
 * AddCcAddressL
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::AddCcAddressL(TDesC* aAddress)
{
    JELOG2(EWMA);
    if (aAddress)
        mCcAddressArray->AppendL(aAddress);
}
/*------------------------------------------------------------------------------
 *
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::AddBccAddressL(TDesC* aAddress)
{
    JELOG2(EWMA);
    if (aAddress)
        mBccAddressArray->AppendL(aAddress);
}
/*------------------------------------------------------------------------------
 * SetMessagePriority
 *
 *------------------------------------------------------------------------------
 */
void CMMSMessageInformation::SetMessagePriority(TMmsMessagePriority aPriority)
{
    JELOG2(EWMA);
    mMessagePriority = aPriority;
}

// End of File
} //namespace wma
} //namespace java

