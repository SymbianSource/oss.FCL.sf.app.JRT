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


#ifndef CSTSCONNECTION_H
#define CSTSCONNECTION_H

//  INCLUDES
#include "functionserver.h"
#include <e32base.h>
#include "stsapduconstants.h"   // for TSTSNetworkType
#include "cstsuri.h"            // for turitype
#include "mstscloseobserver.h"

using namespace java::util;

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSChannelManager;
class CSTSSlotManager;
class CSTSPinManager;
class CSTSApduExchanger;
class CSTSAccessControl;
class CSTSCardOperationsFacade;
class CSTSCardAppManager;
class MSTSRespHandler;
class CSTSCmdApdu;
class CSTSMidletInfo;
class CSTSBTSapListener;
class CSTSCardStateListener;

// CLASS DECLARATION
/**
 *  SATSA-APDUs native side implementation's interface to JNI side.
 *  Handels communication with smart card.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSConnection) : public CBase, public MSTSCloseObserver, public FunctionServer
{

private: // Type definition
    enum TAidParserState
    {
        EFirstDigit,
        ESecondDigit,
        EDot,
        EERROR
    };

public:
    enum TConnectionState
    {
        EOpened,
        EClosed,
        ECardRemoved,
        EBTSapInUse
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aRespHandler When asynchronous exchange is ready, response will
     * be delivered to response handler. Ownership transferred.
     * @param aMidletInfo Information about midlet
     */
    static CSTSConnection* NewL(JNIEnv& aJni,
                                jobject aPeer,MSTSRespHandler* aRespHandler,
                                CSTSMidletInfo* aMidletInfo);

    /**
     * Destructor.
     */
    virtual ~CSTSConnection();

public: // New functions

    /**
     * Opens a connection to smart card application. URI must be set before
     * this method is called.
     * @since 3.0
     */
    void OpenL();

    /**
     * To wrap the CallMethodL()
     */
    static void WrapOpenL(CSTSConnection* connection);

    /**
     * Closes a connection to smart card. Cancels all pending apdu sending.
     * @since 3.0
     */
    void CloseL();

    /**
     * To wrap the CallMethodL()
     */
    static void WrapCloseL(CSTSConnection* connection);

    /**
     * Pops up a UI that requests the user for an old or existing PIN value
     * and the new PIN value to change the value of the PIN.
     * @since 3.0
     * @param aPinID The type of PIN which is supposed to prompt the user
     * to change.
     * @param aResponseAPDU On return contains status word received from
     * the smart card in byte array. Null if the user cancels the request.
     */
    void ChangePinL(TInt aPinID);

    /**
     * To wrap the CallMethodL()
     */
    static void WrapChangePinL(CSTSConnection* connection, TInt aPinID);

    /**
     * Pops up a UI that requests the user to enter the value for the PIN
     * that is to be disabled.
     * @since 3.0
     * @param aPinID The type of PIN which is required to prompt the user
     * to enter.
     */
    void DisablePinL(TInt aPinID);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapDisablePinL(CSTSConnection* connection, TInt aPinID);

    /**
     * Pops up a UI that requests the user to enter the value for the PIN
     * that is to be enabled.
     * @since 3.0
     * @param aPinID The type of PIN which is required to prompt the user
     * to enter.
     */
    void EnablePinL(TInt aPinID);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapEnablePinL(CSTSConnection* connection, TInt aPinID);

    /**
     * Asks from the user PIN value for PIN verification purposes and sends
     * it to the card.
     * @since 3.0
     * @param aPinID The type of PIN which is supposed to prompt the user
     * to enter.
     */
    void EnterPinL(TInt aPinID);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapEnterPinL(CSTSConnection* connection, TInt aPinID);

    /**
     * Pops up a UI that requests the user to enter the value for the
     * unblocking PIN and the new value for the blocked PIN.
     * @since 3.0
     * @param aBlockedPinID The ID of PIN that is to be unblocked.
     * @param aUnblockingPinID The ID of unblocking PIN.
     */
    void UnblockPinL(TInt aBlockedPinID, TInt aUnblockingPinID);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapUnblockPinL(CSTSConnection* connection, TInt aBlockedPinID, TInt aUnblockingPinID);
    /**
     * Exchanges an APDU command with a smart card application.
     * @since 3.0
     * @param aCommandAPDU A byte encoded command for the smart card
     * application
     */
    void ExchangeAPDUL(TPtrC8 aCommandAPDU);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapExchangeAPDUL(CSTSConnection* connection, TPtrC8 aCommandDes);

    /**
     * Returns the Answer To Reset (ATR) message sent by the smart card
     * in response to the reset operation.
     * @since 3.0
     * @param aATR On return contains the ATR message if the card
     * is inserted, or null if the card has been removed or the connection
     * has been closed.
     */
    std::wstring GetATRL();
    /**
     * To wrap the CallMethodL()
     */
    static std::wstring WrapGetATRL(CSTSConnection* connection);

    /**
     * Gets information of smart card slots.
     * @since 3.0
     * @param aResponseAPDU Return a information of smart cards slots in
     * string format.
     */

    std::wstring GetSlotsL();
    /**
     * To wrap the CallMethodL()
     */
    static std::wstring WrapGetSlotsL(CSTSConnection* connection);

    /**
     * Sets URI. Ownership transferred.
     * @since 3.0
     * @param aURI URI object.
     */
    void SetURI(TInt /*CSTSURI& */aURI);
    /**
     * To wrap the CallMethodL()
     */
    static void WrapSetURI(CSTSConnection* connection, CSTSURI* aURI);

