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



#include "swtbrowserdialogsproviderobserver.h"
#include "swtbrowserdialogsproviderobserverproxyprivate.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderObserverProxyPrivate
// ---------------------------------------------------------------------------
//
CSwtBrowserDialogsProviderObserverProxyPrivate::CSwtBrowserDialogsProviderObserverProxyPrivate(
    MSwtBrowserDialogsProviderObserver* aDelegate)
        : CBase()
        , iDelegate(aDelegate)
{
}

// ---------------------------------------------------------------------------
// ~CSwtBrowserDialogsProviderObserverProxyPrivate
// ---------------------------------------------------------------------------
//
CSwtBrowserDialogsProviderObserverProxyPrivate::~CSwtBrowserDialogsProviderObserverProxyPrivate()
{
    // Not own
    iDelegate = NULL;
}

// ---------------------------------------------------------------------------
// CSwtBrowserDialogsProviderObserverProxyPrivate::ReportDialogEventL
// From MSwtBrowserDialogsProviderObserver
// ---------------------------------------------------------------------------
//
void CSwtBrowserDialogsProviderObserverProxyPrivate::ReportDialogEventL(
    TInt aType, TInt aFlags)
{
    if (iDelegate)
    {
        iDelegate->ReportDialogEventL(aType, aFlags);
    }
}
