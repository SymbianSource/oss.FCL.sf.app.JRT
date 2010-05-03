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


#ifndef CSTSAPDUEXCHANGER_H
#define CSTSAPDUEXCHANGER_H

//  INCLUDES
#include <e32base.h>
#include <jni.h>


namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSCmdApdu;
class CSTSRespApdu;
class MSTSRespHandler;
class CSTSCardOperationsFacade;
class CSTSGetResponse;


// CLASS DECLARATION
/**
*  Active object for sending APDUs to card.
*
*
*  @since 3.0
*/

NONSHARABLE_CLASS(CSTSApduExchanger): public CActive
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CSTSApduExchanger* NewL(JNIEnv* aJni,
    jobject aPeer,
    CSTSCardOperationsFacade* aCOFacade);

    /**
    * Destructor.
    */
    virtual ~CSTSApduExchanger();

public: // New functions

    /**
    * Send gived APDU to card synchronously
    * @since 3.0
    * @param aCmdApdu Command apdu message
    * @param aRespApdu On return contains response apdu
    * @return Error code
    */
    TInt ExchangeApdu(CSTSCmdApdu& aCmdApdu,
                      CSTSRespApdu& aRespApdu);

    /**
    * Send gived APDU to card asynchronously.
    * @since 3.0
    * @param aCmdApdu Command apdu message
    * @param aRespHandler Object which will handle response
    */
    void ExchangeApduAsync(CSTSCmdApdu& aCmdApdu,
                           MSTSRespHandler* aRespHandler);

    /**
    * Send gived APDU to card synchronously. Response bytes can
    * be get after this asynhronous call is returned with help of
    * GetResponseBytes method
    * @since 3.0
    * @param aStatus Status
    * @param aCmdApdu Command apdu message
    */
    void ExchangeApduAsync(TRequestStatus& aStatus,
                           CSTSCmdApdu& aCmdApdu);

    /**
    * Gets response bytes. Asychronous ExchangeApduAsync with status
    * parameter must be called and asynhronous call must be
    * ready before this method is called.
    * @since 3.0
    * @param aRspData Descriptor where data will be saved
    */
    void GetResponseBytes(CSTSRespApdu& aRespApdu);

    /**
    * Cancels pending asynchronous apdu axchange operation.
    * @since 3.0
    */
    void CancelExchange();

    /**
    * Sets channel
    * @since 3.0
    * @param aChannel Used channel
    */
    void SetChannel(TInt aChannel);

    /**
    * Sets reader
    * @since 3.0
    * @param aReader Used reader
    */
    void SetReader(TInt aReader);

    /**
    * Sets connection type.
    * @since 3.0
    * @param aSatConnection True if SAT connection, False if AID connection
    */
    void SetConnectionType(TBool aSatConnection);


protected:  // Functions from base classes

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

private:

    /**
    * C++ default constructor.
    */
    CSTSApduExchanger(CSTSCardOperationsFacade* aCOFacade);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    /**
    * Does needed initialization and exchange call.
    * @since 3.0
    * @param aCmdApdu Command apdu message
    */
    void DoExchange(CSTSCmdApdu& aCmdApdu);



private:    // Data

    // for sending apdus, not owned
    CSTSCardOperationsFacade* iCOFacade;

    // for handling asynchronous call response, not owned
    MSTSRespHandler* iRespHandler;

    // for gathering all responses together
    HBufC8* iRespBuf; //owned

    // for saving one response
    CSTSRespApdu* iRespApdu; //owned
    CSTSGetResponse* iGetResponseApdu; //owned
    // for saving original command apdu
    CSTSCmdApdu* iCmdApdu; //not owned

    CActiveSchedulerWait* iWait; //owned

    TInt iChannel;
    TInt iReader;

    JNIEnv* mJni;
    jobject mPeer;

};

} // namespace satsa
} // namespace java
#endif // CSTSAPDUEXCHANGER_H

// End of File

