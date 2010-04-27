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


#ifndef CJAVAMMSMESSAGEHANDLER_H
#define CJAVAMMSMESSAGEHANDLER_H

#include <jni.h>
#include <string>
#include <s32mem.h>
#include <s32strm.h>
#include <e32base.h>
#include <e32const.h>
#include <charconv.h>
#include "cmmsmessageinformation.h"

namespace java
{
namespace wma
{

const TInt KPriorityLow = 1;
const TInt KPriorityNormal = 2;
const TInt KPriorityHigh = 3;

const TInt K2BitsMask = 2;
const TInt K4BitsMask = 4;
const TInt K8BitsMask = 8;
const TInt K16BitsMask = 16;
const TInt K24BitsMask = 24;
const TInt K32BitsMask = 32;
const TInt K40BitsMask = 40;
const TInt K48BitsMask = 48;
const TInt K56BitsMask = 56;
const TInt K64BitsMask = 64;

class CJavaMMSMessageHandler:public CBase
{
public:
    //Destructor
    virtual ~CJavaMMSMessageHandler();

    static CJavaMMSMessageHandler* NewL(RFs aFs);
    /**
     * Deserializes a byte array containing a multimedia message to be sent
     * into an internal representation of the multimedia message.
     *
     * @param aMMS A Buffer containing the multimedia message in a byte
     * array representation
     * @return The internal representation of the multimedia message to be
     * sent
     */
    CMMSMessageInformation& DeSerializeL(CBufFlat& aMMS);
    /**
     * Serializes an internal representation of the multimedia message into
     * a byte array representation.
     *
     * @param aMMS A Buffer containing the multimedia message in the
     * internal representation
     * @return The byte array representation of the multimedia message
     */
    CBufFlat& SerializeLC(CMMSMessageInformation& aMMS);

private:
    //Constructor
    CJavaMMSMessageHandler(RFs aRFs);
    /**
     * Returns the rest of dividing aNum1 with aNum2.
     *
     * @param aNum1 The number to divide
     * @param aNum2 The number to be divided with
     * @return The rest of dividing aNum1 with aNum2
     */
    int Divide(int aNum1, int aNum2);
    /**
     * Reads an integer as 4 bytes from the read stream.
     *
     * @param aStream The stream containing a byte array
     * @return An integer read as 4 bytes
     */
    TInt32 ReadIntL(RBufReadStream& aStream);
    /**
     * Reads an integer as 8 bytes from the read stream.
     *
     * @param aStream The stream containing a byte array
     * @return An integer read as 8 bytes
     */
    TInt64 ReadInt64L(RBufReadStream& aStream);
    /**
     * Resolves the map of the multimedia message. The map of the multimedia
     * message consists of a byte whose bits indicate which of the
     * multimedia message headers are present. See implementation comments
     * for more details.
     *
     * @param aStream The stream containing the multimedia message
     * @param aHeaderLengths The lengths of the multimedia message
     * headers
     */
    void ResolveMapL(RBufReadStream& aStream, TInt32 aHeaderLengths[]);
    /**
     * Resolves/reads the headers of the multimedia message from the stream
     * and sets them to the internal representation of the multimedia
     * message
     *
     * @param aStream The stream containing the multimedia message
     * @param aHeaderLengths The lengths of the multimedia message
     * headers
     * @return The internal representation of the multimedia message with
     * the headers being set
     */
    CMMSMessageInformation& ResolveHeadersLC(RBufReadStream& aStream,
            TInt32 aHeaderLengths[]);
    /**
     * Splits the concatenated addresses (";" separated string/descriptor)
     * into sepparate tokens and sets them individually to the internal
     * representation of the multimedia message.
     *
     * @param aAddressType The address type. Possible values are:
     * EMsvRecipientTo, EMsvRecipient and BccEMsvRecipientCc
     * @param aMsg The internal representation of the multimedia
     * message where the sepparate addresses will be set to
     */
    void ParseAndSetAddressL(TMsvRecipientTypeValues aAddressType,
                             TDes8& aAddr, CMMSMessageInformation& aMsg);
    /**
     * Save data to the D-drive. MMS engine assume that attachments are
     * always files. It also help us to use less RAM.
     *
     * @param aDataArray
     * @return TDesC* FileName
     */
    void SaveDataToFileL(const TDesC8& aDataArray, TDesC8*& aFileName,
                         const TDesC8& aDataType);

    /**
     * Concatenates all the addresses/recipients into a ";" separated
     * string/descriptor.
     *
     * @param aAddressType The address type. Possible values are:
     * EMsvRecipientTo, EMsvRecipient and BccEMsvRecipientCc
     * @param aMMS The internal representation of the multimedia
     * message containing the addresses
     */
    HBufC8& BuildAddressLC(TMsvRecipientTypeValues aAddressType,
                           CMMSMessageInformation& aMMS);

