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



#ifndef SWTBROWSERDIALOGSPROVIDEROBSERVERPROXYPRIVATE_H
#define SWTBROWSERDIALOGSPROVIDEROBSERVERPROXYPRIVATE_H


#include <AknUtils.h>
#include <browserdialogsproviderobserver.h> // DOMAIN API


class MSwtBrowserDialogsProviderObserver;


/**
 * CSwtBrowserDialogsProviderObserverProxyPrivate
 * @lib eswtapifacade.dll
 */
class CSwtBrowserDialogsProviderObserverProxyPrivate
        : public CBase
        , public MBrowserDialogsProviderObserver
{
// Own functions
public:
    CSwtBrowserDialogsProviderObserverProxyPrivate(
        MSwtBrowserDialogsProviderObserver* aDelegate);
    virtual ~CSwtBrowserDialogsProviderObserverProxyPrivate();

// From MBrowserDialogsProviderObserver
public:
    void ReportDialogEventL(TInt aType, TInt aFlags);

// Data
private:
    MSwtBrowserDialogsProviderObserver* iDelegate; // not own
};

#endif // SWTBROWSERDIALOGSPROVIDEROBSERVERPROXYPRIVATE_H
