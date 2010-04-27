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
* Description:
 *
*/


// INCLUDE FILES
#include  "cstscmdapdu.h"
#include "stsapduconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{

// CONSTANTS

//Apdu instructions
const TUint8 KSTSINSManageChannel = 0x70;
const TUint8 KSTSINSSelectFile = 0xA4;
const TUint8 KSTSINSDeactivateFile = 0x04;
const TUint8 KSTSINSActivateFile = 0x44;

const TUint8 KSTSP1SelectAID = 0x04;

const TUint8 KSTSP1SelectByFileId = 0x00;
const TUint8 KSTSP1SelectByPathFromMF = 0x08;
const TUint8 KSTSP1SelectByPathFromCurrentDF = 0x09;

const TInt KFileIdLength = 2;

//Lc byte length is one (it is not part of the mandatory header and is not
//part of the real data
const TInt KSTSHeaderAndLcLength = KSTSApduMandatoryHeaderLen + 1;

//Phone application's File Ids on the UICC card
//according to ETSI TS 102 221
_LIT8(KSTSFileIdDFTelecom, "\x7F\x10");
_LIT8(KSTSFileIdDFGSM, "\x7F\x20");

_LIT8(KSTSFileIdDFDCS1800, "\x7F\x21");
_LIT8(KSTSFileIdDFIS_41, "\x7F\x22");
_LIT8(KSTSFileIdDFFP_CTS, "\x7F\x23");
_LIT8(KSTSFileIdDFTIA_EIA_136, "\x7F\x24");
_LIT8(KSTSFileIdDFTIA_EIA_95, "\x7F\x25");
_LIT8(KSTSFileIdDFRFU1, "\x7F\x26"); //Reserved For Future Use
_LIT8(KSTSFileIdDFRFU2, "\x7F\x27");
_LIT8(KSTSFileIdDFRFU3, "\x7F\x28");
_LIT8(KSTSFileIdDFRFU4, "\x7F\x29");
_LIT8(KSTSFileIdDFRFU5, "\x7F\x2A");
_LIT8(KSTSFileIdDFRFU6, "\x7F\x2B");
_LIT8(KSTSFileIdDFRFU7, "\x7F\x2C");
_LIT8(KSTSFileIdDFRFU8, "\x7F\x2D");
_LIT8(KSTSFileIdDFRFU9, "\x7F\x2E");
_LIT8(KSTSFileIdDFRFU10, "\x7F\x2F");

_LIT8(KSTSFileIdDFPDC, "\x7F\x80");
_LIT8(KSTSFileIdDFTETRA, "\x7F\x90");
_LIT8(KSTSFileIdDFIDEN, "\x7F\x31");

