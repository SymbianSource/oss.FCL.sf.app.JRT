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


#ifndef CMMSMESSAGEINFORMATION_H
#define CMMSMESSAGEINFORMATION_H

// INCLUDE FILES
#include <e32std.h>
#include <mmsconst.h>
#include <msvstd.h>
#include "cmmsmessageattachment.h"

namespace java
{
namespace wma
{

/**
 * Class to store the MMS messages in the  native side
 * 1. Meessage Time
 * 2. ApplicationId
 * 3. ReplyToApplicationId
 * 4. Addresses (To, Cc, Bcc) count
 * 5. Addresses (To, Cc, Bcc)
 * 6. Subject
 * 7. Message Priority
 * 8. Attchment count
 * 9. Attachments
 *
 *  @lib hmidpS60v30.lib on wins/winscw and midp2.exe
 */
class CMMSMessageInformation: public CBase
{

public:
    /**
     * Create the Java counter part of MultipartMessage.
     * @param aFromAddress: From address of received message
     * @param aDate: date When the message is created
     * @param aApplicationID8: ApplicationId
     * @param aReceivedMsvId: Id of the received message
     *
     * @return newly created CMidMessageInformation
     */
    static CMMSMessageInformation* NewL(const TDesC& aFromAddress,
                                        TInt64 aDate, const TDesC8& aApplicationID8, TMsvId aReceivedMsvId);

    /**
     * Get the TMsvId of received message.
     *
     * @return TMsvId (Received message Id)
     */
    inline TMsvId MessageMsvId();

    /**
     * Get the from address of received message
     *
     * @return TDesC8
     */
    TDesC8& MessageFromAddressL();

    /**
     * Get the creation time of message
     *
     * @return TInt64
     */
    inline TInt64 MessageTime();

    /**
     * Set the applicationId from TDesC16
     * @param aAppId set the application Id in message.
     *
     * @return
     */
    void SetApplicationIdL(const TDesC16& aAppId);

    /**
     * Set the applicationId from TDesC8
     * @param aAppId set the application Id in message.
     *
     * @return
     */
    void SetApplicationIdL(const TDesC8& aAppId);

    /**
     * Get the application Id
     *
     * @return TDesC8&
     */
    TDesC8& ApplicationIdL();

    /**
     * Set the ReplyToApplicationId from TDesC16
     * @param aReplyAppId: ReplyToApplication Id as TDesC16
     *
     * @return
     */
    void SetReplyToApplicationIdL(const TDesC16& aReplyAppId);

    /**
     * Set the ReplyToApplicationId from TDesC8
     * @param aReplyAppId: ReplyToApplication Id as TDesC8
     *
     * @return
     */
    void SetReplyToApplicationIdL(const TDesC8& aReplyAppId);

    /**
     * Get the ReplyToApplicationId
     *
     * @return TDesC8
     */
    TDesC8& ReplyToApplicationIdL();

    /**
     * Get the address count in given address type
     * @param aAddressType: Address type to, cc, bcc
     *
     * @return TInt
     */
    TInt AddressesCount(TMsvRecipientTypeValues aAddressType);

    /**
     * Get the address by type and index
     * @param aAddressType: address type
     * @param aIndex:
     *
     * @return TDesC
     */
    TDesC& MessageAddressL(TMsvRecipientTypeValues aAddressType, TInt aIndex);

    /**
     * Add the address by type in CMidMessageInformation. Owner ship is
     *  transfered.
     * @param aAddressType:  is TDesC16
     * @param aAddress: owner ship will transfer to CMidMessageInformation
     *
     * @return
     */
    void AddAddressL(TMsvRecipientTypeValues aAddressType, TDesC16* aAddress);

    /**
     * Add the address by type in CMidMessageInformation.
     * @param aAddressType:  is TDesC8
     * @param aAddress:
     *
     * @return
     */
    void AddAddressL(TMsvRecipientTypeValues aAddressType, TDesC8& aAddress8);

    /**
     * Set the priority of CMidMessageInformation
     * @param aPriority: set the message priority
     *
     * @return
     */
    void SetMessagePriority(TMmsMessagePriority aPriority);

    /**
     * Get the priority of CMidMessageInformation
     *
     * @return
     */
    inline TMmsMessagePriority MessagePriority();

    /**
     * Set the subject of CMidMessageInformation
     * @param aSubject as TDesC16&
     *
     * @return
     */
    void SetMessageSubjectL(TDesC16& aSubject);