public: // Functions from base classes

    void NotifyClose(TInt aReason);

private:

    // From FunctionServer
    virtual void vmAttached();
    virtual void doServerSideInit();
    void AddToScheduler();

    /**
     * C++ default constructor.
     */
    CSTSConnection(MSTSRespHandler* aRespHandler,
                   CSTSMidletInfo* aMidletInfo);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Closes a connection to smart card. Cancels all pending apdu sending.
     * For destructor use.
     * @since 3.0
     */
    void Close();

    /**
     * Checks iState member variable and leaves if connetion is
     * closed or card is removed. Checks also iTarget member variable
     * and leaves if SAT connection is used and it is not allowed.
     */
    void CheckStateL(TBool aSATNotAllowed = EFalse);

    /**
     * Opens channel and selects card application. If selection fails
     * closes the channel and leaves with proper leave code.
     */
    void DoSelectApplicationL();

private: // Data

    CSTSChannelManager* iChannelManager;//owned
    CSTSSlotManager* iSlotManager;//owned
    CSTSPinManager* iPinManager; //owned
    CSTSApduExchanger* iApduExchanger; //owned
    CSTSAccessControl* iAccessControl; //owned
    CSTSCardOperationsFacade* iCOFacade; //owned
    CSTSCardAppManager* iCardAppManager; //owned

    TInt iChannel;
    //Uri, owned
    CSTSURI* iURI;

    //SAT or AID connection
    CSTSURI::TURIType iURITarget;
    //is connection opened succesfully or not
    TConnectionState iState;
    //whether the phone is running in GSM or UMTS
    TSTSNetworkType iNetwork;

    //command apdu, owned
    CSTSCmdApdu* iCmdApdu;
    MSTSRespHandler* iRespHandler; //owned

    CSTSMidletInfo* iMidletInfo; //owned

    HBufC8* iATR; //owned

    //Bluetooth remote Sim Access Profile Listener
    CSTSBTSapListener* iBTSapListener; //owned

    //For listening is card available or not
    CSTSCardStateListener* iCardStateListener; //owned
    //JNIEnv* mJni;
    //jobject mPeer;

};

} // namespace satsa
} // namespace java
#endif // CSTSCONNECTION_H
// End of File
