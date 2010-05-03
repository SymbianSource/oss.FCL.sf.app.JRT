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


#ifndef CSTSMANAGECHANNELRESP_H
#define CSTSMANAGECHANNELRESP_H

//  INCLUDES
#include "cstsrespapdu.h"

namespace java
{
namespace satsa
{

// CLASS DECLARATION
/**
 *  Detailed response APDU class for Manage Channel command APDU
 *
 *  @since 3.0
 */
NONSHARABLE_CLASS(CSTSManageChannelResp): public CSTSRespApdu
{

public: // Constructors and destructor
    /**
     * Two-phased constructor.
     */
    static CSTSManageChannelResp* NewLC(
        TInt aMaxLength = 2,//maximum length can be minimum at 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    static CSTSManageChannelResp* NewL(
        TInt aMaxLength = 2,//maximum length can be minimum at 2
        CSTSApdu::TSTSApduStandard aStandard = CSTSApdu::ESTSUICC);

    /**
     * Destructor.
     */
    virtual ~CSTSManageChannelResp();

public: // New functions

    /**
     * Gets channel number from response apdu
     * @since 3.0
     * @return Channel number
     */
    TInt GetChannel() const;

private: // New functions

    /**
     * C++ default constructor.
     */
    CSTSManageChannelResp();

};

} // namespace satsa
} // namespace java
#endif // CSTSMANAGECHANNELRESP_H
// End of File
