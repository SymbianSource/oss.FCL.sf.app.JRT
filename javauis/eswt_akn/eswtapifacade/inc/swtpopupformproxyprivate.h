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


#ifndef SWTPOPUPFORMPROXYPRIVATE_H
#define SWTPOPUPFORMPROXYPRIVATE_H


#include <AknUtils.h>
#include <AknPopupform.h> // DOMAIN API


/**
 * CSwtPopupFormProxyPrivate
 * @lib eswtapifacade.dll
 */
class CSwtPopupFormProxyPrivate
        : public CAknPopupForm
{
// Own functions
public:
    static CSwtPopupFormProxyPrivate* NewL(const TTone& aTone,
                                           const TInt aMsgBoxMenuBarResourceId,
                                           const TInt aSwtCmdYes, const TInt aSwtCmdNo, const TInt aTimeOut = 0);
    virtual ~CSwtPopupFormProxyPrivate();

// From CEikDialog
public:
    TInt RunLD();

// From MEikCommandObserver
public:
    void ProcessCommandL(TInt aCommandId);

// From CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif // RD_SCALABLE_UI_V2

// From CEikDialog
protected:
    TInt MappedCommandId(TInt aButtonId);

// From CAknDialog
protected:
    TBool OkToExitL(TInt aButtonId);

// Own functions
private:
    CSwtPopupFormProxyPrivate(const TTone& aTone,
                              const TInt aMsgBoxMenuBarResourceId,
                              const TInt aSwtCmdYes, const TInt aSwtCmdNo, const TInt aTimeOut = 0);

// Data
private:
    const TInt iMsgBoxMenuBarResourceId;
    const TInt iSwtCommandYes;
    const TInt iSwtCommandNo;
    const TInt iTimeOut;
};

#endif // SWTPOPUPFORMPROXYPRIVATE_H
