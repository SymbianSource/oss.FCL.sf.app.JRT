/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: RuntimeUI QT to show error notes and security queries.
*
*/

#include "runtimeuiqt.h"

#include <hbdevicemessageboxsymbian.h>
#include <hbpopup.h>

#include "logger.h"

using namespace java::runtimeui;

OS_EXPORT void RuntimeUiQt::errorL(const TDesC& /*aAppName*/, const TDesC& aShortMsg, const TDesC& aDetailedMsg)
{
    CHbDeviceMessageBoxSymbian* messageBox
    = CHbDeviceMessageBoxSymbian::NewL(CHbDeviceMessageBoxSymbian::EWarning);
    CleanupStack::PushL(messageBox);

    messageBox->SetTextL(aShortMsg);
    messageBox->SetTimeout(HbPopup::NoTimeout);

    // Read localised versions instead of hard coded values.
    _LIT(KOkButtonText, "OK");

    if (aDetailedMsg.Size() > 0)
    {
        _LIT(KDetailsButtonText, "Details");

        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, KOkButtonText);
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::ERejectButton, KDetailsButtonText);
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::ERejectButton, ETrue);

        if (messageBox->ExecL() == CHbDeviceMessageBoxSymbian::ERejectButton)
        {
            messageBox->SetButton(CHbDeviceMessageBoxSymbian::ERejectButton, EFalse);
            messageBox->SetTextL(aDetailedMsg);
            (void)messageBox->ExecL();
        }
    }
    else
    {
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, KOkButtonText);
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
        (void)messageBox->ExecL();
    }
    messageBox->Close();
    CleanupStack::PopAndDestroy(messageBox);
}

OS_EXPORT int RuntimeUiQt::confirmL(const TDesC& /*aAppName*/, const TDesC& aQuestion)
{
    CHbDeviceMessageBoxSymbian* messageBox
    = CHbDeviceMessageBoxSymbian::NewL(CHbDeviceMessageBoxSymbian::EWarning);
    CleanupStack::PushL(messageBox);

    messageBox->SetTextL(aQuestion);
    messageBox->SetTimeout(HbPopup::NoTimeout);

    // Read localised versions instead of hard coded values.
    _LIT(KAllowButtonText, "Allow");
    _LIT(KDenyButtonText, "Deny");

    messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, KAllowButtonText);
    messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
    messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::ERejectButton, KDenyButtonText);
    messageBox->SetButton(CHbDeviceMessageBoxSymbian::ERejectButton, ETrue);

    // by default the answer is Allow
    int result = 0;
    if (messageBox->ExecL() == CHbDeviceMessageBoxSymbian::ERejectButton)
    {
        // change the answer to Deny
        result = 1;
    }

    messageBox->Close();
    CleanupStack::PopAndDestroy(messageBox);
    return result;
}
