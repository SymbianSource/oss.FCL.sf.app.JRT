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


#ifndef CSTSPINAPDURESP_H
#define CSTSPINAPDURESP_H

//  INCLUDES
#include "cstsrespapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed response APDU class for PIN related command APDUs
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSPinApduResp): public CSTSRespApdu
{

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSPinApduResp* NewLC(
        TInt aMaxLength = 2,//maximum length is minimum at 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    static CSTSPinApduResp* NewL(
        TInt aMaxLength = 2,//maximum length is minimum at 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    /**
     * Destructor.
     */
    virtual ~CSTSPinApduResp();

public: // New functions

    /**
     * Checks from response bytes, is PIN blocked or not
     * @since 3.0
     * @return ETrue, if PIN is blocked. Otherwise EFalse.
     */
    TBool IsBlocked();

    /**
     * Gets retries left counter value from response apdu.
     * @since 3.0
     * @return Retries left counter value
     */
    TInt RetriesLeft() const;

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSPinApduResp();

    /**
     * Gets retries left counter value from response apdu.
     * @since 3.0
     * @param aSW1 SW1 value of response bytes
     * @param aSW2 SW2 value of response bytes
     * @return Retries left counter value
     */
    TInt CountRetriesLeft(TUint8 aSW1, TUint8 aSW2) const;

};

} // namespace satsa
} // namespace java
#endif // CSTSPINAPDURESP_H
// End of File
