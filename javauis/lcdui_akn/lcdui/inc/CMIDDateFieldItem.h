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


#ifndef CMIDDATEFIELDITEM_H
#define CMIDDATEFIELDITEM_H

// using CEikTTimeEditor as memeber iEditor
#include <eikmfne.h>
#include <lcdui.h>

// MTickerObserver interface in class declaration
#include "CMIDTicker.h"
// CMIDControlItem inheritance in class declaration
#include "CMIDControlItem.h"
#include "CMIDCommand.h"

NONSHARABLE_CLASS(CMIDDateFieldItem) : public CMIDControlItem,
        public MMIDDateField, public MMIDTickerObserver
{
public:
    /**
     * Double ctor
     * @param aLabel Label
     * @param aInputMode Input mode
     * @param aUIManager UI manager
     * @return CMIDDateFieldItem instance
     *
     * @since s60
     */
    static CMIDDateFieldItem* NewL(
        const TDesC& aLabel, MMIDDateField::TInputMode aInputMode, CMIDUIManager* aUIManager);

    virtual ~CMIDDateFieldItem();
    //
    // From MMIDDateField
    //
    TTime Date() const;
    void SetDate(const TTime& aTime);
    void SetUninitialized();
    void SetInputModeL(MMIDDateField::TInputMode aInputMode);
    //
    // From MMIDItem
    //
    void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);
    TBool IsSelectable() const;
    TBool ProcessCommandL(CMIDCommand* aCommand);
    TSize ResetPreferredSize() const;
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
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void SetContainerWindowL(const CCoeControl& aContainer);
    virtual void FocusChanged(TDrawNow /*aDrawNow*/);
    TCoeInputCapabilities InputCapabilities() const;
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    //
    // From CMIDControlItem
    //
    void ResolutionChange(TInt aType); // this is called when resolution is changed
    void AdjustToSizeL(const TSize& aSize);
    void CursorUpdate();
private: // methods
    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDDateFieldItem(CMIDUIManager* aUIManager);
    void ConstructL(const TDesC& aLabel,MMIDDateField::TInputMode aInputMode);
    void SetInitialized(TInt aSetCurrentTime = ETrue);
    void  DoSafeDraw();
    TBool IsDateTimeNonEmpty() const;

    /**
     * Resets the commands of this form item to expose the functions that
     * are currently available. Should be called whenever the focus inside
     * the editor changes.
     **/
    void  UpdateCommands();

    /**
     * Returns ETrue if the current field in the time editor is am/pm field.
     **/
    TBool IsCurrentFieldAmPmField();

    /**
     * Returns ETrue if the field indicated by the fieldIndex in the
     * time editor is the am/pm field. (Assumes that there is only one)
     **/
    TBool IsFieldAmPmField(TInt fieldIndex);

    /**
     * Toggles the value of the am/pm field in this editor from the current
     * value to the other value (am -> pm, pm -> am). If am/pm field is not
     * found, nothing is done.
     **/
    void  ToggleAmPmFieldValue();

    /**
     * Updates the LAF related member variables
     **/
    void UpdateMemberVariables();

private: // data

    CEikTTimeEditor* iEditor;
    TBool iInitialised;
    TBool prevInitialised;

    MMIDDateField::TInputMode iInputMode;
    CMIDCommand* iAmPmToggleCommand; // command to be shown in MSK to toggle am/pm value

    TMargins iMargins;
    TInt iEditorHeight;
    const TKeyEvent* usedKeyEvent;

#if defined( RD_SCALABLE_UI_V2)
    // The pointer grabbing component control that received the pointer down event. May be NULL.
    // Need to keep track of the grabbing control inside datefield because in long tap or canceled long tap
    // cases datefield does not forward the up event to the component controls. Therefore the grabbing state
    // of the component controls may be wrong inside CCoeControl implementation.
    CCoeControl* iGrabbingControl;
#endif //if defined( RD_SCALABLE_UI_V2)
};


#endif // CMIDDATEFIELDITEM_H
