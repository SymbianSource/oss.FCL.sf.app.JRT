/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDCONTROLITEM_H
#define CMIDCONTROLITEM_H

#include <gdi.h>
#include <coecntrl.h>
#include <fepbase.h>    // for MCoeCaptionRetrieverForFep

// for CMIDForm::TDirection as parameter of PostFocusTransferEvent function
#include "CMIDForm.h"
// CMIDItem inheritance in class declaration
#include "CMIDItem.h"

class CMIDItemLabel;
class MAknsControlContext;
class CAknsFrameBackgroundControlContext;

NONSHARABLE_CLASS(CMIDControlItem) : public CMIDItem, public MCoeCaptionRetrieverForFep
{
public:
    virtual ~CMIDControlItem();
    //
    // From CMIDItem
    //
    virtual TBool IsSelectable() const;
    virtual void SetLabelL(const TDesC& aLabel);

    virtual TInt LabelHeight() const;
    inline void SetSizeQuiet(const TSize& aSize);   // won't call SizeChanged
    inline void SetPositionQuiet(const TPoint& aPosition);   // won't call PositionChanged


    // Returns height of a label that has only one line of text
    TInt OneLineLabelHeight() const;

    // from CCoeControl
    virtual void SetContainerWindowL(const CCoeControl& aContainer);
    void SetFocus(TBool aFocus, TDrawNow aDrawNow = ENoDrawNow);
    virtual void FocusChanged(TDrawNow aDrawNow);
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

    // From MCoeCaptionRetrieverForFep
    void GetCaptionForFep(TDes& aCaption) const;

    void SetLabelColor(CMIDItemLabel* aLabelControl);

    //
    // Resets the preferred size. This is called on all items at the beginning of form layout.
    // This means we can fiddle with the preferred size of items during layout.
    // Returns the new preferred size.
    //
    virtual TSize ResetPreferredSize() const;
    virtual void AdjustToNewWidthL(TInt aWidth);
    virtual void AdjustToSizeL(const TSize& aSize);
    inline virtual CMIDItemLabel* LabelControl();

    virtual TRect FocusableRect(); // returns a rect that should be partially visible for an Item to have focus

    virtual void ResolutionChange(TInt aType); // this is called when resolution is changed
    virtual void ColorChange(TInt aType); //this is called when there is a skin or color scheme change

    /**
     * @return ETrue if label exists and is not empty
     */
    TBool HasLabel();

    /**
     * The function is being invoked after form has changed focus of the component.
     * Subclasses of CMIDControlItem may override the default implementation to be notified about the focus transfer.
     * @param aFocus true if the component has gained focus
     * @param aDirection TDirection
     */
    virtual void PostFocusTransferEvent(TBool aFocus,CMIDForm::TDirection aDirection);

    /**
     * The method is invoked from CMIDForm::HandleResourceChange when there is a need
     * to restore the focus inside the item (if the item has its own subitems).
     * (For instance there is focus on ChoiceGroupItem inside the form with inner focus
     * to some sub-item. After the resolution change the main focus on form item is restored,
     * but focused sub-item would be below the screen, so this method is called in order
     * to restore focus on sub-item.)
     * Every form item, which is able to focus its own internal sub-items should
     * implement this method.
     */
    virtual void RestoreInnerFocus();

    /**
      * This method is called when the window server
      * fades or unfades the current window
      * @param aFaded true if the window is goiging to be faded, false if unfaded
      */
    virtual void HandleWindowFade(TBool aFaded);

protected:
    CMIDControlItem(MMIDItem::TLayout aLayout, CMIDUIManager* aUIManager);
    void ConstructL();

    TInt FormClientAreaWidth() const; // Width of form2_midp_field_pane (scroll bar excluded)
    TRect FormClientAreaRect() const; // Layout of Rect of form2_midp_field_pane (scroll bar excluded)
    TInt ItemContentBottomMargin() const; // Bottom margin of form2_midp_content_pane

public:
    inline TBool GetVisibilityInForm();
    inline void SetVisibilityInForm(const TBool aVisibility);

protected:
    CCoeEnv* iCoeEnv;
    CMIDItemLabel* iLabelControl; // The label for the control

    //The background control context for when the item is highlighted
    CAknsFrameBackgroundControlContext* iHighlightedBackgroundCc;

    TBool iVisibilityInForm;
};

inline void CMIDControlItem::SetSizeQuiet(const TSize& aSize)
{
    iSize = aSize;
}

inline void CMIDControlItem::SetPositionQuiet(const TPoint& aPosition)
{
    iPosition = aPosition;
}

inline CMIDItemLabel* CMIDControlItem::LabelControl()
{
    return iLabelControl;
}

inline TBool CMIDControlItem::GetVisibilityInForm()
{
    return iVisibilityInForm;
}

inline void CMIDControlItem::SetVisibilityInForm(const TBool aVisibility)
{
    iVisibilityInForm = aVisibility;
}

#endif // CMIDCONTROLITEM_H
