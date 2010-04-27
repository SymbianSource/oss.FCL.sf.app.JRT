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
* Description:  The CMIDChoiceGroup listbox
*
*/

#ifndef CMIDCHOICEGROUPLISTBOX_H
#define CMIDCHOICEGROUPLISTBOX_H

#include <lcdui.h>
#include <eikclb.h>
// using CMIDChoiceGroupModel as parameter of ConstructL function
#include "CMIDChoiceGroupModel.h"
// CAknColumnListBox inheritance in declaration
// using API in CMIDChoiceGroupControl
#include <aknlists.h>


#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK


class CAknsFrameBackgroundControlContext;
class CMIDChoiceGroupControl;

// Shallow frosting around the CAknColumnListBox cake
NONSHARABLE_CLASS(CMIDChoiceGroupListBox) : public CAknColumnListBox
{
public:
    // c'tor
    CMIDChoiceGroupListBox(CMIDChoiceGroupControl* aChoiceControl);

    // d'tor
    ~CMIDChoiceGroupListBox();

    // Construct with model from outside. Just sets the model
    // and calls CAknColumnListBox::ContructL()
    void ConstructL(
        const CCoeControl* aParent,
        TInt aFlags,
        CMIDChoiceGroupModel* aModel);

public:  // from CCoeControl
    // Overridden, because CEikListBox returns EKeyWasConsumed when
    // up is pressed on the first item, or down on last. We need to
    // return not consumed in order to make the form move focus
    // to other items
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    void SizeChanged();
    void PositionChanged();
    void Draw(const TRect& aRect) const;

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);
    void HandleResourceChange(TInt aType);

#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif

#ifdef RD_TACTILE_FEEDBACK
private:
    MTouchFeedback* iFeedback;
#endif

protected:
    // Overridden to use the CMIDChoiceGroupModel instead of the
    // normal listbox model. Note that this model is owned outside
    // the listbox, and must not be deleted by the box
    virtual void CreateModelL();

private:
    void SetTextColors();

private:
    CMIDChoiceGroupControl* iChoiceControl;

#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool iUpEventSent;
#endif // RD_JAVA_S60_RELEASE_9_2
};


#endif // CMIDCHOICEGROUPLISTBOX_H