    /**
     * Set the subject of CMidMessageInformation
     * @param aSubject as TDesC8&
     *
     * @return
     */
    void SetMessageSubjectL(TDesC8& aSubject);

    /**
     * Get the subject of CMidMessageInformation
     *
     * @return TDesC8& (Subject)
     */
    TDesC8& MessageSubjectL() const;

    /**
     * Set the size of CMidMessageInformation
     * @param aIndex
     *
     * @return
     */
    void SetMessageSize(TInt aSize);

    /**
     * Get the size of CMidMessageInformation
     * @param aIndex
     *
     * @return TInt (the message size)
     */
    inline TInt MessageSize() const;

    /**
     * Inser CMMSMessageAttachment. Owner ship is transfer to
     * CMidMessageInformation
     * @param aIndex
     *
     * @return CMMSMessageAttachment
     */
    void InsertAttachmentL(CMMSMessageAttachment* aMessageAttachment);

    /**
     * Get CMMSMessageAttachment count.
     * @param aIndex
     *
     * @return TInt (attachment count)
     */
    inline TInt AttachmentCount() const;

    /**
     * Get CMMSMessageAttachment by index.
     * Index should not exced the attachment cout.
     * @param aIndex
     *
     * @return CMMSMessageAttachment or return null
     *         if there is not more attachement
     */
    CMMSMessageAttachment& Attachment(TInt aIndex);

    /**
     * Destructor
     *
     * @return
     */
    ~CMMSMessageInformation();

private:
    /**
     * C++ default constructor.
     *
     * @return
     */
    CMMSMessageInformation();

    /**
     * default Symbian 2nd phase constructor.
     * @param aFromAddress
     * @param aDate: date when the message is created
     * @param aApplicationID8: (Optional) Destination applicationId
     * @param aReceivedMsvId: If the message is received in the device
     *                        then the received message Id.
     * @return
     */
    void ConstructL(const TDesC& aFromAddress, TInt64 aDate,
                    const TDesC8& aApplicationID8, TMsvId aReceivedMsvId);

    /**
     * Add the "to" destination address.
     * @param aAddress
     *
     * @return
     */
    void AddToAddressL(TDesC* aAddress);

    /**
     * Add the "cc" destination address.
     * @param aAddress
     *
     * @return
     */
    void AddCcAddressL(TDesC* aAddress);

    /**
     * Add the "bcc" destination address.
     * @param aAddress
     *
     * @return
     */
    void AddBccAddressL(TDesC* aAddress);

private:
    // private members
    TInt mMessageSize;

    TInt64 mDateTime; // Send/Receive message date
    /**
     * own the mFromAddress8
     */
    HBufC8* mFromAddress8; // Address from where the message arrive
    /**
     * own the mApplicationID8
     */
    HBufC8* mApplicationID8; // ApplicationID (on receiving device)
    /**
     * own the mReplyToApplicationID8
     */
    HBufC8* mReplyToApplicationID8;
    /**
     * own the mMessageSubject8
     */
    HBufC8* mMessageSubject8;// Message Subject

    TMsvId mReceivedMsvId;

    TMmsMessagePriority mMessagePriority;
    /**
     * own the mToAddressArray
     */
    CArrayPtrSeg<TDesC>* mToAddressArray;
    /**
     * own the mBccAddressArray
     */
    CArrayPtrSeg<TDesC>* mBccAddressArray;
    /**
     * own the mCcAddressArray
     */
    CArrayPtrSeg<TDesC>* mCcAddressArray;
    /**
     * own the mAttachmentArray
     */
    CArrayPtrSeg<CMMSMessageAttachment>* mAttachmentArray;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
TMmsMessagePriority CMMSMessageInformation::MessagePriority()
{
    return mMessagePriority;
}
//--------------------------------------------------------------------------------------------
// Get the received message Id
//--------------------------------------------------------------------------------------------
TMsvId CMMSMessageInformation::MessageMsvId()
{
    return mReceivedMsvId;
}
//--------------------------------------------------------------------------------------------
//  MessageTime
//    It return the TIn64 (received/created message date)
//--------------------------------------------------------------------------------------------
TInt64 CMMSMessageInformation::MessageTime()
{
    return mDateTime;
}
//--------------------------------------------------------------------------------------------
//    Get Attachment Count
//--------------------------------------------------------------------------------------------
TInt CMMSMessageInformation::AttachmentCount() const
{
    return mAttachmentArray->Count();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
} //namespace wma
} //namespace java
#endif // CMMSMESSAGEINFORMATION_H
// End of File
