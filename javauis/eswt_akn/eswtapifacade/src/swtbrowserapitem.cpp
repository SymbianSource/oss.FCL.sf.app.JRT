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



#include <ApAccessPointItem.h>
#include "swtbrowserapitem.h"



CSwtBrowserApItem::CSwtBrowserApItem()
{
}


void CSwtBrowserApItem::ConstructL()
{
    iApItem = CApAccessPointItem::NewLC();
    CleanupStack::Pop(iApItem);
}


EXPORT_C CSwtBrowserApItem* CSwtBrowserApItem::NewL()
{
    CSwtBrowserApItem* self
    = new(ELeave) CSwtBrowserApItem();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}



CSwtBrowserApItem::~CSwtBrowserApItem()
{
    delete iApItem;
}

EXPORT_C CApAccessPointItem& CSwtBrowserApItem::ApItem()
{
    ASSERT(iApItem);
    return *iApItem;
}


