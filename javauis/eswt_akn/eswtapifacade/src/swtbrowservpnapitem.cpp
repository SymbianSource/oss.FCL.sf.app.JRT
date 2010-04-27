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
#include "swtbrowservpnapitem.h"



// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSwtBrowserVpnApItem::CSwtBrowserVpnApItem()
{
}


void CSwtBrowserVpnApItem::ConstructL()
{
    iVpnApItem = CVpnApItem::NewLC();
    CleanupStack::Pop(iVpnApItem);
}


EXPORT_C CSwtBrowserVpnApItem* CSwtBrowserVpnApItem::NewL()
{
    CSwtBrowserVpnApItem* self
    = new(ELeave) CSwtBrowserVpnApItem();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}



CSwtBrowserVpnApItem::~CSwtBrowserVpnApItem()
{
    delete iVpnApItem;
}

CVpnApItem& CSwtBrowserVpnApItem::VpnApItem()
{
    ASSERT(iVpnApItem);
    return *iVpnApItem;
}


EXPORT_C TInt CSwtBrowserVpnApItem::ReadUint(const TUint aColumn, TUint32& aValue)
{
    ASSERT(iVpnApItem);
    return iVpnApItem->ReadUint(static_cast< TVpnApMember >(aColumn), aValue);
}

EXPORT_C TInt CSwtBrowserVpnApItem::ReadVpnApUid(TUint32& aValue)
{
    ASSERT(iVpnApItem);
    return iVpnApItem->ReadUint(EApVpnWapAccessPointID, aValue);
}

EXPORT_C TInt CSwtBrowserVpnApItem::ReadRealIapId(TUint32& aValue)
{
    ASSERT(iVpnApItem);
    return iVpnApItem->ReadUint(EApVpnRealWapID, aValue);
}



