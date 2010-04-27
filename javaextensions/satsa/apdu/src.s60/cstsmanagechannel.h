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


#ifndef CSTSMANAGECHANNEL_H
#define CSTSMANAGECHANNEL_H

//  INCLUDES
#include "cstscmdapdu.h"
#include "stsapduconstants.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed command APDU class for Manage Channel apdu.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSManageChannel): public CSTSCmdApdu
{
public: // data types
    enum TSTSManageChannelApduType
    {
        ESTSOpenChannel,
        ESTSCloseChannel
    };

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     * @param aChannel Channel number which will be used. If using open
     *  channel purposes, this parameter is not used anywhere.
     * @param aType Type of the manage channel apdu
     * @param aStandard Apdu standard
     */
    static CSTSManageChannel* NewLC(TInt aChannel,
                                    TSTSManageChannelApduType aType,
                                    CSTSApdu::TSTSApduStandard aStandard);

    static CSTSManageChannel* NewL(TInt aChannel,
                                   TSTSManageChannelApduType aType,
                                   CSTSApdu::TSTSApduStandard aStandard);

public: // New functions

    /**
     * Sets type of manage channel apdu and channel which will be closed.
     * @since 3.0
     * @param Type of Manage Channel apdu
     * @param aChannel In close channel case contains channel, which will
     * be closed. Other cases this parameter is not used at all (zero can
     * be used).
     */
    void SetApduType(TSTSManageChannelApduType aType, TInt aChannel);

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSManageChannel();

    /**
     * Generates P1 and P2 values depending of Manage Channel apdu type
     * @since 3.0
     * @param aP1 Reference to P1
     * @param aP2 Reference to P2
     * @param aMaxLength Reference to maximum length
     * @param aType Type of Manage Channel apdu
     */
    void GenerateP1P2(TUint8& aP1,
                      TUint8& aP2,
                      TInt aChannel,
                      TSTSManageChannelApduType aType);

protected: // New functions

    /**
     * Protected construction to allow derivation
     */
    void ConstructL(TInt aChannel,
                    TSTSManageChannelApduType aType,
                    CSTSApdu::TSTSApduStandard aStandard);

private: // Data

    TSTSManageChannelApduType iType;

};

} // namespace satsa
} // namespace java
#endif // CSTSMANAGECHANNEL_H
// End of File
