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


#ifndef CSTSPINAPDU_H
#define CSTSPINAPDU_H

//  INCLUDES
#include "cstscmdapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed command APDU class for PIN related apdus.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPinApdu): public CSTSCmdApdu
{
public: // data types
    enum TSTSPinApduType
    {
        ESTSVerifyPin = 0,
        ESTSChangePin = 1,
        ESTSDisablePin = 2,
        ESTSEnablePin = 3,
        ESTSUnblockPin = 4
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aType Type of PIN method
     * @param aP2 Value of P2.
     * @param aStandard Type of APDU standard
     */
    static CSTSPinApdu* NewLC(TSTSPinApduType aType,
                              TUint8 aP2,
                              CSTSApdu::TSTSApduStandard aStandard);

    static CSTSPinApdu* NewL(TSTSPinApduType aType,
                             TUint8 aP2,
                             CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSPinApdu();

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSPinApdu();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(TSTSPinApduType aType,
                    TUint8 aP2,
                    CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Sets INS byte to APDU data bytes depending of type
     * @since 3.0
     * @param aType Type of PIN method
     */
    void SetPinINS(TSTSPinApduType aType);

};

} // namespace satsa
} // namespace java
#endif // CSTSPINAPDU_H
// End of File
