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



#include <ApDataHandler.h>

#include "swtbrowserapdatahandler.h"



CSwtBrowserApDataHandler::CSwtBrowserApDataHandler()
{
}


void CSwtBrowserApDataHandler::ConstructL(CCommsDatabase& aDb)
{
    iApDataHandler = CApDataHandler::NewLC(aDb);
    CleanupStack::Pop(iApDataHandler);
}


EXPORT_C CSwtBrowserApDataHandler* CSwtBrowserApDataHandler::NewL(CCommsDatabase& aDb)
{
    CSwtBrowserApDataHandler* self
    = new(ELeave) CSwtBrowserApDataHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aDb);
    CleanupStack::Pop(self);
    return self;
}


CSwtBrowserApDataHandler::~CSwtBrowserApDataHandler()
{
    delete iApDataHandler;
    iApDataHandler = NULL;
}


EXPORT_C void CSwtBrowserApDataHandler::AccessPointDataL(TUint32 aUid,
        CApAccessPointItem& aApItem)
{
    ASSERT(iApDataHandler);
    iApDataHandler->AccessPointDataL(aUid, aApItem);
}
