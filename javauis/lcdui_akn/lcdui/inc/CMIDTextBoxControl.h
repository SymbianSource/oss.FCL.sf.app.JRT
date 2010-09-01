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
* Description:  ?Description
*
*/


#ifndef CMIDTEXTBOXCONTROL_H
#define CMIDTEXTBOXCONTROL_H

// ========== INCLUDE FILES ================================

#include <coecntrl.h>

// CMIDEdwin for iTextBox
#include "CMIDEdwin.h"

class CMsgEditorCustomDraw;

// ========== CLASS DECLARATION ============================

NONSHARABLE_CLASS(CMIDTextBoxControl) : public CCoeControl, public MMIDTextBox
{
public:
    static CMIDTextBoxControl* NewL(TInt aConstraints,const TDesC& aText,
    TInt aMaxSize,MMIDDisplayable* aDisplayable);
    ~CMIDTextBoxControl();

private:
    CMIDTextBoxControl();
    void ConstructL(TInt aConstraints,const TDesC& aText,
                    TInt aMaxSize,MMIDDisplayable* aDisplayable);
    TUint iConstraints;
    const TKeyEvent* iUsedKeyEvent;
public:
    //
    // From MMIDTextBox
    //
    void DeleteTextL(TInt aOffset,TInt aLength);
    void SetTextL(const TDesC& aText);
    void InsertTextL(const TDesC& aText,TInt aPosition);
    void SetConstraintsL(TUint aConstraints);
    TInt SetMaxSizeL(TInt aSize);
    TInt GetMaxSize();
    TInt Size();
    TInt GetCaretPosition();
    HBufC* GetTextL();
    void SetInitialInputModeL(const TDesC& aCharacterSubset);
    //
    // From MMIDComponent
    //
    inline CBase* Ptr();
    inline void Dispose();
    //
    // From CCoeControl
    //
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt) const;
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void FocusChanged(TDrawNow aDrawNow);
    void ActivateL();
    //
    void HandleCurrentL(TBool aActivate);
    void SizeChanged();
    void HandleResourceChange(TInt aType);

private:
    CMIDEdwin* CreateTextBoxL(const TDesC& aText, TInt aMaxSize, TUint aConstraints);
    void RecreateTextBoxL(TUint aConstraints);

    /**
    * Layouts the scrollbar
    *
    * @param aRect the area for <code>CMIDTextBoxControl</code>
    */
    void LayoutScrollBar(const TRect& aRect);

protected:
    CMIDDisplayable*    iDisplayable;
    CMIDEdwin*          iTextBox;  // own
    TInt                iCursorPos;
};

inline CBase* CMIDTextBoxControl::Ptr()
{
    return this;
}

inline void CMIDTextBoxControl::Dispose()
{
    delete this;
}

#endif // CMIDTEXTBOXCONTROL_H
