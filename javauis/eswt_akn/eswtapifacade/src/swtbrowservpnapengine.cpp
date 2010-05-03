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



#include <VpnApItem.h>
#include <VpnApEngine.h>
#include "swtbrowservpnapitem.h"
#include "swtbrowservpnapengine.h"



// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSwtBrowserVpnApEngine::CSwtBrowserVpnApEngine()
{
}


void CSwtBrowserVpnApEngine::ConstructL(CCommsDatabase& aDb)
{
    iVpnApEngine = CVpnApEngine::NewLC(&aDb);
    CleanupStack::Pop(iVpnApEngine);
}


EXPORT_C CSwtBrowserVpnApEngine* CSwtBrowserVpnApEngine::NewL(CCommsDatabase& aDb)
{
    CSwtBrowserVpnApEngine* self
    = new(ELeave) CSwtBrowserVpnApEngine();
    CleanupStack::PushL(self);
    self->ConstructL(aDb);
    CleanupStack::Pop(self);
    return self;
}



CSwtBrowserVpnApEngine::~CSwtBrowserVpnApEngine()
{
    delete iVpnApEngine;
}

EXPORT_C TBool CSwtBrowserVpnApEngine::IsVpnApL(const TUint32 aUid)
{
    ASSERT(iVpnApEngine);
    return iVpnApEngine->IsVpnApL(aUid);
}

EXPORT_C void CSwtBrowserVpnApEngine::VpnDataL(TUint32 aUid, CSwtBrowserVpnApItem& aVpnApItem)
{
    ASSERT(iVpnApEngine);
    iVpnApEngine->VpnDataL(aUid, aVpnApItem.VpnApItem());
}



