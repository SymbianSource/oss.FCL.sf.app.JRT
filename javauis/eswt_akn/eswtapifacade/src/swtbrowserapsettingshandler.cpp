/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


#include <w32std.h>
#include <ApSettingsHandlerUI.h>
#include "swtbrowserapsettingshandler.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSwtBrowserApSettingsHandler::CSwtBrowserApSettingsHandler()
{
}


void CSwtBrowserApSettingsHandler::ConstructL()
{
    CApSettingsHandler* settingsHandler =
        CApSettingsHandler::NewLC(ETrue,
                                  EApSettingsSelListIsPopUp,
                                  EApSettingsSelMenuSelectNormal,
                                  KEApIspTypeAll,
                                  EApBearerTypeAll,
                                  KEApSortNameAscending,
                                  EIPv4 | EIPv6);
    CleanupStack::Pop(settingsHandler);
    iSettingsHandler = settingsHandler;
}

EXPORT_C CSwtBrowserApSettingsHandler* CSwtBrowserApSettingsHandler::NewLC()
{
    CSwtBrowserApSettingsHandler* self
    = new(ELeave) CSwtBrowserApSettingsHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CSwtBrowserApSettingsHandler::~CSwtBrowserApSettingsHandler()
{
    delete iSettingsHandler;
}

EXPORT_C TInt CSwtBrowserApSettingsHandler::RunSettingsL(TUint32 aHighLight, TUint32& aSelected)
{
    ASSERT(iSettingsHandler);
    return iSettingsHandler->RunSettingsL(aHighLight, aSelected);
}


