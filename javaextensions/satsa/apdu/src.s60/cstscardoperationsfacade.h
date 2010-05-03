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


#ifndef CSTSCARDOPERATIONSFACADE_H
#define CSTSCARDOPERATIONSFACADE_H

//  INCLUDES
#include <e32base.h>
#include <rmmcustomapi.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Facade type of class for handling card related operations.
 *
 *
 *  @since 3.0
 */

NONSHARABLE_CLASS(CSTSCardOperationsFacade): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSCardOperationsFacade* NewL();

    /**
     * Destructor.
     */
    virtual ~CSTSCardOperationsFacade();

public: // New functions

    /**
     * Send gived APDU to card asynchronously
     * @since 3.0
     * @param aStatus Status
     * @param aCardReaderId Used reader(=slot)
     * @param aCmdData Standard type of APDU message
     */
    void SendAPDUReq(TRequestStatus& aStatus,
                     TUint8 aCardReaderId,
                     TDes8* aCmdData);

    /**
     * Gets response bytes. Asychronous SendAPDUReq must be called and it
     * must be returned before this method is called.
     * @since 3.0
     * @param aRspData Descriptor where data will be saved
     */
    void GetResponseBytes(TDes8* aRspData);

    /**
     * Cancels asynchronous APDU sending.
     * @since 3.0
     */
    void CancelAPDUReq();

    /**
     * Gets Answer To Reset bytes from the card.
     * @since 3.0
     * @param aATR Returned ATR bytes
     * @param aReader Used reader.
     */
    void GetATR(TDes8& aATR, TUint8 aReader);

    /**
     * Checks is phone running in GSM network or not
     * @since 3.0
     */
    TBool IsGSMNetwork() const;

private:

    /**
     * C++ default constructor.
     */
    CSTSCardOperationsFacade();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Generates info part and does apduReq
     */
    void DoSend(TRequestStatus& aStatus,
                TUint8 aServiceType,
                TUint8 aReader,
                TDes8* aCmdData);

private: // Data
    RMmCustomAPI iCustomAPI;

    RTelServer iSession;
    RMobilePhone iPhone;

    //buffer for TApdu info field, owned
    HBufC8* iInfoBuf;
    //points to info buffer
    TPtr8 iInfoPtr;

    RMmCustomAPI::TApdu* iApduReq; //owned

    //buffer for command and response data, owned
    HBufC8* iDataBuf;
    //points to data buffer
    TPtr8 iDataPtr;

    TBool iGSMNetwork;

};

} // namespace satsa
} // namespace java
#endif // CSTSCARDOPERATIONSFACADE_H
// End of File

