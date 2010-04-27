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


//  INCLUDE FILES
#include <charconv.h>
#include <secdlg.h>

#include "cstsapduexchanger.h"
#include "cstsaccesscontrol.h"
#include "cstspinmanager.h"
#include "cstsauthtype.h"
#include "cstspinattributes.h"
#include "stsapduconstants.h"
#include "cstspinapdu.h"
#include "cstspinapduresp.h"
#include "cstsresphandler.h"
#include "cstspinconverter.h"
#include "logger.h"


using namespace java::util;

namespace java
{
namespace satsa
{

//  CONSTANTS
const TInt KSTSMaxPinAttempts = 3;
const TInt KSTSPinHeaderLength = 4;

CSTSPinManager::CSTSPinManager(CSTSApduExchanger* aApduExchanger,
                               CSTSAccessControl* aAccessControl, MSTSRespHandler* aRespHandler) :
        CActive(EPriorityNormal)
{

    CActiveScheduler::Add(this);
    iApduExchanger = aApduExchanger;
    iAccessControl = aAccessControl;
    iRespHandler = aRespHandler;

}

void CSTSPinManager::ConstructL()
{
    // create pinUI
    iSecurityDialog = SecurityDialogFactory::CreateL();

    iAuthTypeFirstPin = CSTSAuthType::NewL();
    iAuthTypeSecondPin = CSTSAuthType::NewL();

}

CSTSPinManager* CSTSPinManager::NewL(JNIEnv* aJni, jobject aPeer,
                                     CSTSApduExchanger* aApduExchanger, CSTSAccessControl* aAccessControl,
                                     MSTSRespHandler* aRespHandler)
{
    CSTSPinManager
    * self =
        new(ELeave) CSTSPinManager(aApduExchanger, aAccessControl, aRespHandler);

    self->mJni = aJni;
    self->mPeer = aPeer;
    CleanupStack::PushL(self);
    self->ConstructL();

    CleanupStack::Pop(self);
    return self;
}

// Destructor
CSTSPinManager::~CSTSPinManager()
{
    Cancel();
    delete iAuthTypeSecondPin;
    delete iAuthTypeFirstPin;
    if (iSecurityDialog)
    {
        iSecurityDialog->Release(); // Release deletes the object
    }
    delete iPinApdu;
    delete iPinApduResp;
    delete iResponseBuf;
    delete mJni;
}

// -----------------------------------------------------------------------------
// CSTSPinManager::DisablePinL
// Calls proper private methods.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DisablePinL(TInt aPinID)
{
    PreparePinMethodL(CSTSPinApdu::ESTSDisablePin, aPinID);
    ShowPinUI(CSTSPinApdu::ESTSDisablePin);
}

// -----------------------------------------------------------------------------
// CSTSPinManager::ChangePinL
// Calls proper private methods.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::ChangePinL(TInt aPinID)
{

    PreparePinMethodL(CSTSPinApdu::ESTSChangePin, aPinID);
    ShowPinUI(CSTSPinApdu::ESTSChangePin);

}

// -----------------------------------------------------------------------------
// CSTSPinManager::EnablePinL
// Calls proper private methods.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::EnablePinL(TInt aPinID)
{

    PreparePinMethodL(CSTSPinApdu::ESTSEnablePin, aPinID);
    ShowPinUI(CSTSPinApdu::ESTSEnablePin);

}

// -----------------------------------------------------------------------------
// CSTSPinManager::EnterPinL
// Calls proper private methods.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::EnterPinL(TInt aPinID)
{
    PreparePinMethodL(CSTSPinApdu::ESTSVerifyPin, aPinID);
    ShowPinUI(CSTSPinApdu::ESTSVerifyPin);
}

// -----------------------------------------------------------------------------
// CSTSPinManager::UnblockPinL
// Calls proper private methods.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::UnblockPinL(TInt aBlockedPinID, TInt aUnblockedPinID)
{
    PreparePinMethodL(CSTSPinApdu::ESTSUnblockPin, aBlockedPinID,
                      aUnblockedPinID);
    ShowPinUI(CSTSPinApdu::ESTSUnblockPin);
}

// -----------------------------------------------------------------------------
// CSTSPinManager::CancelOperation
// Cancels any pending operation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::CancelOperation()
{
    LOG(ESATSA, EInfo, "CSTSPinManager::CancelOperation called!");
    Cancel();
}

// -----------------------------------------------------------------------------
// CSTSPinManager::RunL
// If PIN value is got from UI, converts user gived PIN values in proper form
// and sends PIN apdu to card. If user has selected cancel option, informs it
// to java side. When apdu sending has returned, checks was user gived value
// correct one and if was not, shows proper PIN ui again. If no retries is left
// shows blocked PIN UI. Supplies response apdus to java side and in error
// conditions supplies proper error code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::RunL()
{
    LOG1(ESATSA, EInfo, "CSTSPinManager::RunL called!:%d", iStatus.Int());

    if (iStatus == KErrCancel)
    {
        LOG(ESATSA, EInfo, "CSTSPinManager::RunL, Cancel selected");
        // if the user selected cancel option. we return null
        iRespHandler->OperationComplete(mJni, mPeer, NULL, KSTSErrCancel);

    }
    else if (iStatus != KErrNone)
    {
        LOG(ESATSA, EInfo, "CSTSPinManager::RunL, Error Occurred!!");
        // in other error conditions supply error code
        iRespHandler->OperationComplete(mJni, mPeer, NULL, iStatus.Int());
    }
    else // no problems
    {
        switch (iState)
        {
        case ESendingPINApdu:
        {
            LOG(ESATSA, EInfo, "CSTSPinManager::RunL, case ESendingPINApdu");
            DoAfterSendApduL();
            break;
        }
        case EShowingPinBlockedUI:
        {
            LOG(ESATSA, EInfo, "CSTSPinManager::RunL, case EShowingPinBlockedUI");
            //return PIN method blocked value to java side
            //that value is already got from card
            DeliverResponseL();
            break;
        }
        case EShowing1ValuePinUI:
        {
            LOG(ESATSA, EInfo, "CSTSPinManager::RunL, case EShowing1ValuePinUI");
            DoAfter1ValuePinUIL();
            break;
        }
        case EShowing2ValuePinUI:
        {
            LOG(ESATSA, EInfo, "CSTSPinManager::RunL, case EShowing2ValuePinUI");
            DoAfter2ValuePinUIL();
            break;
        }
        default:
        {
            break;
        }
        }
    }
}
// -----------------------------------------------------------------------------
// CSTSPinManager::DoCancel
// Cancels all possible active actions like PIN UIs or apdu exchange
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DoCancel()
{
    LOG(ESATSA, EInfo, "CSTSPinManager::DoCancel, called");
    iSecurityDialog->Cancel();
    iApduExchanger->CancelExchange();
}

// -----------------------------------------------------------------------------
// CSTSPinManager::RunError
// If leave occures in RunL,this method is called. Notifies java side with
// leave code.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSTSPinManager::RunError(TInt aError)
{
    LOG1(ESATSA, EInfo, "CSTSPinManager::RunError, called: %d", aError);
    iRespHandler->OperationComplete(mJni, mPeer, NULL, aError);
    // must return KErrNone
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CSTSPinManager::ConvertPINL Converts PIN to correct form for presenting it
// to card. Implementation done according to document "PKCS #15 v1.1:
// Cryptographic Token Information Syntax Standard" chapter "6.8.2.1
// Transforming a supplied PIN".
//
// EXAMPLE (ascii-) Numeric PIN ‘1234’(10), stored length 8 bytes, and padding
//  character ‘FF’(16) gives that the value presented to the card will be
//  ‘31323334FFFFFFFF’(16)
//
// Returns: aConvertedPIN: PIN converted to needed format.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::ConvertPINL(const CSTSPinAttributes& aPinAttributes,
                                 const TDes& aPinValue, HBufC8*& aConvertedPIN)
{
    LOG(ESATSA, EInfo, "CSTSPinManager::ConvertPINL called ");

    TPtr8 convertedPtr(aConvertedPIN->Des());

    CSTSPinAttributes::TPinType pinType = aPinAttributes.PinType();
    LOG1(ESATSA, EInfo, "CSTSPinManager::ConvertPINL, pintype: %d", pinType);

    if (pinType == CSTSPinAttributes::EUTF8)
    {
        // if case-sensitive bit is off, convert to uppercase
        TBool upperCase = !aPinAttributes.IsPinFlagSet(
                              CSTSPinAttributes::ECaseSensitive);
        CSTSPinConverter::ConvertToUTF8L(aPinValue, convertedPtr, upperCase);
    }
    else if (pinType == CSTSPinAttributes::EBCD)
    {
        CSTSPinConverter::ConvertToBCDL(aPinValue, convertedPtr, EFalse); // not half BCD
    }
    else if (pinType == CSTSPinAttributes::EAsciiNumeric || pinType
             == CSTSPinAttributes::EISO9564_1)
    {
        CSTSPinConverter::ConvertToASCIIL(aPinValue, convertedPtr);

    }
    else if (pinType == CSTSPinAttributes::EHalfNibbleBCD)
    {
        CSTSPinConverter::ConvertToBCDL(aPinValue, convertedPtr, ETrue); // Half BCD
    }
    else
    {
        User::Leave(KErrCorrupt);
    }

    // If indicated in the pinFlags field, pad to the right with the
    // padding character, to stored length


    // must be popped from cleanupstack before padding
    CleanupStack::Pop(aConvertedPIN);
    TInt err = CSTSPinConverter::DoBadding(aPinValue, aConvertedPIN,
                                           aPinAttributes.StoredLength(), aPinAttributes.PadChar()[0]);
    CleanupStack::PushL(aConvertedPIN);
    User::LeaveIfError(err);

}

// -----------------------------------------------------------------------------
// CSTSPinManager::PreparePinMethodL Makes preparation common for all PIN
// methods. Checks, is PIN method allowed to use. Gets PIN headers from the
// card and if there was not any, generates it. Sets needed PIN UI parameters.
// Returns:
//      iPinApdu: Saves APDU into this member variable
//      iPinApduResp: Saves response APDU into this member variable
//      iAuthTypeFirstPin: Saves PIN attributes into this member variable
//      iAuthTypeSecondPin: Saves PIN attributes into this member variable
//      iPINParamsFirstPin: Saves PIN UI parameters into this member variable
//      iPINParamsSecondPin: Saves PIN UI parameters into this member variable
// -----------------------------------------------------------------------------
//
void CSTSPinManager::PreparePinMethodL(
    CSTSPinApdu::TSTSPinApduType aPinApduType, TInt aFirstPinID,
    TInt aUnblockedPinID)
{
    LOG(ESATSA, EInfo, "CSTSPinManager::PreparePinMethodL called");

    iPinApduType = aPinApduType;

    // is pinmethods supported to use with card application
    if (!iAccessControl->IsAllowedPinMethod())
    {
        ELOG(ESATSA, "CSTSPinManager::PreparePinMethodL, Not supported");
        User::Leave(KSTSErrSecurity + KSTSErrSECNoRightsPin);
    }

    // gets the pin apdu header part from the card, if there is any
    TPtrC8 headerPtr(iAccessControl->GetPinHeaderL(aFirstPinID,
                     (TPinApduType) aPinApduType));

    iAuthTypeFirstPin->CopyL(iAccessControl->GetPinAttributesL(aFirstPinID));

    // leaves if PIN flags does not allow this PIN apdu
    iAccessControl->CheckPinFlagsL((TPinApduType) aPinApduType);

    // if pin headers are not found,
    if (headerPtr == KNullDesC8())
    {
        // Pin reference is used if the APDU headers are not specified in the ACL
        CSTSPinApdu* pinApdu = CSTSPinApdu::NewL(aPinApduType,
                               iAuthTypeFirstPin->PinAttributes().PinReference(),
                               CSTSApdu::ESTSUICC);
        delete iPinApdu;
        iPinApdu = pinApdu;
    }
    else
    {
        LOG(ESATSA, EInfo, "CSTSPinManager::PreparePinMethodL, specified header used");
        CSTSCmdApdu* tmp = CSTSCmdApdu::NewL(headerPtr, CSTSApdu::ESTSUICC);
        delete iPinApdu;
        iPinApdu = tmp;
    }

    // check is the J2ME application allowed to use this apdu
    if (!iAccessControl->IsAllowedApduL(iPinApdu->Header()))
    {
        ELOG(ESATSA, "CSTSPinManager::PreparePinMethodL, Not allowed");
        // if not allowed
        User::Leave(KSTSErrSecurity + KSTSErrSECIllegalApdu);
    }

    SetPINUIParametersL(iAuthTypeFirstPin, iPINParamsFirstPin);

    // In unblockPIN case we need to initialize parameters of unblocked PIN
    if (aPinApduType == EUnblockPinAPDU)
    {
        LOG(ESATSA, EInfo, "unblock parameters");
        iAuthTypeSecondPin->CopyL(iAccessControl->GetPinAttributesL(
                                      aUnblockedPinID));
        SetPINUIParametersL(iAuthTypeSecondPin, iPINParamsSecondPin);
    }

    iRetries = EFirstTime;
    LOG(ESATSA, EInfo, "-- CSTSPinManager::PreparePinMethodL");
}

// -----------------------------------------------------------------------------
// CSTSPinManager::ShowPinUI Shows PIN UI for getting PIN value or values.
// Shows correct UI depending on which PIN method is used.
//
// -----------------------------------------------------------------------------
//
void CSTSPinManager::ShowPinUI(CSTSPinApdu::TSTSPinApduType aPinApduType)
{
    LOG(ESATSA, EInfo, "CSTSPinManager::ShowPinUI+");
    TBool retrying = iRetries == ERetrying;
    TPinManagerState state = EShowing1ValuePinUI;
    // get PIN from the user with UI
    if (aPinApduType == EVerifyPinAPDU)
    {
        iSecurityDialog->EnterPIN(iPINParamsFirstPin, retrying,
                                  iPinValueForFirstPin, iStatus);
    }
    else if (aPinApduType == EEnablePinAPDU)
    {
        iSecurityDialog->EnablePIN(iPINParamsFirstPin, retrying,
                                   iPinValueForFirstPin, iStatus);

    }
    else if (aPinApduType == EDisablePinAPDU)
    {
        iSecurityDialog->DisablePIN(iPINParamsFirstPin, retrying,
                                    iPinValueForFirstPin, iStatus);
    }
    else if (aPinApduType == EChangePinAPDU)
    {
        state = EShowing2ValuePinUI;
        iSecurityDialog->ChangePIN(iPINParamsFirstPin, retrying,
                                   iPinValueForFirstPin, iPinValueForSecondPin, iStatus);
    }
    else if (aPinApduType == EUnblockPinAPDU)
    {
        state = EShowing2ValuePinUI;
        iSecurityDialog->UnblockPIN(iPINParamsFirstPin, iPINParamsSecondPin,
                                    retrying, iPinValueForFirstPin, iPinValueForSecondPin, iStatus);
    }
    else
    {

    }

    SetActive();
    iState = state;

}

// -----------------------------------------------------------------------------
// CSTSPinManager::DeliverResponseL
// Saves responsebytes to member variable and delivers them to java side with
// help of response handler.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DeliverResponseL()
{
    LOG(ESATSA, EInfo, "CSTSPinManager::DeliverResponseL, called");
    HBufC8* tmp = iPinApduResp->ResponseBytes().AllocL();
    delete iResponseBuf;
    iResponseBuf = tmp;

    iRespHandler->OperationComplete(mJni, mPeer, iResponseBuf, KErrNone);

}

// -----------------------------------------------------------------------------
// CSTSPinManager::SetPINUIParametersL
// Uses authtype parameter to get needed values and sets them to gived PINParams
// variable.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::SetPINUIParametersL(const CSTSAuthType* aAuthType,
        TPINParams& aPINParams)
{
    // maximum length is optional in card, so we need to set something for
    // PIN UI needs
    TInt maxLength = PINMaxLength(aAuthType);
    TInt minLength = aAuthType->PinAttributes().MinLength();

    // create TPINParams

    // label that identifies the PIN
    aPINParams.iPINLabel = aAuthType->Label();
    aPINParams.iTokenLabel = iAccessControl->TokenLabelL();
    aPINParams.iMinLength = minLength;
    aPINParams.iMaxLength = maxLength;

}

// -----------------------------------------------------------------------------
// CSTSPinManager::DoAfterSendApduL
// Checks response adpu and depending on response delivers response bytes
// to response handler, shows PIN Blocked UI or shows previous PIN UI again.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DoAfterSendApduL()
{
    //check response apdu, and show UI again if wrong pin value
    CSTSPinApduResp* resp = CSTSPinApduResp::NewL();
    delete iPinApduResp;
    iPinApduResp = resp;
    iApduExchanger->GetResponseBytes(*iPinApduResp);

    TInt retriesLeft = iPinApduResp->RetriesLeft();

    //if response not indicate retresleft value
    if (retriesLeft == KErrNotFound)
    {
        // return response bytes to java side
        DeliverResponseL();
    }

    //if pin is blocked, call iSecurityDialog::PINBlocked method
    else if (iPinApduResp->IsBlocked())
    {
        // save state to member variable that we can return asynchronous
        // after SetActive call
        iRetries = EPINBlocked;

        iSecurityDialog->PINBlocked(iPINParamsFirstPin, iStatus);
        iState = EShowingPinBlockedUI;
        SetActive();
    }
    // if user gived pin value was incorrect
    else if (retriesLeft < KSTSMaxPinAttempts && retriesLeft > 0)
    {
        iRetries = ERetrying;
        // "initialize" header
        iPinApdu->ApduBytes().SetLength(KSTSPinHeaderLength);
        // show UI again, because PIN value was wrong
        ShowPinUI(iPinApduType);
    }
    // if max pin attemps is left, user gived pin value was correct
    // one and we can supply response bytes to java side
    else if (retriesLeft == KSTSMaxPinAttempts)
    {
        // return response bytes to java side
        DeliverResponseL();
    }
    else
    {
        User::Leave(KSTSErrIO + KSTSErrIOCommProblems);
    }
}

// -----------------------------------------------------------------------------
// CSTSPinManager::DoAfter1ValuePinUIL
// Convers PIN value gor from UI to proper form and sends proper PIN apdu to
// smart card.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DoAfter1ValuePinUIL()
{
    HBufC8* convertedBuf =
        HBufC8::NewLC(PINMaxLength(iAuthTypeFirstPin, ETrue));

    ConvertPINL(iAuthTypeFirstPin->PinAttributes(), iPinValueForFirstPin,
                convertedBuf);
    TPtr8 convertedPin(convertedBuf->Des());

    iPinApdu->AppendDatabytesL(convertedPin);

    CleanupStack::PopAndDestroy(convertedBuf);

    iApduExchanger->ExchangeApduAsync(iStatus, *iPinApdu);
    iState = ESendingPINApdu;
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSPinManager::DoAfter2ValuePinUIL
// Convers PIN values gor from UI to proper form and sends proper PIN apdu to
// smart card.
// -----------------------------------------------------------------------------
//
void CSTSPinManager::DoAfter2ValuePinUIL()
{

    // in changePIN case we use same pin parameters for both pins
    if (iPinApduType == EChangePinAPDU)
    {
        iAuthTypeSecondPin->CopyL(*iAuthTypeFirstPin);
    }

    // finds maximum length to create large enough buffer
    TInt pinValue2MaxLength = PINMaxLength(iAuthTypeSecondPin, ETrue);

    HBufC8* convertedBuf1 = HBufC8::NewLC((PINMaxLength(iAuthTypeFirstPin,
                                           ETrue)) +pinValue2MaxLength);

    ConvertPINL(iAuthTypeFirstPin->PinAttributes(), iPinValueForFirstPin,
                convertedBuf1);

    HBufC8* convertedBuf2 = HBufC8::NewLC(pinValue2MaxLength);
    ConvertPINL(iAuthTypeSecondPin->PinAttributes(), iPinValueForSecondPin,
                convertedBuf2);

    TPtr8 convertedPins(convertedBuf1->Des());
    convertedPins.Append(convertedBuf2->Des());

    iPinApdu->AppendDatabytesL(convertedPins);

    CleanupStack::PopAndDestroy(convertedBuf2);
    CleanupStack::PopAndDestroy(convertedBuf1);

    iApduExchanger->ExchangeApduAsync(iStatus, *iPinApdu);
    iState = ESendingPINApdu;
    SetActive();

}

// -----------------------------------------------------------------------------
// CSTSPinManager::PINMaxLength
// Is used to find PIN value max length for UI and for buffer creation. For UI
// needs aReturnBiggest parameter is not used and method will return proper
// maxLength of PIN. If is used for creating buffer for converted PIN
// aReturnBiggest parameter must be set to ETrue. Then method will return either
// maxlength or stored length value depending on which one is bigger.
// -----------------------------------------------------------------------------
//
TInt CSTSPinManager::PINMaxLength(const CSTSAuthType* aAuthType,
                                  TBool aReturnBiggest) const
{
    TInt returnValue;
    TInt maxLength = aAuthType->PinAttributes().MaxLength();
    TInt storedLength = aAuthType->PinAttributes().StoredLength();

    // maximum length is optional in card, so we need to set something for
    // PIN UI needs
    if (maxLength == KErrNotFound)
    {
        // if optional maxLength is not found, we use mandatory stored length
        maxLength = storedLength;
    }
    returnValue = maxLength;

    // compares maxlength and stored length
    if (aReturnBiggest)
    {
        if (storedLength > maxLength)
        {
            returnValue = storedLength;
        }
    }

    return returnValue;
}

} // namespace satsa
} // namespace java
//  End of File

