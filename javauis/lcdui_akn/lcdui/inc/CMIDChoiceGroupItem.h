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
* Description:  CMIDChoiceGroupItem implements the MID Form ChoiceGroup item
*
*/

#ifndef CMIDCHOICEGROUPITEM_H
#define CMIDCHOICEGROUPITEM_H

#include <eiktxlbx.h>
#include <eiktxlbm.h>
#include <eikclb.h>
#include <gulicon.h>

#include <aknlists.h>

#include "CMIDForm.h"
#include "CMIDCommand.h"
#include "CMIDControlItem.h"
// for MMIDChoiceGroupControlObserver interface in class declaration
#include "CMIDChoiceGroupControl.h"


// Class CMIDChoiceGroupItem
NONSHARABLE_CLASS(CMIDChoiceGroupItem) :
        public CMIDControlItem,
        public MMIDChoiceGroup,
        public MMIDChoiceGroupControlObserver
{
public:
    /**
     * Double ctor
     * @param aMIDEnv Environment
     * @return CMIDChoiceGroupItem instance
     *
     * @since s60
     */
    static CMIDChoiceGroupItem* NewL(
        MMIDEnv& aMIDEnv, const TDesC& aLabel, TInt aType,
        RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray, CMIDUIManager* aUIManager);

    ~CMIDChoiceGroupItem();

private:
    CMIDChoiceGroupItem(
        MMIDEnv& aMIDEnv,
        TInt aType,
        RArray<TPtrC>& aStringArray,
        RArray<MMIDImage*>& aImageArray,
        CMIDUIManager* aUIManager);

    void ConstructL(const TDesC& aLabel);

public:
//    void Draw(const TRect& aRect) const;
    void UpdateCommands();

public: // from MMIDChoiceGroup
    void InsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage);
    void DeleteElementL(TInt aIndex);
    void DeleteAllL();
    void SetElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage);
    void SelectElementL(TInt aIndex, TBool aSelected);
    TBool IsSelected(TInt aIndex);
    void SetFontL(TInt aIndex, MMIDFont* aFont);
    void SetFitPolicyL(TInt aFitPolicy);

public: // from MMIDItem
    void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    TBool IsSelectable() const
    {
        return ETrue;
    }
    void SetDefaultCommand(MMIDCommand* aCommand);
    TBool ProcessCommandL(CMIDCommand* aCommand);
    void HandleCurrentL(TBool aCurrent);

public: // from MMIDComponent
    void Dispose();
    inline TType Type() const
    {
        return EChoiceGroup;
    }

public: // from CCoeControl
    void SetContainerWindowL(const CCoeControl& aContainer);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void SizeChanged();
    TSize MinimumSize();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    // Responds to focus change.
    void FocusChanged(TDrawNow aDrawNow);

#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

public: // from CMIDControlItem
    // Returns a rect that should be partially visible for an Item to have focus
    virtual TRect FocusableRect();
    void ResolutionChange(TInt aType);
    void ColorChange(TInt aType);
    void RestoreInnerFocus();

public: // from MMIDChoiceGroupControlObserver
    virtual void HandleChoiceGroupControlEventL(
        CMIDChoiceGroupControl* aControl,
        TChoiceGroupControlEvent aEvent,
        TAny* aParam);

public: // From CMIDItem
    // Returns the item's preferred height without the label part
    TInt ItemPreferredHeightWithoutLabel();

    // Overridden, because the choice listbox needs to be created
    // only when adding the item to a form (the listbox very much
    // likes to have a parent window when created)
    void ItemAddedToFormL();
    void ItemRemovedFromForm();
    void NotifyScrollingCompleted();

    // from CMIDControlItem
    void PostFocusTransferEvent(TBool aFocus,CMIDForm::TDirection aDirection);

public: // New stuff (for control use mainly)
    // Retrieves the rect of the form where this item is
    TRect FormRect();
    void PostItemStateChangedEventL();

#ifdef RD_JAVA_S60_RELEASE_9_2
public:
    void PostPendingUpEventL();
    TBool LongTapDetected();
#endif // RD_JAVA_S60_RELEASE_9_2

private:
    void SafeDrawNow();
    void CreateChoiceGroupControlL(CCoeControl* aParent);
    void CalcChildSizes(
        const TRect& aItemRect,
        TRect& aLabelRect,
        TRect& aChoiceRect);

private:
    // Saved parameters
    MMIDChoiceGroup::TChoiceType iChoiceType;
    MMIDChoiceGroup::TFitPolicy iFitPolicy;

    MMIDEnv& iMIDEnv;

    // The actual control used in this item
    CMIDChoiceGroupControl* iChoiceGroupControl;

    CMIDCommand* iSelectCommand;
    CMIDCommand* iMarkCommand;
    CMIDCommand* iUnmarkCommand;

    // Saved initialization array pointers
    RArray<TPtrC>& iStringArray;
    RArray<MMIDImage*>& iImageArray;

#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool           iLongTapDetected;
    TPointerEvent   iPendingUpEvent;
#endif // RD_JAVA_S60_RELEASE_9_2
};

#endif // CMIDCHOICEGROUPITEM_H