//PKCS#15 DF files
//should read from EF(DIR) file (hardcoded at the moment)
_LIT8(KSTSFileIdDFPKCS15Gemplus, "\x7F\x50");
_LIT8(KSTSFileIdDFPKCS15Orga, "\x50\x15");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CSTSCmdApdu
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSCmdApdu::CSTSCmdApdu()
{
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCmdApdu* CSTSCmdApdu::NewLC(TDesC8& aApduBytes,
                                CSTSApdu::TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::NewLC");
    CSTSCmdApdu* self = new(ELeave) CSTSCmdApdu();
    CleanupStack::PushL(self);
    self->ConstructL(aApduBytes, aStandard);
    LOG(ESATSA, EInfo, "-- CSTSCmdApdu::NewLC");
    return self;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSCmdApdu* CSTSCmdApdu::NewL(TDesC8& aApduBytes,
                               CSTSApdu::TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::NewL() called");
    CSTSCmdApdu* self = CSTSCmdApdu::NewLC(aApduBytes, aStandard);
    CleanupStack::Pop(self);
    LOG(ESATSA, EInfo, "-- CSTSCmdApdu::NewL ");
    return self;
}

// Destructor
CSTSCmdApdu::~CSTSCmdApdu()
{
    delete iApduData;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSTSCmdApdu::ConstructL(TDesC8& aApduBytes,
                             CSTSApdu::TSTSApduStandard aStandard)
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::ConstructL");
    iApduData = NULL;
    iApduData = CSTSApdu::NewL(aApduBytes, aStandard);
    if (!iApduData)
    {
        ELOG(ESATSA, "CSTSCmdApdu::ConstructL: failed to create iApduData");
    }
    LOG(ESATSA, EInfo, "-- CSTSCmdApdu::ConstructL");
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckL
// Checks apdu data and leaves with proper error code
// Checking is done according to JSR177 design document.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckL(TInt aUsedChannel) const
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::CheckL");
    CheckApduLengthL();

    TInt8 cla = ApduByte(ESTSCLA);
    TInt8 ins = ApduByte(ESTSINS);

    //contains a card selection APDU
    if ((ins == (TInt8) KSTSINSSelectFile) && (iApduData->ApduPtr()[ESTSP1]
            == KSTSP1SelectAID))
    {
        ELOG(ESATSA, "CSTSCmdApdu::CheckL:  Select AID: IllegalArgument");
        User::Leave(KSTSErrIllegalArgument + KSTSErrIASelectApp);
    }

    //contains a MANAGE CHANNEL command APDU
    if (((cla >= 0x00) || (cla <= 0x0F)) //b1 can be "anything", b2 must be 0
            && (ins == KSTSINSManageChannel))
    {
        ELOG(ESATSA, "CSTSCmdApdu::CheckL: Manage Channel: IllegalArgument");
        User::Leave(KSTSErrIllegalArgument + KSTSErrIAManageChannel);
    }

    //if the channel associated with the connection object is non-zero
    //and the CLA byte has a value other than 0x0X, 0x8X, 0x9X or 0xAX
    if (aUsedChannel != 0)
    {
        TUint8 claMask = 0xF0; //11110000
        TUint8 claLeft = (TUint8)(cla & claMask);  //ignore low order bits
        if ((claLeft != 0x00) && //The coding is according to ISO7816-4
                (claLeft != 0x90) && (claLeft != 0xA0) && //Coded as ISO7816-4 unless stated otherwise
                (claLeft != 0x80)) //Structured as ISO/IEC 7816-4 [12], coding
            //and meaning is defined in ETSI TS 102 221
        {
            ELOG(ESATSA, "CSTSCmdApdu::CheckL: aUsedChannel: IllegalArgument");
            User::Leave(KSTSErrIllegalArgument + KSTSErrIAOtherCLA);
        }
    }
    CheckPhoneAppSelectionL();
    LOG(ESATSA, EInfo, "-- CSTSCmdApdu::CheckL");
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::ApduBytes
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TPtr8& CSTSCmdApdu::ApduBytes() const
{
    return iApduData->ApduPtr();
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckApduLengthL
// Leaves if apdu length is not atleast same than standard apdu header length
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckApduLengthL() const
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::CheckApduLengthL");
    //too short apdu
    TInt apduLength = iApduData->ApduPtr().Length();
    if (apduLength < KSTSApduMandatoryHeaderLen)
    {
        ELOG1(ESATSA, "Too short: KSTSErrIllegalArgument. apdulength is %d",
              apduLength);
        User::Leave(KSTSErrIllegalArgument + KSTSErrIAMalformedAPDU);
    }

    //if data length is not same than Lc
    //note: if there is no data, P3 means Le
    if (apduLength > KSTSHeaderAndLcLength + 1) // must be data or Le
    {
        TInt lengthAfterHeaderAndLc = apduLength - KSTSHeaderAndLcLength;
        TInt lc = ApduByte(ESTSLc);
        ILOG2(ESATSA, "Data length = %d, lc = %d",
              lengthAfterHeaderAndLc, lc);
        if ((lengthAfterHeaderAndLc != lc)
                && (lengthAfterHeaderAndLc - lc != 1)) //there can be one Le byte
        {
            ELOG(ESATSA, "Incorrect data length: KSTSErrIllegalArgument");
            User::Leave(KSTSErrIllegalArgument + KSTSErrIAMalformedAPDU);
        }
    }
    LOG(ESATSA, EInfo, "-- CSTSCmdApdu::CheckApduLengthL");
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::SetChannel
// Adds gived channel to command APDU message's CLA byte
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::SetChannel(TInt aChannel)
{

    // mask for getting cla bytes six first bits
    TUint8 claMask = 0xFC; //11111100
    TUint8 channel = (TUint8) aChannel;
    TUint8 cla = ApduByte(ESTSCLA);

    // take cla byte's six first bits
    cla = (TUint8)(cla & claMask);
    // add channel to cla byte's two last bits
    cla = (TUint8)(cla | channel);

    SetApduByte(cla, ESTSCLA);
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::SetHeader
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::SetHeader(TUint8 aCla, TUint8 aIns, TUint8 aP1, TUint8 aP2)
{

    SetApduByte(aCla, ESTSCLA);
    SetApduByte(aIns, ESTSINS);
    SetApduByte(aP1, ESTSP1);
    SetApduByte(aP2, ESTSP2);
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::Header
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
const TDesC8& CSTSCmdApdu::Header()
{
    iHeaderPtr.Set(iApduData->ApduPtr().Mid(0, KSTSApduMandatoryHeaderLen));
    return iHeaderPtr;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::AppendDatabytesL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::AppendDatabytesL(const TDesC8& aDataBytes)
{
    TInt dataLength = aDataBytes.Length();

    //headerlength + Lc + Data
    TInt maxLength = KSTSApduMandatoryHeaderLen + 1 + dataLength;
    iApduData->ReAllocL(maxLength);
    iApduData->ApduPtr().Append((TUint8) dataLength); //Le
    iApduData->ApduPtr().Append(aDataBytes); //data
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::SetLe
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::SetLe(TUint8 aLe)
{
    TInt lenght = iApduData->ApduPtr().Length();

    //last byte is Le
    iApduData->ApduPtr()[lenght - 1] = aLe;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::ApduByte
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TUint8 CSTSCmdApdu::ApduByte(TSTSApduByteType aByteType) const
{

    return iApduData->ApduPtr()[aByteType];
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::SetApduByte
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::SetApduByte(TUint8 aByte, TSTSApduByteType aByteType)
{
    iApduData->ApduPtr()[aByteType] = aByte;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckPhoneAppSelectionL
// Checks if used apdu leads to selection of application/file or
// other restricted file which is not allowed to be selected. Leaves with
// security leave if it is true.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckPhoneAppSelectionL() const
{
    LOG(ESATSA, EInfo, "+ CSTSCmdApdu::CheckPhoneAppSelectionL, called");
    TInt dataLength = iApduData->ApduPtr().Length() - KSTSHeaderAndLcLength;
    LOG1(ESATSA, EInfo, "CSTSCmdApdu::CheckPhoneAppSelectionL, dataLength: %d", dataLength);

    //Checks does apdu lead to file selection
    if (IsProperFileSelectionApdu(dataLength))
    {
        LOG(ESATSA, EInfo, "is file selection apdu");
        //take data part from the apdu
        const TPtrC8 data(iApduData->ApduPtr().Ptr() + KSTSHeaderAndLcLength,
                          dataLength);

        //check that possible file id does not contain forbidden file id
        CheckFileIdL(data);
        //check that possible path does not contain forbidden file ids
        CheckPathL(data);
    }
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckIllegalFileIdL
// Checks is file id forbidden or not. Leaves if it is forbidden.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckIllegalFileIdL(const TDesC8& aFileId) const
{
    LOG(ESATSA, EInfo, "CSTSCmdApdu::CheckIllegalFileIdL, called");
    //file id length should be 2


    if ((aFileId == KSTSFileIdDFTelecom()) || (aFileId == KSTSFileIdDFGSM())
            || (aFileId == KSTSFileIdDFDCS1800()) || (aFileId
                    == KSTSFileIdDFIS_41()) || (aFileId == KSTSFileIdDFFP_CTS())
            || (aFileId == KSTSFileIdDFTIA_EIA_136()) || (aFileId
                    == KSTSFileIdDFTIA_EIA_95()) || (aFileId == KSTSFileIdDFRFU1())
            || (aFileId == KSTSFileIdDFRFU2()) || (aFileId
                                                   == KSTSFileIdDFRFU3()) || (aFileId == KSTSFileIdDFRFU4())
            || (aFileId == KSTSFileIdDFRFU5()) || (aFileId
                                                   == KSTSFileIdDFRFU6()) || (aFileId == KSTSFileIdDFRFU7())
            || (aFileId == KSTSFileIdDFRFU8()) || (aFileId
                                                   == KSTSFileIdDFRFU9()) || (aFileId == KSTSFileIdDFRFU10())
            || (aFileId == KSTSFileIdDFPDC()) || (aFileId
                                                  == KSTSFileIdDFTETRA()) || (aFileId == KSTSFileIdDFIDEN())
            || (aFileId == KSTSFileIdDFPKCS15Gemplus()) || (aFileId
                    == KSTSFileIdDFPKCS15Orga()))
    {
        ELOG(ESATSA, "CSTSCmdApdu::CheckIllegalFileIdL, Is illegal!");
        User::Leave(KSTSErrSecurity + KSTSErrSECIllegalFileID);
    }
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::IsProperFileSelectionApdu
// Checks does apdu lead to file selection. SELECT FILE, DEACTIVATE FILE and
// ACTIVATE FILE apdu will lead to file selection in case of successful
// execution of the command,
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TBool CSTSCmdApdu::IsProperFileSelectionApdu(TInt aDataLength) const
{
    LOG(ESATSA, EInfo, "CSTSCmdApdu::IsProperFileSelectionApdu, called");

    TBool returnValue = EFalse;
    //Check is it SELECT FILE apdu and contains data field
    if (ApduByte(ESTSINS) == KSTSINSSelectFile && aDataLength > 0)
    {
        returnValue = ETrue;
    }
    //Check is it DEACTIVATE FILE apdu and contains data field
    else if (ApduByte(ESTSINS) == KSTSINSDeactivateFile && aDataLength > 0)
    {
        returnValue = ETrue;
    }
    //Check is it ACTIVATE apdu and contains data field
    else if (ApduByte(ESTSINS) == KSTSINSActivateFile && aDataLength > 0)
    {
        returnValue = ETrue;
    }

    return returnValue;
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckPathL
// Checks is there proper path and does it contain illegal file ids.
// Leaves if there is illegal file ids.
// Note that apdu must be SELECT FILE, DEACTIVATE FILE or ACTIVATE FILE apdu.
// That can be checked with IsProperFileSelectionApdu method. P1 byte of those
// apdus are the same in select by path from MF and select by path from current
// DF case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckPathL(const TDesC8& aData) const
{
    TUint8 p1 = ApduByte(ESTSP1);
    TInt dataLength = aData.Length();

    //check that path does not contain forbidden ids
    if ((p1 == KSTSP1SelectByPathFromMF) || (p1
            == KSTSP1SelectByPathFromCurrentDF))
    {
        LOG(ESATSA, EInfo, "CSTSCmdApdu::CheckPathL, is Path");
        TInt remainder;
        TInt tmpDataLength = dataLength;
        I64DIVMOD(tmpDataLength, KFileIdLength, remainder);
        //if data field does not contain concatenation of file ids
        if (remainder != 0)
        {
            ELOG(ESATSA, "CSTSCmdApdu::CheckPathL, Leave!!");
            User::Leave(KSTSErrIllegalArgument + KSTSErrIAMalformedAPDU);
        }
        else
        {
            LOG(ESATSA, EInfo, "CSTSCmdApdu::CheckPathL, go through in loop");
            //go through in loop all file ids in path
            for (TInt i = 0; i < dataLength; i += KFileIdLength)
            {
                TPtrC8 currentFid(aData.Ptr() + i, KFileIdLength);
                CheckIllegalFileIdL(currentFid);
            }
        }
    }
    //else no actions
}

// -----------------------------------------------------------------------------
// CSTSCmdApdu::CheckFileIdL
// Checks is there proper file id and is it illegal. Leaves if there is illegal
// file id.
// Note that apdu must be SELECT FILE, DEACTIVATE FILE or ACTIVATE FILE apdu.
// That can be checked with IsProperFileSelectionApdu method. INS byte of those
// apdus are the same in select by file id case.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CSTSCmdApdu::CheckFileIdL(const TDesC8& aData) const
{
    TUint8 p1 = ApduByte(ESTSP1);
    TInt dataLength = aData.Length();

    //Check is file id used
    if (p1 == KSTSP1SelectByFileId)
    {
        //if data field does not contain file id, leave
        if (dataLength != KFileIdLength)
        {
            ELOG(ESATSA, "CSTSCmdApdu::CheckFileIdL, Leave !!");
            User::Leave(KSTSErrIllegalArgument + KSTSErrIAMalformedAPDU);
        }
        else
        {
            //check that file id is not forbidden
            CheckIllegalFileIdL(aData);
        }
    }
    //else no actions
}

} // namespace satsa
} // namespace java
//  End of File

