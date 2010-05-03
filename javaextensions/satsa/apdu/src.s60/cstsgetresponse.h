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


#ifndef CSTSGETRESPONSE_H
#define CSTSGETRESPONSE_H

//  INCLUDES
#include "cstscmdapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed command APDU class for Get Response apdu.
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSGetResponse): public CSTSCmdApdu
{
public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSGetResponse* NewLC(TInt aExpectedRespLength,
    CSTSApdu::TSTSApduStandard aStandard);

    static CSTSGetResponse* NewL(TInt aExpectedRespLength,
                                 CSTSApdu::TSTSApduStandard aStandard);

    /**
     * Destructor.
     */
    virtual ~CSTSGetResponse();

public: // New functions

    /**
     * Sets expected response length.
     * @since 3.0
     * @param aExpectedRespLength Length of expected response.
     */
    void SetExpectedResponseLength(TInt aExpectedRespLength);

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSGetResponse();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(TInt aExpectedRespLength,
                    CSTSApdu::TSTSApduStandard aStandard);

};

} // namespace satsa
} // namespace java
#endif // CSTSGETRESPONSE_H
// End of File
