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


#include <apmstd.h>
#include <s32stor.h>
#include <badesca.h>  //to use descriptor arrays
#include <eikenv.h>
#include <eikdef.h>
#include <driveinfo.h>
#include <DocumentHandler.h>

#include "logger.h"
#include "s60commonutils.h"
#include "javasymbianoslayer.h"
#include "cjavammsmessagehandler.h"

namespace java
{
namespace wma
{

const TInt KMmsMapLength = 7;
const TInt KReplyToAppIDFieldIndex = 0;
const TInt KAppIDFieldIndex = 1;
const TInt KSubjectFieldIndex = 2;
const TInt KFromFieldIndex = 3;
const TInt KToFieldIndex = 4;
const TInt KBccFieldIndex = 5;
const TInt KCcFieldIndex = 6;
const TInt KBufFlatGranularity = 10;

// ============================ MEMBER FUNCTIONS ===============================

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::CJavaMMSMessageHandler
 * C++ default constructor
 * -----------------------------------------------------------------------------
 */
CJavaMMSMessageHandler::CJavaMMSMessageHandler(RFs aRFs) :
        mRFs(aRFs)
{
    JELOG2(EWMA);
}

/* -----------------------------------------------------------------------------
 * Destructor
 * -----------------------------------------------------------------------------
 */
CJavaMMSMessageHandler::~CJavaMMSMessageHandler()
{
    JELOG2(EWMA);
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::NewL
 * Two-phased constructor.
 * -----------------------------------------------------------------------------
 */
CJavaMMSMessageHandler* CJavaMMSMessageHandler::NewL(RFs aFs)
{
    JELOG2(EWMA);
    CJavaMMSMessageHandler* self = new CJavaMMSMessageHandler(aFs);
    return self;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::SerializeLC
 * This method serializes an MMS (as it is received from the MMS engine) into
 * a byte array which will be sent to java.
 * The format of the serialized MMS is according to the following scheme:
 *     ----------------------------------------------------------------------------------------------------------
 *     | Map | HeaderLengths | AttachmentsCount | AttachmentsLengths | StartContentFlag | Headers | Attachments |
 *     ----------------------------------------------------------------------------------------------------------
 *
 * where Map                 Represents the decimal value of a binary
 *                          representation of MMS header fields
 *                          (applicationID, Subject, From, To, Bcc, Cc)
 *                          Ex: 52 = 110101 means that 'applicationID'
 *                          is present, 'Subject' is present, 'From' is
 *                          not present,'To' is present, 'Bcc' is not
 *                          present and 'Cc' is present
 *                          The lengths of each of the MMS header
 *                          fields which are present
 *      AttachmentsCount    The number of attachments
 *      AttachmentsLengths  The lengths of each of the attachments
 *      StartContentFlag    A boolean indicating if the first
 *                          attachment should be interpreted as
 *                          startContent or not
 *      Headers             The MMS header fields
 *      Attachments         The attachments
 * -----------------------------------------------------------------------------
 */
CBufFlat& CJavaMMSMessageHandler::SerializeLC(CMMSMessageInformation& aMMS)
{
    JELOG2(EWMA);
    // mms fields
    TDesC8& replyToAppID = aMMS.ReplyToApplicationIdL();
    TDesC8& appID = aMMS.ApplicationIdL();
    TDesC8& subject = aMMS.MessageSubjectL();
    TDesC8& from = aMMS.MessageFromAddressL();
    // the address is on the cleanup stack
    HBufC8& to = BuildAddressLC(EMsvRecipientTo, aMMS);
    // the address is on the cleanup stack
    HBufC8& bcc = BuildAddressLC(EMsvRecipientBcc, aMMS);
    // the address is on the cleanup stack
    HBufC8& cc = BuildAddressLC(EMsvRecipientCc, aMMS);

    // calculate the mms Map
    TInt mmsMap = 0;
    mmsMap += CalculateMap(replyToAppID, appID, subject, from, to, bcc, cc);

    // The serialized buffer
    CBufFlat* buffer = CBufFlat::NewL(KBufFlatGranularity);
    CleanupStack::PushL(buffer);
    CCnvCharacterSetConverter* cConv = CCnvCharacterSetConverter::NewL();
    CleanupStack::PushL(cConv);
    RBufWriteStream writeStream(*buffer, 0);

    // add the mmsMap
    writeStream.WriteUint8L(mmsMap);

    // add the mms fields lengths
    WriteDes8LengthL(writeStream, replyToAppID);
    WriteDes8LengthL(writeStream, appID);
    WriteDes8LengthL(writeStream, subject);
    WriteDes8LengthL(writeStream, from);
    WriteDes8LengthL(writeStream, to);
    WriteDes8LengthL(writeStream, bcc);
    WriteDes8LengthL(writeStream, cc);

    // add the attachments count
    TInt8 attchmts = aMMS.AttachmentCount();
    writeStream.WriteUint8L(attchmts);

    // add the attachments lenghts
    TInt firstAttchmt = WriteAttachmentsLengthsL(writeStream, aMMS, *cConv);

    // add the starterCID indicator
    if (attchmts > 0)
    {
        if (firstAttchmt >= 0)
        {
            writeStream.WriteUint8L(1);
        }
        else
        {
            writeStream.WriteUint8L(0);
        }
    }

    // add the priority
    TMmsMessagePriority priority = aMMS.MessagePriority();
    switch (priority)
    {
    case EMmsPriorityLow:
        writeStream.WriteUint8L(KPriorityLow);
        break;
    case EMmsPriorityNormal:
        writeStream.WriteUint8L(KPriorityNormal);
        break;
    case EMmsPriorityHigh:
        writeStream.WriteUint8L(KPriorityHigh);
        break;
    default:
        writeStream.WriteUint8L(KPriorityNormal);
        break;
    }

    // add the date
    WriteInt64L(writeStream, aMMS.MessageTime());

    // add the mms fields
    WriteDes8L(writeStream, replyToAppID);
    WriteDes8L(writeStream, appID);
    WriteDes8L(writeStream, subject);
    WriteDes8L(writeStream, from);
    WriteDes8L(writeStream, to);
    WriteDes8L(writeStream, bcc);
    WriteDes8L(writeStream, cc);

    // add the attachments
    WriteAttachmentsL(writeStream, aMMS, *cConv, firstAttchmt);

    // cleanup
    CleanupStack::PopAndDestroy(cConv);
    CleanupStack::Pop(buffer);
    CleanupStack::PopAndDestroy(&cc);
    CleanupStack::PopAndDestroy(&bcc);
    CleanupStack::PopAndDestroy(&to);
    CleanupStack::PushL(buffer);

    return *buffer;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteAttachmentsL
 * Writes all the attachments into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteAttachmentsL(RBufWriteStream& aWriteStream,
        CMMSMessageInformation& aMMS, CCnvCharacterSetConverter& aCharConv,
        TInt firstAttchmt)
{
    JELOG2(EWMA);
    TInt8 attchmts = aMMS.AttachmentCount();
    if (firstAttchmt >= 0)
    {
        WriteAttachmentL(aWriteStream, aMMS.Attachment(firstAttchmt), aCharConv);
    }
    for (TInt i = 0; i < attchmts; i++)
    {
        if (i != firstAttchmt)
        {
            WriteAttachmentL(aWriteStream, aMMS.Attachment(i), aCharConv);
        }
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteAttachmentL
 * Writes an attachment into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteAttachmentL(RBufWriteStream& aWriteStream,
        CMMSMessageAttachment& aAttch, CCnvCharacterSetConverter& aCharConv)
{
    JELOG2(EWMA);
    // mime type
    WriteDes8L(aWriteStream, aAttch.AttachmentType());
    // content ID
    WriteDes8L(aWriteStream, aAttch.ContentIdL());
    // content location
    WriteDes8L(aWriteStream, aAttch.ContentLocationL());
    // content encoding
    TUint charSetID = aCharConv.ConvertMibEnumOfCharacterSetToIdentifierL(
                          aAttch.Encoding(), mRFs);
    HBufC8* encoding = aCharConv.ConvertCharacterSetIdentifierToStandardNameL(
                           charSetID, mRFs);
    CleanupStack::PushL(encoding);
    if (encoding)
    {
        aWriteStream.WriteL(*(encoding));
    }
    CleanupStack::PopAndDestroy(encoding);
    // content
    WriteDes8L(aWriteStream, aAttch.AttachmentData());
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteAttachmentsLengthsL
 * Writes all attachments lengths (one by one) into the write stream.
 * -----------------------------------------------------------------------------
 */
TInt CJavaMMSMessageHandler::WriteAttachmentsLengthsL(RBufWriteStream& aWriteStream,
        CMMSMessageInformation& aMMS, CCnvCharacterSetConverter& aCharConv)
{
    JELOG2(EWMA);
    TInt firstAttchmt = -1;
    TInt8 attchmts = aMMS.AttachmentCount();
    if (attchmts > 0)
    {
        // quick browse through the attachments to see if the starterCID is used
        for (TInt i = 0; i < attchmts; i++)
        {
            CMMSMessageAttachment& attch = aMMS.Attachment(i);
            if (attch.IsStarterContentId())
            {
                // put the starterCID first
                WriteAttachmentLengthsL(aWriteStream, attch, aCharConv);
                firstAttchmt = i;
                break;
            }
        }
    }
    for (TInt i = 0; i < attchmts; i++)
    {
        if (i != firstAttchmt)
        {
            WriteAttachmentLengthsL(aWriteStream, aMMS.Attachment(i), aCharConv);
        }
    }
    return firstAttchmt;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteAttachmentLengthsL
 * Writes an attachment lengths into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteAttachmentLengthsL(RBufWriteStream& aWriteStream,
        CMMSMessageAttachment& aAttch, CCnvCharacterSetConverter& aCharConv)
{
    JELOG2(EWMA);
    // mime type length
    WriteDesLengthOrZeroL(aWriteStream, aAttch.AttachmentType());
    // content ID length
    WriteDesLengthOrZeroL(aWriteStream, aAttch.ContentIdL());
    // content location length
    WriteDesLengthOrZeroL(aWriteStream, aAttch.ContentLocationL());
    // content encoding length
    TUint charSetID = aCharConv.ConvertMibEnumOfCharacterSetToIdentifierL(
                          aAttch.Encoding(), mRFs);
    HBufC8* encoding = aCharConv.ConvertCharacterSetIdentifierToStandardNameL(
                           charSetID, mRFs);
    CleanupStack::PushL(encoding);
    if (encoding)
    {
        WriteIntL(aWriteStream, encoding->Length());
    }
    else
    {
        WriteIntL(aWriteStream, 0);
    }
    CleanupStack::PopAndDestroy(encoding);
    // content size
    WriteDesLengthOrZeroL(aWriteStream, aAttch.AttachmentData());
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::BuildAddressLC
 * Concatenates all the addresses/recipients into a ";" separated
 * string/descriptor.
 * -----------------------------------------------------------------------------
 */
HBufC8& CJavaMMSMessageHandler::BuildAddressLC(
    TMsvRecipientTypeValues aAddressType, CMMSMessageInformation& aMMS)
{
    JELOG2(EWMA);
    _LIT(KAddressSep, ";");
    HBufC8* address = HBufC8::NewLC(0);
    TPtr8 AddressPtr(address->Des());
    for (int i = 0; i < aMMS.AddressesCount(aAddressType); i++)
    {
        TDesC& currentAddress = aMMS.MessageAddressL(aAddressType, i);
        HBufC8* tmp = NULL;
        if (i < aMMS.AddressesCount(aAddressType) - 1)
        {
            tmp = address->ReAllocL(address->Length() + currentAddress.Length()
                                    + 1);
        }
        else
        {
            tmp
            = address->ReAllocL(address->Length()
                                + currentAddress.Length());
        }
        CleanupStack::Pop(address);
        address = tmp;
        CleanupStack::PushL(address);
        AddressPtr.Set(address->Des());
        AddressPtr.Append(currentAddress);
        if (i < aMMS.AddressesCount(aAddressType) - 1)
        {
            AddressPtr.Append(KAddressSep);
        }
    }
    return *address;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::CalculateMap
 * Calculates the mms map.
 * -----------------------------------------------------------------------------
 */
TInt CJavaMMSMessageHandler::CalculateMap(TDesC8& aReplyToAppID, TDesC8& aAppID,
        TDesC8& aSubject, TDesC8& aFrom,
        HBufC8& aTo, HBufC8& aBcc, HBufC8& aCc)
{
    JELOG2(EWMA);
    TInt mmsMap = 0;
    if (&aReplyToAppID)
    {
        mmsMap += K64BitsMask;
    }
    if (&aAppID)
    {
        mmsMap += K32BitsMask;
    }
    if (&aSubject)
    {
        mmsMap += K16BitsMask;
    }
    if (&aFrom)
    {
        mmsMap += K8BitsMask;
    }
    if (&aTo)
    {
        mmsMap += K4BitsMask;
    }
    if (&aBcc)
    {
        mmsMap += K2BitsMask;
    }
    if (&aCc)
    {
        mmsMap += 1;
    }
    return mmsMap;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteInt64L
 * Writes an integer as 8 bytes into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteInt64L(RBufWriteStream& aWriteStream,
        const TInt64 aInteger)
{
    JELOG2(EWMA);
    aWriteStream.WriteInt8L(aInteger >> K56BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K48BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K40BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K32BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K24BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K16BitsMask);
    aWriteStream.WriteInt8L(aInteger >> K8BitsMask);
    aWriteStream.WriteInt8L(aInteger);
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteIntL
 * Writes an integer as 4 bytes into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteIntL(RBufWriteStream& aWriteStream,
                                       const TInt32 aInteger)
{
    JELOG2(EWMA);
    aWriteStream.WriteUint8L(aInteger >> K24BitsMask);
    aWriteStream.WriteUint8L(aInteger >> K16BitsMask);
    aWriteStream.WriteUint8L(aInteger >> K8BitsMask);
    aWriteStream.WriteUint8L(aInteger);
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteDes8L
 * Writes a descriptor (if not NULL) into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteDes8L(RBufWriteStream& aWriteStream,
                                        const TDesC8& aDes)
{
    JELOG2(EWMA);
    if (&aDes)
    {
        aWriteStream.WriteL(aDes);
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteDes8LengthL
 * Writes a descriptor's length (if not NULL) into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteDes8LengthL(RBufWriteStream& aWriteStream,
        const TDesC8& aDes)
{
    JELOG2(EWMA);
    if (&aDes)
    {
        WriteIntL(aWriteStream, aDes.Length());
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteDes8LengthOrZeroL
 * Writes a descriptor's length (if descriptor is not NULL) or zero (if
 * descriptor is NULL) into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteDes8LengthOrZeroL(RBufWriteStream& aWriteStream,
        const TDesC8& aDes)
{
    JELOG2(EWMA);
    if (&aDes)
    {
        aWriteStream.WriteUint8L(aDes.Length());
    }
    else
    {
        aWriteStream.WriteUint8L(0);
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::WriteDesLengthOrZeroL
 * Writes a descriptor's length (if descriptor is not NULL) or zero (if
 * descriptor is NULL) into the write stream.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::WriteDesLengthOrZeroL(RBufWriteStream& aWriteStream,
        const TDesC8& aDes)
{
    JELOG2(EWMA);
    if (&aDes)
    {
        WriteIntL(aWriteStream, aDes.Length());
    }
    else
    {
        WriteIntL(aWriteStream, 0);
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::DeSerializeL
 * This method de-serializes an MMS (as it is received java) into
 * an internal representation (used by the MMS engine proxy).
 * The format of the serialized MMS is according to the following scheme:
 *     ----------------------------------------------------------------------------------------------------------
 *     | Map | HeaderLengths | AttachmentsCount | AttachmentsLengths | StartContentFlag | Headers | Attachments |
 *     ----------------------------------------------------------------------------------------------------------
 *
 * where Map                 Represents the decimal value of a binary
 *                          representation of MMS header fields
 *                          (applicationID, Subject, From, To, Bcc, Cc)
 *                          Ex: 52 = 110101 means that 'applicationID'
 *                          is present, 'Subject' is present, 'From' is
 *                          not present,'To' is present, 'Bcc' is not
 *                          present and 'Cc' is present
 *                          The lengths of each of the MMS header
 *                          fields which are present
 *      AttachmentsCount    The number of attachments
 *      AttachmentsLengths  The lengths of each of the attachments
 *      StartContentFlag    A boolean indicating if the first
 *                          attachment should be interpreted as
 *                          startContent or not
 *      Headers             The MMS header fields
 *      Attachments         The attachments
 * -----------------------------------------------------------------------------
 */
CMMSMessageInformation& CJavaMMSMessageHandler::DeSerializeL(CBufFlat& aMMS)
{
    JELOG2(EWMA);
    RBufReadStream rStream(aMMS, 0);

    // get the raw mms map
    TInt32 headerLengths[KMmsMapLength] =
        { 0, 0, 0, 0, 0, 0 };

    ResolveMapL(rStream, headerLengths);
    // get the attachments count
    TInt8 attchmts = rStream.ReadUint8L();

    // get the attachments lenghts
    RArray<TInt> mimeLengths(1);
    CleanupClosePushL(mimeLengths);
    RArray<TInt> cidLengths(1);
    CleanupClosePushL(cidLengths);
    RArray<TInt> cLocationLengths(1);
    CleanupClosePushL(cLocationLengths);
    RArray<TInt> cEncodingLengths(1);
    CleanupClosePushL(cEncodingLengths);
    RArray<TInt32> contentLengths(1);
    CleanupClosePushL(contentLengths);
    for (TInt i = 0; i < attchmts; i++)
    {
        // read the mime length
        mimeLengths.InsertL(ReadIntL(rStream), i);
        // read the content ID length
        cidLengths.InsertL(ReadIntL(rStream), i);
        // read the content Location length
        cLocationLengths.InsertL(ReadIntL(rStream), i);
        // read the content Encoding length
        cEncodingLengths.InsertL(ReadIntL(rStream), i);
        // read the content length
        contentLengths.InsertL(ReadIntL(rStream), i);
    }

    // get the startContentId flag
    TInt8 startCID = 0;
    if (attchmts > 0)
    {
        startCID = rStream.ReadUint8L();
    }

    CMMSMessageInformation& msg = ResolveHeadersLC(rStream, headerLengths);
    // msg is on the cleanup stack

    // read the attachments
    CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewL();
    CleanupStack::PushL(charConv);
    for (TInt i = 0; i < attchmts; i++)
    {
        HBufC8* mime = HBufC8::NewL(mimeLengths[i]);
        CleanupStack::PushL(mime);
        TPtr8 mimePtr(mime->Des());
        HBufC8* cid = HBufC8::NewL(cidLengths[i]);
        CleanupStack::PushL(cid);
        TPtr8 cidPtr(cid->Des());
        HBufC8* cLocation = HBufC8::NewL(cLocationLengths[i]);
        CleanupStack::PushL(cLocation);
        TPtr8 cLocationPtr(cLocation->Des());
        HBufC8* cEncoding = HBufC8::NewL(cEncodingLengths[i]);
        CleanupStack::PushL(cEncoding);
        TPtr8 cEncodingPtr(cEncoding->Des());
        HBufC8* content = HBufC8::NewL(contentLengths[i]);
        CleanupStack::PushL(content);
        TPtr8 contentPtr(content->Des());
        // read the mime
        rStream.ReadL(mimePtr, mimeLengths[i]);
        // read the content ID
        rStream.ReadL(cidPtr, cidLengths[i]);
        // read the content Location
        rStream.ReadL(cLocationPtr, cLocationLengths[i]);
        // read the content Encoding
        rStream.ReadL(cEncodingPtr, cEncodingLengths[i]);
        // read the content
        rStream.ReadL(contentPtr, contentLengths[i]);

        // Save the contentPtr in the file and later add the fileName
        TDesC8* attachmentName = NULL;
        SaveDataToFileL(contentPtr, attachmentName, *mime);
        CMMSMessageAttachment* attch = CMMSMessageAttachment::NewL(mRFs,
                                       *attachmentName, mimePtr, cidPtr, cLocationPtr);
        delete attachmentName;
        attachmentName = NULL;
        TUint charSetID = charConv->ConvertStandardNameOfCharacterSetToIdentifierL
                          (cEncodingPtr, mRFs);
        TUint mibEnum = charConv->ConvertCharacterSetIdentifierToMibEnumL(
                            charSetID, mRFs);
        attch->SetEncoding(mibEnum);
        CleanupStack::PushL(attch);
        if (i == 0 && startCID == 1)
        {
            attch->SetStarterContentId(TRUE);
        }
        else
        {
            attch->SetStarterContentId(FALSE);
        }
        msg.InsertAttachmentL(attch);
        CleanupStack::Pop(attch);
        CleanupStack::PopAndDestroy(content);
        CleanupStack::PopAndDestroy(cEncoding);
        CleanupStack::PopAndDestroy(cLocation);
        CleanupStack::PopAndDestroy(cid);
        CleanupStack::PopAndDestroy(mime);
    }
    CleanupStack::PopAndDestroy(charConv);
    CleanupStack::Pop(&msg); // msg

    // deallocate the dynamic arrays
    CleanupStack::PopAndDestroy(&contentLengths);
    CleanupStack::PopAndDestroy(&cEncodingLengths);
    CleanupStack::PopAndDestroy(&cLocationLengths);
    CleanupStack::PopAndDestroy(&cidLengths);
    CleanupStack::PopAndDestroy(&mimeLengths);
    return msg;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::Divide
 * This method returns what's left from dividing aNum1 with aNum2.
 * It is used in serialization/deserialization, when calculating the mmsMap
 * -----------------------------------------------------------------------------
 */
TInt CJavaMMSMessageHandler::Divide(TInt aNum1, TInt aNum2)
{
    JELOG2(EWMA);
    if (aNum1 / aNum2 > 0)
    {
        return (aNum1 - (aNum2 *(aNum1 / aNum2)));
    }
    return -1;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::ReadIntL
 * Reads an integer as 4 bytes
 * -----------------------------------------------------------------------------
 */
TInt32 CJavaMMSMessageHandler::ReadIntL(RBufReadStream& aReadStream)
{
    JELOG2(EWMA);
    TInt32 byte1 = aReadStream.ReadUint8L();
    TInt32 byte2 = aReadStream.ReadUint8L();
    TInt32 byte3 = aReadStream.ReadUint8L();
    TInt32 byte4 = aReadStream.ReadUint8L();
    return (byte1 << K24BitsMask) + (byte2 << K16BitsMask) + (byte3
            << K8BitsMask) + byte4;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::ReadInt64L
 * Reads an integer as 8 bytes
 * -----------------------------------------------------------------------------
 */
TInt64 CJavaMMSMessageHandler::ReadInt64L(RBufReadStream& aReadStream)
{
    JELOG2(EWMA);
    TInt64 byte1 = aReadStream.ReadUint8L();
    TInt64 byte2 = aReadStream.ReadUint8L();
    TInt64 byte3 = aReadStream.ReadUint8L();
    TInt64 byte4 = aReadStream.ReadUint8L();
    TInt64 byte5 = aReadStream.ReadUint8L();
    TInt64 byte6 = aReadStream.ReadUint8L();
    TInt64 byte7 = aReadStream.ReadUint8L();
    TInt64 byte8 = aReadStream.ReadUint8L();
    return (byte1 << K56BitsMask) + (byte2 << K48BitsMask) + (byte3
            << K40BitsMask) + (byte4 << K32BitsMask) + (byte5 << K24BitsMask)
           + (byte6 << K16BitsMask) + (byte7 << K8BitsMask) + byte8;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::ResolveMapL
 * Resolves the map of the multimedia message. The map of the multimedia
 * message consists of a byte whose bits indicate which of the
 * multimedia message headers are present. See comments of 'DeSerialize' method
 * for more details about the mms map.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::ResolveMapL(RBufReadStream& aReadStream,
        TInt32 aHeaderLengths[])
{
    JELOG2(EWMA);
    TInt8 mmsMap = aReadStream.ReadUint8L();

    // resolve the mms map
    while (true)
    {
        TInt div = Divide(mmsMap, K64BitsMask);
        if (div >= 0)
        {
            // get reply to app id length
            aHeaderLengths[KReplyToAppIDFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        div = Divide(mmsMap, K32BitsMask);
        if (div >= 0)
        {
            // get appID length
            aHeaderLengths[KAppIDFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        div = Divide(mmsMap, K16BitsMask);
        if (div >= 0)
        {
            // get subject length
            aHeaderLengths[KSubjectFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        div = Divide(mmsMap, K8BitsMask);
        if (div >= 0)
        {
            // get the from length
            aHeaderLengths[KFromFieldIndex] = ReadIntL(aReadStream);
        }
        div = Divide(mmsMap, K4BitsMask);
        if (div >= 0)
        {
            // get to length
            aHeaderLengths[KToFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        div = Divide(mmsMap, K2BitsMask);
        if (div >= 0)
        {
            // get bcc length
            aHeaderLengths[KBccFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        if (mmsMap >= 0)
        {
            // get cc length
            aHeaderLengths[KCcFieldIndex] = ReadIntL(aReadStream);
            if (div == 0)
                break;
            mmsMap = div;
        }
        break;
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::ResolveHeadersLC
 * Resolves/reads the headers of the multimedia message from the stream
 * and sets them to the internal representation of the multimedia
 * message
 * -----------------------------------------------------------------------------
 */
CMMSMessageInformation& CJavaMMSMessageHandler::ResolveHeadersLC(
    RBufReadStream& aReadStream, TInt32 aHeaderLengths[])
{
    JELOG2(EWMA);
    // priority
    TInt8 prio = aReadStream.ReadUint8L();
    // date
    TInt64 date = ReadInt64L(aReadStream);
    CMMSMessageInformation* msg = CMMSMessageInformation::NewL(KNullDesC, date,
                                  KNullDesC8, NULL);
    CleanupStack::PushL(msg);
    // reply to appID
    HBufC8* replyToAppID =HBufC8::NewL(aHeaderLengths[KReplyToAppIDFieldIndex]);
    CleanupStack::PushL(replyToAppID);
    TPtr8 replyToAppIDPtr(replyToAppID->Des());
    // appID
    HBufC8* appID = HBufC8::NewL(aHeaderLengths[KAppIDFieldIndex]);
    CleanupStack::PushL(appID);
    TPtr8 appIDPtr(appID->Des());
    // subject
    HBufC8* subject = HBufC8::New(aHeaderLengths[KSubjectFieldIndex]);
    CleanupStack::PushL(subject);
    TPtr8 subjectPtr(subject->Des());
    // from
    HBufC8* from = HBufC8::NewL(aHeaderLengths[KFromFieldIndex]);
    CleanupStack::PushL(from);
    TPtr8 fromPtr(from->Des());
    // to
    HBufC8* to = HBufC8::NewL(aHeaderLengths[KToFieldIndex]);
    CleanupStack::PushL(to);
    TPtr8 toPtr(to->Des());
    // bcc
    HBufC8* bcc = HBufC8::NewL(aHeaderLengths[KBccFieldIndex]);
    CleanupStack::PushL(bcc);
    TPtr8 bccPtr(bcc->Des());
    // cc
    HBufC8* cc = HBufC8::NewL(aHeaderLengths[KCcFieldIndex]);
    CleanupStack::PushL(cc);
    TPtr8 ccPtr(cc->Des());
    for (TInt i = 0; i < KMmsMapLength; i++)
    {
        if (aHeaderLengths[i] > 0)
        {
            // read the first aHeaderLengths[i] bytes
            switch (i)
            {
            case(KReplyToAppIDFieldIndex):
                // reply to app ID
                aReadStream.ReadL(replyToAppIDPtr, aHeaderLengths[i]);
                break;
            case(KAppIDFieldIndex):
                // app ID
                aReadStream.ReadL(appIDPtr, aHeaderLengths[i]);
                break;
            case(KSubjectFieldIndex):
                // subject
                aReadStream.ReadL(subjectPtr, aHeaderLengths[i]);
                break;
            case(KFromFieldIndex):
                // from
                aReadStream.ReadL(fromPtr, aHeaderLengths[i]);
                break;
            case(KToFieldIndex):
                // to
                aReadStream.ReadL(toPtr, aHeaderLengths[i]);
                break;
            case(KBccFieldIndex):
                // bcc
                aReadStream.ReadL(bccPtr, aHeaderLengths[i]);
                break;
            case(KCcFieldIndex):
                // cc
                aReadStream.ReadL(ccPtr, aHeaderLengths[i]);
                break;
            default:
                // Nothing to do
                break;
            }
        }
    }
    // create the CMMSMessageInformation
    msg->SetApplicationIdL(appIDPtr);
    msg->SetReplyToApplicationIdL(replyToAppIDPtr);
    msg->SetMessageSubjectL(subjectPtr);
    switch (prio)
    {
    case KPriorityLow:
        //low
        msg->SetMessagePriority(EMmsPriorityLow);
        break;
    case KPriorityNormal:
        //normal
        msg->SetMessagePriority(EMmsPriorityNormal);
        break;
    case KPriorityHigh:
        //high
        msg->SetMessagePriority(EMmsPriorityHigh);
        break;
    default:
        // Nothing to do
        break;
    }
    // parse the addresses and set them to the message
    ParseAndSetAddressL(EMsvRecipientTo, toPtr, *msg);
    ParseAndSetAddressL(EMsvRecipientBcc, bccPtr, *msg);
    ParseAndSetAddressL(EMsvRecipientCc, ccPtr, *msg);
    // pop the pointers before deleting them
    CleanupStack::PopAndDestroy(cc);
    CleanupStack::PopAndDestroy(bcc);
    CleanupStack::PopAndDestroy(to);
    CleanupStack::PopAndDestroy(from);
    CleanupStack::PopAndDestroy(subject);
    CleanupStack::PopAndDestroy(appID);
    CleanupStack::PopAndDestroy(replyToAppID);
    return *msg;
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::ParseAndSetAddressL
 * Splits the concatenated addresses (";" separated string/descriptor)
 * into sepparate tokens and sets them individually to the internal
 * representation of the multimedia message.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::ParseAndSetAddressL(
    TMsvRecipientTypeValues aAddressType,
    TDes8& aAddr,
    CMMSMessageInformation& aMsg)
{
    JELOG2(EWMA);
    if (aAddr.Length() == 0)
    {
        return;
    }
    else
    {
        while (true)
        {
            TInt off = aAddr.Locate(';');
            if (off != KErrNotFound)
            {
                TPtrC8 left = aAddr.Left(off);
                aMsg.AddAddressL(aAddressType, left);
                TPtrC8 right = aAddr.Right(aAddr.Length() - off - 1);
                aAddr.Copy(right);
            }
            else
            {
                aMsg.AddAddressL(aAddressType, aAddr);
                break;
            }
        }
    }
}

/* -----------------------------------------------------------------------------
 * CJavaMMSMessageHandler::SaveDataToFileL
 * Save data to the D-drive. MMS engine assume that attachments are
 * always files. It also help us to use less RAM.
 * -----------------------------------------------------------------------------
 */
void CJavaMMSMessageHandler::SaveDataToFileL(const TDesC8& aDataArray,
        TDesC8*& aFileName,
        const TDesC8& aDataType)
{
    JELOG2(EWMA);
    //// create a dummy attached file here on the D (temp) drive
    RFile attachementFile;
    TInt err = 0;
    TBuf<KMaxFileName> fileName;
    int i = 1;
    CDocumentHandler* docHandler = CDocumentHandler::NewLC();
    do
    {
        fileName.SetLength(0);
        //fileName.Append(KTempDrive); // Add the drive name
        TChar driveInfo;
        DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRam, driveInfo);
        fileName.Append(driveInfo);
        fileName.Append(KTempDriveExt);
        fileName.Append(KTempMmsAttachmentName); // Add the drive name
        fileName.AppendNum((int) this);
        fileName.AppendNum(i++);
        // we delegate the DocHandler to append the file extension
        TDataType mimeType(aDataType);
        docHandler->CheckFileNameExtension(fileName, mimeType);
        err = attachementFile.Create(mRFs, fileName, EFileWrite
                                     | EFileShareExclusive);
    }
    while (KErrAlreadyExists == err);
    CleanupStack::PopAndDestroy(docHandler);
    if (err != KErrNone)
    {
        User::LeaveIfError(err);
    }
    else
    {
        // Get the file name, so that we can delete it later
        java::util::S60CommonUtils::ConvertWiderToNarrowL(fileName,
                (TDesC8*&) aFileName);

        // add data to the newly created attached file.
        attachementFile.Write(aDataArray);
        attachementFile.Flush();
        attachementFile.Close();
    }
}

} //namespace wma
} //namespace java

