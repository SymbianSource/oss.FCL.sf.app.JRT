/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manages popups
*
*/


#ifndef CMIDPOPUPNOTECONTROLLER_H
#define CMIDPOPUPNOTECONTROLLER_H

#include <e32base.h>
#include <lcdui.h>

class CAknInfoPopupNoteController;

// normal way to show (delay 1 second, show 5 seconds )
/* Time the popup will be visible in ms */
const TInt KPopupNoteVisibleDelayMs = 1000; // 1 second in ms
/* Time the popup will be visible in ms */
const TInt KPopupNoteVisibleTimeMs = 5000; // 5 seconds in ms

// fast way to show (delay 1 second, show 1 second ), used for example, when scrolling
/* Time the popup will be visible in ms */
const TInt KPopupNoteShortVisibleDelayMs = 1000; // 1 seconds in ms
/* Time the popup will be visible in ms */
const TInt KPopupNoteShortVisibleTimeMs = 5000; // 5 seconds in ms

NONSHARABLE_CLASS(CMIDPopupNoteController): public CBase
{
public:
    //Public constructor an destructor
    static CMIDPopupNoteController* NewL();
    virtual ~CMIDPopupNoteController();

public:
    /**
     * Brings popup with text. The popup is shown for KPopupNoteVisibleTimeMs (5 seconds)
     *
     * @param aText The text inside the popup.
     * @param aRect a rectangle for the popup
     * @param isShortPopup if ETrue then time is decreased to KPopupNoteShortVisibleTimeMs
     */
    void ShowPopupL(const TDesC& aText, const TRect aRect, TBool isShortPopup=EFalse);
    /**
     * Hides popup
     */
    void HidePopup();

private:
    //Private two phase constructor and destructor
    CMIDPopupNoteController();
    void ConstructL();

private:
    CAknInfoPopupNoteController* iPopupController;
};

#endif // CMIDPOPUPNOTECONTROLLER_H
