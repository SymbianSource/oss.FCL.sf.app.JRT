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


// INCLUDE FILES
#include  "cstsmanagechannelresp.h"

namespace java
{
namespace satsa
{

// CONSTANTS
const TInt KSTSSuccesfullRespLength = 3;
const TInt KSTSMaxChannelNum = 3;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSTSManageChannelResp::CSTSManageChannelResp
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSTSManageChannelResp::CSTSManageChannelResp()
{

}

// -----------------------------------------------------------------------------
// CSTSApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSManageChannelResp* CSTSManageChannelResp::NewLC(TInt aMaxLength,
        CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSManageChannelResp* self = new(ELeave) CSTSManageChannelResp();
    CleanupStack::PushL(self);
    self->ConstructL(aMaxLength, aStandard);
    return self;
}

// -----------------------------------------------------------------------------
// CSTSApdu::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSTSManageChannelResp* CSTSManageChannelResp::NewL(TInt aMaxLength,
        CSTSApdu::TSTSApduStandard aStandard)
{
    CSTSManageChannelResp* self = CSTSManageChannelResp::NewLC(aMaxLength,
                                  aStandard);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CSTSManageChannelResp::~CSTSManageChannelResp()
{
}

// -----------------------------------------------------------------------------
// CSTSManageChannelResp::GetChannel
// Panics, if apdu data buffer contains too little information.
// Returns KErrNotFound if Data part of response apdu buffer is missing or is
// too long. It should be only one byte long and contain channel number.
// Returns channel number if Data part is correct length.
//
// -----------------------------------------------------------------------------
TInt CSTSManageChannelResp::GetChannel() const
{

    TInt returnValue = KErrNotFound;

    if (iRespData->ApduPtr().Length() == KSTSSuccesfullRespLength)
    {
        //first byte is channel number
        returnValue = (TInt) iRespData->ApduPtr().Ptr()[0];

        //if channel number is not ok
        if (returnValue > KSTSMaxChannelNum)
        {
            returnValue = KErrNotFound;
        }
    }
    return returnValue;
}

} // namespace satsa
} // namespace java
//  End of File
