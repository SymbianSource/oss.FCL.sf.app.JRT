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

OS_EXPORT void RuntimeUiQt::errorL(const TDesC& /*aAppName*/, const TDesC& aShortMsg,
    const TDesC& aDetailedMsg, const TDesC& aDetailsButton, const TDesC& aOkButton)
{
    CHbDeviceMessageBoxSymbian* messageBox
    = CHbDeviceMessageBoxSymbian::NewL(CHbDeviceMessageBoxSymbian::EWarning);
    CleanupStack::PushL(messageBox);

    messageBox->SetTextL(aShortMsg);
    messageBox->SetTimeout(HbPopup::NoTimeout);

    if (aDetailedMsg.Size() > 0)
    {
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, aOkButton);
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::ERejectButton, aDetailsButton);
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
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, aOkButton);
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);
        (void)messageBox->ExecL();
    }
    messageBox->Close();
    CleanupStack::PopAndDestroy(messageBox);
}

OS_EXPORT int RuntimeUiQt::confirmL(const TDesC& /*aAppName*/, const TDesC& aQuestion,
    const ConfirmData& aConfirmData, bool /*aIdentified*/)
{
    CHbDeviceMessageBoxSymbian* messageBox
        = CHbDeviceMessageBoxSymbian::NewL(CHbDeviceMessageBoxSymbian::EWarning);
    CleanupStack::PushL(messageBox);

    messageBox->SetTextL(aQuestion);
    messageBox->SetTimeout(HbPopup::NoTimeout);

    // Deny by default.
    int result = 1;

    // ConfirmData must always have two entries. If not confirmation is denied.
    if (aConfirmData.iAnswerOptions.size() >= 2)
    {
        // Button 1 localized text.
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::EAcceptButton, (aConfirmData.iAnswerOptions.at(0))->Des());
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::EAcceptButton, ETrue);

        // Button 2 localized text.
        messageBox->SetButtonTextL(CHbDeviceMessageBoxSymbian::ERejectButton, (aConfirmData.iAnswerOptions.at(1))->Des());
        messageBox->SetButton(CHbDeviceMessageBoxSymbian::ERejectButton, ETrue);

        if (messageBox->ExecL() == CHbDeviceMessageBoxSymbian::EAcceptButton)
        {
            // AcceptButton selected.
            result = 0;
        }

        messageBox->Close();
    }
    CleanupStack::PopAndDestroy(messageBox);
    return result;
}
