/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for TextField
*
*/


#ifndef CMIDTEXTFIELDEDWIN_H
#define CMIDTEXTFIELDEDWIN_H

// ========== INCLUDE FILES ================================

// CMIDEdwin inheritance in class declaration
#include "CMIDEdwin.h"

// ========== FORWARD DECLARATIONS =========================

// ========== CLASS DECLARATION ============================

NONSHARABLE_CLASS(CMIDTextFieldEdwin) : public CMIDEdwin, public MCoeControlObserver
{
public:
    static CMIDTextFieldEdwin* NewL(TInt aConstraints,const TDesC& aText,TInt aMaxSize,
    MMIDDisplayable* aDisplayable, CMIDTextFieldItem* aTextFieldItem=NULL);
    ~CMIDTextFieldEdwin();

    //
    // From MCoeControlObserver
    //
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

    //
    // From CCoeControl
    //
    void SetContainerWindowL(const CCoeControl& aContainer);

    // from CMIDEdwin
    /**
     * Layouts the CMIDTextFieldEdwin relative to the parent.
     *
     * @param aRect the parent rect
     * @param aLayout multi line layout info
     */
    virtual void DoLayout(const TRect& aRect);

    void PositionChanged();
protected: // from CMIDEdwin
    void AddCommandsToEdwinL(RPointerArray<MMIDCommand>& aArray);

private: // Constructors
    /**
     * Constructor
     */
    CMIDTextFieldEdwin(TUint aConstraints, MMIDDisplayable* aDisplayable,
                       CMIDTextFieldItem* aTextFieldItem);

    /**
     * Second phase constructor
     * @param aText a text to be set in the field
     * @param aMaxSize maximum number of characters to allow
     */
    void ConstructL(const TDesC& aText, TInt aMaxSize);

#ifdef RD_SCALABLE_UI_V2
    /**
     * Calculate the rect for scrolling TextField text internally
     * @param   aParentRect parent rect from which to calculate scrolling area
     * @param   aLine   number of lines to deduct from scrolling
     * @param   aLayout multi line text layout
     * @return  rect that can be scrolled
     */
    TRect LineRect(TRect aParentRect, TInt aLine, TAknMultiLineTextLayout aLayout);
    /**
     * Set the rect for scrolling TextField text internally
     * @param   aEdwin  the editor
     * @param   aParentRect parent rect from which to calculate scrolling area
     * @param   aLayout multi line text layout
     * @param   aNumberOfLines  number of laid out lines
     */
    void SetThisScrollRect(CMIDTextFieldEdwin *aEdwin, TRect aParentRect, TAknMultiLineTextLayout aLayout, TInt aNumberOfLines);

#endif // RD_SCALABLE_UI_V2

    /**
    * Get the layout for the Edwin control relative to the parent rect where
    * the Edwin will be laid out
    *
    * @param aRect the parent rectangle where the Edwin is laid out
    * @param aNumLines how many lines will be displayed
    *
    * @return multi line layout structure
    * @note it changes the supplied rectangle leaving the margins for form2_midp_text_pane
    */
    TAknMultiLineTextLayout GetLayout(TRect& aRect, TInt aNumLines) const;

};

#endif // CMIDTEXTFIELDEDWIN_H
