/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Custom command utility class for AMMS custom commands.
*
*/



// INCLUDE FILES
#include <jdebug.h>
#include "cammscustomcommandutility.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAMMSCustomCommandUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSCustomCommandUtility* CAMMSCustomCommandUtility::NewL(
    RMMFController& aMMFController)
{
    CAMMSCustomCommandUtility* self = NewLC(aMMFController);
    CleanupStack::Pop(self);

    return self;
}

// -----------------------------------------------------------------------------
// CAMMSCustomCommandUtility::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CAMMSCustomCommandUtility* CAMMSCustomCommandUtility::NewLC(
    RMMFController& aMMFController)
{
    CAMMSCustomCommandUtility* self =
        new(ELeave) CAMMSCustomCommandUtility(aMMFController);

    CleanupStack::PushL(self);

    return self;
}

// Destructor
CAMMSCustomCommandUtility::~CAMMSCustomCommandUtility()
{
    DEBUG("AMMS::CAMMSCustomCommandUtility::~CAMMSCustomCommandUtility");
}

TInt CAMMSCustomCommandUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom)
{
    DEBUG_INT(
        "AMMS::CAMMSCustomCommandUtility::CustomCommandSync(1) function: %d",
        aFunction);

    // In the first message (ECibGetBuilder) the aDataTo1 is null,
    // but in the second message (ECibBuild) it should contain the interface ID

    return iMMFController.CustomCommandSync(
               aDestination,
               aFunction,
               aDataTo1,
               aDataTo2,
               aDataFrom);
}

TInt CAMMSCustomCommandUtility::CustomCommandSync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2)
{
    DEBUG_INT(
        "AMMS::CAMMSCustomCommandUtility::CustomCommandSync(2) function: %d",
        aFunction);
    return iMMFController.CustomCommandSync(
               aDestination,
               aFunction,
               aDataTo1,
               aDataTo2);
}

void CAMMSCustomCommandUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TDes8& aDataFrom,
    TRequestStatus& aStatus)
{
    DEBUG_INT(
        "AMMS::CAMMSCustomCommandUtility::CustomCommandAsync(1) function: %d",
        aFunction);
    iMMFController.CustomCommandAsync(
        aDestination,
        aFunction,
        aDataTo1,
        aDataTo2,
        aDataFrom,
        aStatus);
}

void CAMMSCustomCommandUtility::CustomCommandAsync(
    const TMMFMessageDestinationPckg& aDestination,
    TInt aFunction,
    const TDesC8& aDataTo1,
    const TDesC8& aDataTo2,
    TRequestStatus& aStatus)
{
    DEBUG_INT(
        "AMMS::CAMMSCustomCommandUtility::CustomCommandAsync(2) function: %d",
        aFunction);
    iMMFController.CustomCommandAsync(
        aDestination,
        aFunction,
        aDataTo1,
        aDataTo2,
        aStatus);
}

// -----------------------------------------------------------------------------
// CCAMMSCustomCommandUtility::CAMMSCustomCommandUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CAMMSCustomCommandUtility::CAMMSCustomCommandUtility(
    RMMFController& aMMFController):
        iMMFController(aMMFController)
{
}

//  End of File
