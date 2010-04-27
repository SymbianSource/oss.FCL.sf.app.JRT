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


#ifndef CSTSENVELOPE_H
#define CSTSENVELOPE_H

//  INCLUDES
#include "cstscmdapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Common envelope command APDU class.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSEnvelope): public CSTSCmdApdu
{

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSEnvelope* NewLC(TDesC8& aApduBytes,
    CSTSApdu::TSTSApduStandard aStandard);

    static CSTSEnvelope* NewL(TDesC8& aApduBytes,
                              CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSEnvelope();

public: // New functions

    /**
     * Check is apdu in correct form according to JSR177. Leaves with proper
     * error code if needed.
     * @since 3.0
     */
    void CheckL() const;

    /**
     * Sets Class byte depending on whether the phone is running GSM or UMTS.
     * @since 3.0
     */
    void SetClassByte();

private: //New fuctions

    /**
     * C++ default constructor.
     */
    CSTSEnvelope();

};

} // namespace satsa
} // namespace java
#endif // CSTSENVELOPE_H
// End of File

