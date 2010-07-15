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


#ifndef CMIDTEXTFIELDITEM_H
#define CMIDTEXTFIELDITEM_H

// for MEikEdwinObserver and MEikEdwinSizeObserver interfaces in class declaration
#include <eikedwob.h>   // for MEikEdwinSizeObserver
// CMIDControl item inheritance in class declaration
#include "CMIDControlItem.h"
#include "lcdui.h"

class CMIDEdwin;
class CAknEdwinDrawingModifier;

NONSHARABLE_CLASS(CMIDTextFieldItem) : public CMIDControlItem, public MMIDTextField,
        public MEikEdwinSizeObserver, public MEikEdwinObserver
{
public:

    /**
     * Double ctor
     * @param aLabel Label
     * @return MMIDTextField instance
     *
     * @since s60
     */
    static MMIDTextField* NewL(
        const TDesC& aLabel, const TDesC& aText,
        TInt aConstraints, TInt aMaxSize, CMIDUIManager* aUIManager);

    ~CMIDTextFieldItem();
    //
    // From MMIDTextField
    //
    void DeleteTextL(TInt aOffset,TInt aLength);
    void SetTextL(const TDesC& aText);
    void InsertTextL(const TDesC& aText,TInt aPosition);
    void SetConstraintsL(TUint aConstraints);
    TInt SetMaxSizeL(TInt aMaxSize);
    TInt GetMaxSize();
    TInt Size();
    TInt GetCaretPosition();
    HBufC* GetTextL();
    void SetInitialInputModeL(const TDesC& aCharacterSubset);
    //
    // From MMIDItem
    //
    void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    TSize ResetPreferredSize() const;
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);
    //
    // From MMIDComponent
    //
    void Dispose();
    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    void SetContainerWindowL(const CCoeControl& aContainer);
    void FocusChanged(TDrawNow /*DrawNow*/);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    TCoeInputCapabilities InputCapabilities() const;
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    void MakeVisible(TBool aVisible);

    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    TRect FocusableRect();
    void HandleCurrentL(TBool aCurrent);

    //
    // From CMIDControlItem
    //
    void ResolutionChange(TInt aType);
    void PostFocusTransferEvent(TBool aFocus, CMIDForm::TDirection aDirection);
    void RestoreInnerFocus();
    //
    // From MEikEdwinSizeObserver
    //
    TBool HandleEdwinSizeEventL(CEikEdwin* /*aEdwin*/,
                                TEdwinSizeEvent /*aEventType*/, TSize aDesirableEdwinSize);
    //
    // From MEikEdwinObserver
    //
    void HandleEdwinEventL(CEikEdwin* /*aEdwin*/,TEdwinEvent /*aEventType*/);
    //
    // TextField implements MCoeControlObserver and calls our HandleControlEventL on EEventStateChanged
    //
    void HandleControlEventL(CCoeControl* aSource, MCoeControlObserver::TCoeEvent aEventType);
    //
    void DoHandleEdwinEventL();
    // returns ETrue if successful, sets aPoint the xy coordinates of the cursor
    TBool GetCursorPosL(TPoint& aPoint);

private:
    void CreateTextFieldL(const TDesC& aText, TUint aConstraints, TInt aMaxSize);

    /**
     * Update the layout for the Edwin control relative to the rect where
     *   the Edwin will be laid out, and the top margin, and the height
     *
     * @note Updates <code>iEdwinMarginTop</code>,
     *   <code>iEdwinMinHeight</code>,
     */
    void UpdateLayout();

    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDTextFieldItem(CMIDUIManager* aUIManager);

    void ConstructL(const TDesC& aLabel,const TDesC& aText,TInt aConstraints,TInt aMaxSize);
    CMIDEdwin* TextComponent() const;

    /**
     * Updates color for text in editor when focus is changed.
     */
    void UpdateTextColorsL();

public:
    /**
     * Sets the cursor visiblity for the text field (=edwin) when this item is focused.
     * If the item is not focused, this method does nothing.
     *
     * @param aVisible  Defines the visibility of the cursor.
     * @since S60 5.0
     */
    void SetCursorVisibility(TBool aVisible);
private:
    CMIDEdwin*                  iTextField;
    HBufC*                      iText;
    CAknEdwinDrawingModifier*   iDrawingModifier;
    TUint8                      iEdwinMarginTop;
    TUint8                      iEdwinMinHeight;

};

#endif // CMIDTEXTFIELDITEM_H