    /**
     * Calculates the mms map. See the implementation for more details
     * about the mms map.
     *
     * @param aReplyToAppID The sender's application identifier
     * @param aAppID The application identifier
     * @param aSubject The subject
     * @param aFrom The 'from' address
     * @param aTo The 'to' address
     * @param aBcc The 'bcc' address
     * @param aCc The 'cc' address
     * @return The mms map
     */
    TInt CalculateMap(TDesC8& aReplyToAppID, TDesC8& aAppID, TDesC8& aSubject,
                      TDesC8& aFrom, HBufC8& aTo, HBufC8& aBcc, HBufC8& aCc);

    /**
     * Writes an integer as 8 bytes into the write stream.
     *
     * @param aStream The stream where the integer will be written to
     * @param aInteger The integer to be written
     */
    void WriteInt64L(RBufWriteStream& aStream, const TInt64 aInteger);

    /**
     * Writes an integer as 4 bytes into the write stream.
     *
     * @param aStream The stream where the integer will be written to
     * @param aInteger The integer to be written
     */
    void WriteIntL(RBufWriteStream& aStream, const TInt32 aInteger);

    /**
     * Writes the lengths of all attachments into the write stream.
     *
     * @param aStream The stream where the attachment lengths will be
     * written to
     * @param aMMS The internal representation of the multimedia
     * message containing all the attachments to be written
     * @param aConv The symbian's character converter used for translating
     * the encoding from the MIBEnum into the text representation
     * @return The index (within the aMMS) of the first attachment
     * (if such an attachment has been set to be the first one), or -1 if
     * none of the attachments has been set to be the first one
     */
    TInt WriteAttachmentsLengthsL(RBufWriteStream& aStream,
                                  CMMSMessageInformation& aMMS, CCnvCharacterSetConverter& aConv);

    /**
     * Writes the lengths of an attachment into the write stream.
     *
     * @param aStream The stream where the attachment lengths will be
     * written to
     * @param aAttch The attachment whose lengths are to be written
     * @param aConv The symbian's character converter used for translating
     * the encoding from the MIBEnum into the text representation
     */
    void WriteAttachmentLengthsL(RBufWriteStream& aStream,
                                 CMMSMessageAttachment& aAttch, CCnvCharacterSetConverter& aConv);

    /**
     * Writes all the attachments into the write stream.
     *
     * @param aStream The stream where the attachments will be written to
     * @param aMMS The internal representation of the multimedia
     * message containing all the attachments to be written
     * @param aConv The symbian's character converter used for translating
     * the encoding from the MIBEnum into the text representation
     * @param aFirstAttchmt The index of the first attachment to be written
     * or -1 if the attachments could be written in an arbitrary order
     */
    void WriteAttachmentsL(RBufWriteStream& aStream,
                           CMMSMessageInformation& aMMS, CCnvCharacterSetConverter& aConv,
                           TInt aFirstAttchmt);

    /**
     * Writes an attachment into the write stream.
     *
     * @param aStream The stream where the attachment will be written to
     * @param aAttch The attachment which will be written
     * @param aConv The symbian's character converter used for translating
     * the encoding from the MIBEnum into the text representation
     */
    void WriteAttachmentL(RBufWriteStream& aStream,
                          CMMSMessageAttachment& aAttch, CCnvCharacterSetConverter& aConv);

    /**
     * Writes a descriptor (if not NULL) into the write stream.
     *
     * @param aStream The stream where the descriptor will be written to
     * @param aDes The descriptor which will be written
     */
    void WriteDes8L(RBufWriteStream& aStream, const TDesC8& aDes);

    /**
     * Writes a descriptor's length (if the descriptor is not NULL)
     *
     * @param aStream The stream where the descriptor's length will be
     * written to
     * @param aDes The descriptor whose length will be written
     */
    void WriteDes8LengthL(RBufWriteStream& aStream, const TDesC8& aDes);

    /**
     * Writes a descriptor's length (if the descriptor is not NULL) or zero
     * (if the descriptor is NULL)
     *
     * @param aStream The stream where the descriptor's length will be
     * written to
     * @param aDes The descriptor whose length will be written
     */
    void WriteDes8LengthOrZeroL(RBufWriteStream& aStream, const TDesC8& aDes);

    /**
     * Writes a descriptor's length (if the descriptor is not NULL) or zero
     * (if the descriptor is NULL)
     *
     * @param aStream The stream where the descriptor's length will be
     * written to
     * @param aDes The descriptor whose length will be written
     */
    void WriteDesLengthOrZeroL(RBufWriteStream& aStream, const TDesC8& aDes);

private:

    // handle to file server
    RFs mRFs;
};
} //namespace wma
} //namespace java
#endif // CJAVAMMSMESSAGEHANDLER_H
