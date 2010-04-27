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


#ifndef CSTSPINMANAGER_H
#define CSTSPINMANAGER_H

//  INCLUDES
#include <e32base.h>
#include <secdlg.h>
#include "cstspinapdu.h"    // for TPinApduType
#include "functionserver.h"
#include "cstsconnection.h"
#include <jni.h>

class SecurityDialogFactory;
class MSecurityDialog;

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS


class CSTSApduExchanger;
class CSTSAccessControl;
class CSTSAuthType;
class CSTSPinAttributes;
class CSTSPinApduResp;
class MSTSRespHandler;

// CLASS DECLARATION

/**
 *  Handles PIN related methods and shows needed UIs.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPinManager): public CActive
{

public: // data types
    enum TPinRetryCounter
    {
        EFirstTime = 0,
        ERetrying = 1,
        EPINBlocked = 2
    };
private:
    enum TPinManagerState
    {
        EShowingPinBlockedUI,
        EShowing1ValuePinUI,
        EShowing2ValuePinUI,
        ESendingPINApdu
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSPinManager* NewL(JNIEnv* aJni,jobject aPeer,
                                CSTSApduExchanger* aApduExchanger,
                                CSTSAccessControl* aAccessControl,
                                MSTSRespHandler* aRespHandler);

    /**
     * Destructor.
     */
    virtual ~CSTSPinManager();

public: // New functions

    /**
     * Uses UI for getting PIN value from user. Constructs
     * needed APDU message and sends it to card. Returns
     * response adpu to caller.
     * @since 3.0
     * @param aPinID Which PIN value will be used
     */
    void DisablePinL(TInt aPinID);

    /**
     * Uses UI for getting PIN value from user. Constructs
     * needed APDU message and sends it to card. Returns
     * response adpu to caller.
     * @since 3.0
     * @param aPinID Which PIN value will be used
     */
    void ChangePinL(TInt aPinID);

    /**
     * Uses UI for getting PIN value from user. Constructs
     * needed APDU message and sends it to card. Returns
     * response adpu to caller.
     * @since 3.0
     * @param aPinID Which PIN value will be used
     */
    void EnablePinL(TInt aPinID);

    /**
     * Uses UI for getting PIN value from user. Constructs
     * needed APDU message and sends it to card. Returns
     * response adpu to caller.
     * @since 3.0
     * @param aPinID Which PIN value will be used
     */
    void EnterPinL(TInt aPinID);

    /**
     * Uses UI for getting PIN values from user. Constructs
     * needed APDU message and sends it to card. Returns
     * response adpu to caller.
     * @since 3.0
     * @param aBlockedPinID Which PIN value is blocked
     * @param aUnblockedPinID Which PIN value will unblock the PIN
     */
    void UnblockPinL(TInt aBlockedPinID, TInt aUnblockedPinID);

    /**
     * Cancels pin method operation execution.
     * @since 3.0
     */
    void CancelOperation();

protected: // Functions from base classes

    /**
     * From CActive RunL
     * @return void
     */
    void RunL();

    /**
     * From CActive Cancel request
     * @return void
     */
    void DoCancel();

    /**
     * From CActive RunError
     * @return The leave code
     */
    TInt RunError(TInt aError);

private:

    /**
     * C++ default constructor.
     */
    CSTSPinManager(CSTSApduExchanger* aApduExchanger,
                   CSTSAccessControl* aAccessControl,
                   MSTSRespHandler* aRespHandler);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Convert gived PIN value to needed form with help of PIN attributes
     * @since 3.0
     * @param aPinAttributes PIN attributes
     * @param aPinValue PIN value which must be converted
     * @param aConvertedPIN Adds converted PIN to this buffer
     */
    void ConvertPINL(const CSTSPinAttributes& aPinAttributes,
                     const TDes& aPinValue,
                     HBufC8*& aConvertedPIN);

    /**
     * Makes preparation common for all PIN methods.
     * @since 3.0
     * @param aPinApduType Type of PIN APDU
     * @param aFirstPinID PIN id number
     * @param aUnblockedPinID Unblocked PIN id number in Unblock case.
     *                       Otherwise can be left empty.
     */
    void PreparePinMethodL(CSTSPinApdu::TSTSPinApduType aPinApduType,
                           TInt aFirstPinID,
                           TInt aUnblockedPinID = KErrNotFound);

    /**
     * Shows correct PIN UI depending on PIN apdu type
     * @since 3.0
     * @param aPinApduType Type of PIN APDU
     */
    void ShowPinUI(CSTSPinApdu::TSTSPinApduType aPinApduType);

    /**
     * Delivers responsebytes to response handler (to java side).
     * @since 3.0
     */
    void DeliverResponseL();

    /**
     * Sets needed parameters for PIN UI.
     * @since 3.0
     * @param aAuthType Authtype where parameters will be get
     * @param aPINParams Object where parameters will be set
     */
    void SetPINUIParametersL(const CSTSAuthType* aAuthType,
                             TPINParams& aPINParams);

    /**
     * Does needed operations after APDU is send and response
     * is got from card.
     * @since 3.0
     */
    void DoAfterSendApduL();

    /**
     * Does needed operations after PIN UI of one PIN value is
     * showed to user.
     * @since 3.0
     */
    void DoAfter1ValuePinUIL();

    /**
     * Does needed operations after PIN UI of two PIN value is
     * showed to user.
     * @since 3.0
     */
    void DoAfter2ValuePinUIL();

    /**
     * Finds proper maximum length of PIN.
     * @since 3.0
     * @param aAuthType Authtype where lengths will be get
     * @param aReturnBiggest If there is need to get maxlength or
     * stored length depending on which one is biggest for buffer creation
     * purposes this must be set to ETrue.
     */
    TInt PINMaxLength(const CSTSAuthType* aAuthType,
                      TBool aReturnBiggest = EFalse) const;

private: // Data

    MSecurityDialog* iSecurityDialog;
    CSTSApduExchanger* iApduExchanger; //not owned
    CSTSAccessControl* iAccessControl; //not owned

    TPINParams iPINParamsFirstPin;
    TPINParams iPINParamsSecondPin;

    //pointer to authTypes, owned
    CSTSAuthType* iAuthTypeFirstPin;
    CSTSAuthType* iAuthTypeSecondPin;

    //save information of current PIN retry state
    TPinRetryCounter iRetries;

    CSTSCmdApdu* iPinApdu; //owned
    CSTSPinApduResp* iPinApduResp; //owned

    MSTSRespHandler* iRespHandler; //not owned

    HBufC8* iResponseBuf;

    TPinManagerState iState;

    //PIN UI returned pin will be put to this
    TPINValue iPinValueForFirstPin;
    TPINValue iPinValueForSecondPin;

    //saves information which pin method is called
    CSTSPinApdu::TSTSPinApduType iPinApduType;
    JNIEnv* mJni;
    jobject mPeer;
};

} //namespace satsa
} //namespace java
#endif // CSTSPINMANAGER_H
// End of File
