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


#ifndef CSTSCHANNELMANAGER_H
#define CSTSCHANNELMANAGER_H

//  INCLUDES
#include <e32base.h>

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class CSTSApduExchanger;
class CSTSConnection;
class CSTSManageChannel;
class CSTSManageChannelResp;

// CLASS DECLARATION

/**
 *  Handels the channel related operations of connection.
 *  Used for opening and closing logical channels.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSChannelManager): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CSTSChannelManager* NewL(CSTSApduExchanger* aApduExchanger);

    /**
     * Destructor.
     */
    virtual ~CSTSChannelManager();

public: // New functions

    /**
     * Requests free channel from the card.
     * @since 3.0
     * @return Opened channel or -1 if no free channel available or
     * other error code
     */
    TInt RequestChannel();

    /**
     * Closes the gived channel.
     * @since 3.0
     * @param aChannel Channel which will be closed
     * @return Error code
     */
    TInt CloseChannel(TInt aChannel);

private:

    /**
     * C++ default constructor.
     */
    CSTSChannelManager(CSTSApduExchanger* aApduExchanger);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data
    //for sending apdus
    CSTSApduExchanger* iApduExchanger; //not owned

    //manage channel apdu, owned
    CSTSManageChannel* iCmdApdu;

    CSTSManageChannelResp* iRespApdu; //owned
};

} // namespace satsa
} // namespace java
#endif // CSTSCHANNELMANAGER_H
// End of File
