/*******************************************************************************
 * Copyright (c) 2006, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef ESWTMOBILEEXTENSIONS_H
#define ESWTMOBILEEXTENSIONS_H


#include "eswt.h"
#include "eswtwidgetscore.h"


class MSwtMobileShell
{
public:
    /**
     * Add the scan code the array of keys pressed.
     */
    virtual void AddKeyStateL(TInt aScanCode) =0;

    /**
     * Change the trim/style of the MSwtMobileShell
     */
    virtual void ChangeTrimL(TInt aStyle, TInt aStatusStyle) =0;

    /**
     * Returns a pointer to the shell member.
     */
    virtual const MSwtShell* Shell() const =0;

    /**
     * Returns ETrue if the key corresponding to aSwtKeyCode
     * has been pressed since the last call to GetKeyState();
     * If the current MSwtMobileShell is not
     * the active MSwtMobileShell.
     */
    virtual TBool GetKeyState(TInt aSwtKeyCode) =0;

    /**
     * Set the text displayed in the status pane.
     */
    virtual void SetStatusTextL(const TDesC& aText) =0;

    /**
     * Get the text displayed in the status pane.
     */
    virtual const TDesC* GetStatusText() const =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.Input class.
 */
class MSwtInput
{
public:
    enum TInputType
    {
        EUnknownInput,      // Unknown input
        ESoftKeys,          // Input device has one or more hardware keys whose meaning can be configured
        EKeyPad,            // Input device has hardware keys labeled 0 - 9, *, and #
        ELimitedKeyboard,   // Input device has more hardware keys than a keypad, but fewer than a full keyboard
        EFullKeyboard       // Input device has hardware keys typical in a full keyboard
    };

protected:
    virtual ~MSwtInput() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of ( i.e. destroys ) the Input
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the location of the input device.
     * @return The location type of the input.
     */
    virtual TInt GetLocation() =0;

    /**
     * Returns the type of the input device.
     * @return The type of the input.
     */
    virtual TInt GetType() =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.Screen class.
 */
class MSwtScreen
{
public:
    enum TScreenType
    {
        EPrimary = 1,   // One primary screen may be active at a time.
        ESecondary,     // Multiple secondary screens may be simultaneously active.
        EStatus         // Sub type of secondary screen that shows minimal content for notification purposes.
    };

    enum TOrientationType
    {
        EPortrait = 0,  // Indicates text is normally written across the shortest width.
        ELandscape      // Indicates text is normally written across the longest width.
    };

    /**
     * Differents values of the EventType.
     */
    enum TScreenEvent
    {
        KEventScreenActivated = 1, // Event type when the screen is actived.
        KEventScreenDeactivated, // Event type when the screen is desactived.
        KEventScreenOrientationChanged // Event type when the orientation screen change.
    };

protected:
    virtual ~MSwtScreen() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Disposes of ( i.e. destroys ) the Screen
     * @return The Java peer associated with this object
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the bounds of the screen.
     * @return The bounds of the screen
     */
    virtual TRect GetBounds() =0;

    /**
     * Returns the color depth of the screen in bits per pixel.
     * @return The color depth.
     */
    virtual TInt GetColorDepth() =0;

    /**
     * Returns the location of the screen.
     * @return The location type of the screen.
     *         The value is one of MSwtMobileDevice::TLocationType
     */
    virtual TInt GetLocation() =0;

    /**
     * Returns the screen orientation.
     * @return The screen orientation. The value is a TOrientationType.
     */
    virtual TInt GetOrientation() =0;

    /**
     * Returns the usage type of the screen.
     * @return The type of the screen. The value is a TScreenType
     */
    virtual TInt GetUsage() =0;

    /**
     * Returns whether the screen is currently active.
     * That is, whether it is powered up and contents are visible to the user.
     * @return The screen's status.
     */
    virtual TBool IsActive() =0;

    /**
     * Returns whether the screen has touch screen capability.
     * @return The touch capability
     */
    virtual TBool IsTouchScreen() =0;

    /**
     * Sets the screen orientation.
     * @param aOrientation The new orientation.
     */
    virtual void SetOrientationL(TInt aOrientation) =0;
};


/**
 * MSwtMobileDeviceObserver
 */
class MSwtMobileDeviceObserver
{
public:
    virtual void ScreenActivatedL(TInt aScreenId) = 0;
    virtual void ScreenDeactivatedL(TInt aScreenId) = 0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.MobileDevice class.
 */
class MSwtMobileDevice
{
public:
    enum TLocationType
    {
        ELocal,     // Feature which is permanently attached to the mobile device and always available
        EInternal,  // Local feature which is only available when mobile device is opened
        EExternal,  // Local feature which is only available when mobile device is closed
        ERemote     // Feature which is not part of the mobile device unless connected via wire or wireless signal
    };

    enum TEventType
    {
        EEventOpened  = 1,
        EEventClosed  = 2,
        EEventAdded   = 3,
        EEventRemoved = 4
    };

    enum TAlertLevel
    {
        EAlertLowImportance  = 1,
        EAlertHighImportance = 2,
        EAlertCritical       = 3
    };

protected:
    virtual ~MSwtMobileDevice() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Add observer
     */
    virtual TBool AddObserverL(const MSwtMobileDeviceObserver* aObserver) = 0;

    /**
     * Alerts the user using device specific mechanisms such as sounds,
     * flashing, or vibration.
     * aLevel must be one of ALERT_LOW_IMPORTANCE,
     * ALERT_HIGH_IMPORTANCE or ALERT_CRITICAL. The
     * method maps these levels to device specific features that may also be
     * dependent upon user configuration or current profile.
     * @param aLevel Constant describing the importance of the alert.
     */
    virtual void Alert(TAlertLevel aLevel) =0;

    /**
     * Disposes of ( i.e. destroys ) the mobile device.
     * @return The Java peer associated with this object.
     */
    virtual TSwtPeer Dispose() =0;

    /**
     * Returns the number of Input objects available on the device.
     * @return This number.
     */
    virtual TInt GetInputNumber() const =0;

    /**
     * Returns the number of Screen objects available on the device.
     * @return This number.
     */
    virtual TInt GetScreenNumber() const =0;

    /**
     * Returns whether device is opened.
     * For devices that have no internal screens or input, the  method always returns ETrue.
     * @return The device's open status.
     */
    virtual TBool IsOpen() const = 0;

    /**
     * Remove observer
     */
    virtual TBool RemoveObserver(const MSwtMobileDeviceObserver* aObserver) = 0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.CaptionedControl class.
 * A class that implements MSwtCaptionedControl must also implement
 * MSwtComposite.
 */
class MSwtCaptionedControl
{
protected:
    virtual ~MSwtCaptionedControl() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() const =0;

    /**
     * Sets the caption label.
     * @param aString The caption label.
     */
    virtual void SetTextL(const TDesC& aString) =0;

    /**
     * Sets the trailing label.
     * @param aString The trailing label.
     */
    virtual void SetTrailingTextL(const TDesC& aString) =0;

    /**
     * Gets the caption label.
     * @return The caption label.
     */
    virtual const TDesC* GetText() const =0;

    /**
     * Gets the trailing label.
     * @return The trailing label.
     */
    virtual const TDesC* GetTrailingText() const =0;

    /**
     * Sets an image as the icon.
     * @param aImage The image ( may be NULL ).
     */
    virtual void SetImageL(MSwtImage* aImage) =0;

    /**
     * Sets the preferred size of CaptionedControl's child
     * @param aSize The child's size.
     */
    virtual void SetChildPreferredSize(const TSize& aSize) =0;

    /**
     * Returns the image if the Captioned control has one set.
     * @return CCoeControl pointer to the image or NULL if there is no image.
     */
    virtual const CCoeControl* GetImageControl() const =0;
};


/**
 * Interface class for the internal pager of the
 * org.eclipse.ercp.swt.mobile.MultiPageDialog class.
 *
 */
class MSwtMultiPagePager
{
protected:
    virtual ~MSwtMultiPagePager() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtComposite interface
     */
    virtual MSwtComposite* Composite() =0;

    /**
     * Displays the page specified by its index.
     * A Selection event is post if the current page is changed.
     * @param aIndex The 0-based index of the page to display.
     *               The first page is at the index 0;
     */
    virtual void DisplayPage(TInt aIndex) =0;

    /**
     * Gets the margins of the pager.
     */
    virtual TMargins GetPagerMargins() const =0;

    /**
     * Gets the margins of the MultiPageDialog.
     */
    virtual TMargins GetDialogMargins() const =0;
};


/**
 * Interface class for the multipage tab group of the
 * org.eclipse.ercp.swt.mobile.MultiPageDialog class.
 *
 */
class MSwtMultiPageTabGroup
{
protected:
    virtual ~MSwtMultiPageTabGroup() {} // Made protected to prevent destruction through the interface

public:

    /**
     * Creates a new tab item.
     * @param aTitle The title of the new tab item.
     * @param aImage Image or icon of the tab item. Not used.
     * @return The 0-based index of the active page.
     *         The first page is at the index 0;
     */
    virtual TInt AddPageL(const TDesC& aTitle, MSwtImage* aImage) =0;

    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() =0;

    /**
     * Removes a page.
     * @param aIndex The 0-based index of the page to remove.
     *               The first page is at the index 0;
     * @return The 0-based index of the active page.
     *         The first page is at the index 0;
     */
    virtual TInt RemovePage(TInt aIndex) =0;

    /**
     * Changes the active page to display.
     * @param aIndex The 0-based index of the page to be active.
     *               The first page is at the index 0;
     * @return The 0-based index of the active page.
     *         The first page is at the index 0;
     */
    virtual TInt SelectPage(TInt aIndex) =0;

    /**
     * Sets the pager the MSwtMultiPageTabGroup
     * @return aPager A pointer to the pager.
     */
    virtual void SetPager(MSwtMultiPagePager* aPager) =0;

    /**
     * Sets the title/label of a tab item.
     * @param aIndex The index of the tab item.
     * @param aText  The title
     */
    virtual void SetTabTitleL(TInt aIndex, const TDesC& aText) =0;
};


/**
 * Interface class for the
 * org.eclipse.ercp.swt.mobile.MultiPageDialog class.
 */
class MSwtMultiPageDialog
{
protected:
    virtual ~MSwtMultiPageDialog() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() =0;

    /**
     * Closes the MultiPageDialog.
     */
    virtual void Close() =0;

    /**
     * Creates a new tab item.
     * @param aComposite The composite for this page.
     * @param aTitle The title of the new tab item.
     * @param aImage Image or icon of the tab item. Not used.
     */
    virtual void CreatePageL(MSwtComposite* aComposite, const TDesC& aTitle, MSwtImage* aImage) =0;

    /**
     * Deletes a page.
     * @param aIndex The 0-based index of the page to remove.
     *        The first page is at the index 0;
     * @return The 0-based index of the active page.
     *        The first page is at the index 0;
     */
    virtual void DeletePageL(TInt aIndex) =0;

    /**
     * Returns the index of the selected page.
     */
    virtual TInt GetSelectedIndex() =0;

    /**
     * Opens a MultiPageDialog
     */
    virtual void OpenL() =0;

    /**
     * Changes the active page to display.
     * @param aIndex The 0-based index of the page to be active.
     *        The first page is at the index 0;
     */
    virtual void SetSelection(TInt aIndex) =0;

    /**
     * Sets the text and image of a tab item.
     */
    virtual void SetTitleAndImageL(TInt aIndex, const TDesC& aText, MSwtImage* aImage) =0;

    /**
     * Sets the title/label of a tab item.
     * @param aIndex The index of the tab item.
     */
    virtual void SetTitleL(TInt aIndex, const TDesC& aText) =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.CaptionedControl class.
 *
 * A class that implements MSwtCaptionedControl must also implement
 * MSwtComposite.
 */
class MSwtConstrainedText
{
public:
    enum TConstraint
    {
        ENumeric     = 1,
        EDecimal     = 2,
        EPhoneNumber = 4
    };

protected:
    virtual ~MSwtConstrainedText() {}; // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() =0;

    /**
     * Sets the constraints
     * Called right after construction, will be called only once.
     */
    virtual void SetConstraintL(TConstraint aConstraint) =0;

    /**
     * Gets the text.
     * @return  A newly allocated buffer containing the text. NULL
     *          may be returned if no text is present. The buffer is for the
     *          caller to destroy.
     */
    virtual HBufC* GetTextL() const =0;

    /**
     * Sets the contents.
     * The method must leave with KErrArgument if the content is invalid.
     * @param aText The new text.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool SetTextL(const TDesC& aText) =0;

    /**
     * Appends a string.
     * The new text is appended to the text at the end of the widget. The
     * excessive characters will be truncated if the length exceeds the maximum
     * number of characters.
     * The method must leave with KErrArgument if the content is invalid.
     * @param aText The text to be appended.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool AppendL(const TDesC& aText) =0;

    /**
     * Inserts a string after the caret position.
     * The old selection is replaced with the new text. The excessive characters
     * will be truncated if the length exceeds the maximum number of characters
     * that the receiver is capable of holding.
     * The method must leave with KErrArgument if the content is invalid.
     * @param aText The text to be inserted.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool InsertL(const TDesC& aText) =0;

    /**
     * Gets the number of characters.
     */
    virtual TInt GetCharCount() const =0;

    /**
     * Sets the maximum number of characters that the receiver is capable of
     * holding to be the argument.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool SetTextLimitL(TInt aLimit) =0;

    /**
     * Gets the position of the caret.
     */
    virtual TInt GetCaretPosition() const =0;

    /**
     * Sets the selection.
     * Indexing is zero based. The range of a selection is from 0..N where N is
     * the number of characters in the widget.
     * Text selections are specified in terms of caret positions. In a text
     * widget that contains N characters, there are N+1 caret positions, ranging
     * from 0..N. This differs from other functions that address character
     * position such as getText () that use the usual array indexing rules.
     * @param aStart The selection's starting point, if greater than the text's
     *               length, the method must leave with ESwtErrorInvalidRange.
     * @param aEnd   The selection's ending point, if greater than the text's
     *               length, the method must leave with ESwtErrorInvalidRange.
     */
    virtual void SetSelectionL(TInt aStart, TInt aEnd) =0;

    /**
     * Clears the selection.
     */
    virtual void ClearSelectionL() =0;

    /**
     * Selects all the text.
     */
    virtual void SelectAllL() =0;

    /**
     * Shows the selection.
     * If the selection is already showing in the receiver, this method simply
     * returns. Otherwise, lines are scrolled until the selection is visible.
     */
    virtual void ShowSelectionL() =0;

    /**
     * Cuts the selected text to the clipboard.
     * The method must leave with KErrArgument if the content is invalid.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool CutL() =0;

    /**
     * Copies the selected text to the clipboard
     */
    virtual void CopyL() =0;

    /**
     * Pastes text from clipboard.
     * The method must leave with KErrArgument if the content is invalid.
     * @return ETrue if a modify notification must be sent
     */
    virtual TBool PasteL() =0;

    /**
     * Gets the editable state.
     */
    virtual TBool GetEditable() const =0;

    /**
     * Sets the editable state.
     */
    virtual void SetEditable(TBool aEditable) =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.HyperLink class.
 */
class MSwtHyperLink
{
public:
    enum TFormat
    {
        EInvalid = 0,
        EUrl     = 1,
        EEmail   = 2,
        EPhone   = 4
    };

protected:
    virtual ~MSwtHyperLink() {}; // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() = 0;

    /**
     * Sets the receiver's text.
     */
    virtual void SetTextL(const TDesC& aText) = 0;

    /**
     * Gets the receiver's text.
     */
    virtual const TDesC* GetText() = 0;

    /**
     * Sets the receiver's text.
     */
    virtual void SetFormat(TInt aFormat) = 0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.Command class
 *
 */
class MSwtCommand
{
public:
    enum TCommandType
    {
        EGeneral      = 1,
        ESelect       = 2,
        EOk           = 3,
        ECancel       = 4,
        EDelete       = 5,
        EBack         = 6,
        EExit         = 7,
        EStop         = 8,
        EHelp         = 9,
        ECommandGroup = 10
    };

protected:
    virtual ~MSwtCommand() {} // Made protected to prevent destruction through the interface

public:
    virtual TSwtPeer Dispose() =0;
    virtual TSwtPeer JavaPeer() const =0;

    /**
     * Returns the parent control.
     * @return A reference to the parent control.
     */
    virtual const MSwtControl& ParentControl() const =0;

    /**
     * Returns the parent command.
     * @return A pointer to the parent command.
     */
    virtual const MSwtCommand* ParentCommand() const =0;

    /**
     * Sets the accelerator.
     * @param aAccelerator The accelerator value.
     */
    virtual void SetAcceleratorL(TInt aAccelerator) =0;

    /**
     * Enables or disables the command.
     * @param aEnabled ETrue to enable the command, EFalse to disable.
     */
    virtual void SetEnabled(TBool aEnabled) =0;

    /**
     * Sets or unset this command as default.
     * @param aDefault ETrue to set, EFalse to unset.
     */
    virtual void SetDefaultCommand(TBool aDefault) =0;

    /**
     * Sets the text of the command.
     * @param aText The text to set.
     */
    virtual void SetTextL(const TDesC& aText) =0;

    /**
     * Sets the image of the command.
     * @param aImage The image to set.
     */
    virtual void SetImageL(const MSwtImage* aImage) =0;

    /**
     * Sets the long label of the command.
     * @param aText The long label to set.
     */
    virtual void SetLongLabelL(const TDesC& aText) =0;

    /**
     * Gets the accelerator value.
     * @return This value.
     */
    virtual TInt Accelerator() const =0;

    /**
     * Return ETrue if the command is enabled, EFalse otherwise.
     * @return A boolean.
     */
    virtual TBool IsEnabled() const =0;

    /**
    * Return ETrue if the command is the default one, EFalse otherwise.
    * @return A boolean.
    */
    virtual TBool IsDefaultCommand() const =0;

    /**
     * Gets the text of the command.
     * @return This text.
     */
    virtual const TDesC& Text() const =0;

    /**
     * Gets the image of the command.
     * @return This image.
     */
    virtual const MSwtImage* Image() const =0;

    /**
     * Gets the long label of the command.
     * @return This long label.
     */
    virtual const TDesC& LongLabel() const =0;

    /**
     * Gets the priority of the command.
     * @return The priority.
     */
    virtual TInt Priority() const =0;

    /**
     * Gets the type of the command.
     * @return The type.
     */
    virtual TInt Type() const =0;
};


/**
 * Interface class for the command arranger
 */
class MSwtCommandArranger
{
public:
    enum TSwtMenuType
    {
        ESwtNoMenu=0,
        ESwtOptionsMenu,
        ESwtContextOptionsMenu
    };

public:
    virtual ~MSwtCommandArranger() {}
    virtual void ControlFocusLost() =0;
    virtual void ControlFocusGained(const MSwtControl& aControl) =0;
    virtual void ActiveShellLost() =0;
    virtual void ActiveShellGained(const MSwtShell& aShell) =0;
    virtual void CommandAddedL(const MSwtCommand& aCommand) =0;
    virtual void CommandRemoved(const MSwtCommand& aCommand) =0;
    virtual void CommandContentUpdated(const MSwtCommand& aCommand) =0;
    virtual void UpdateMSKLabelL() =0;
    virtual void CommandEnablingUpdated(const MSwtCommand& aCommand) =0;
    virtual void CommandSetAsDefault(const MSwtCommand& aCommand) =0;
    virtual TInt NumberOfCommandsInCba() const =0;
    virtual TBool ShowMenuL(TSwtMenuType aMenuType) =0;
    virtual TBool DoContextSensitiveOperationL() =0;
    virtual TBool DoClearOperationL() =0;
    virtual TBool DoHelpOperationL() = 0;
    virtual TBool IsContextSensitiveOperationSet() const =0;
    virtual TBool IsClearOperationSet() const =0;
    virtual TBool IsDefaultCommandSet() const=0;
    virtual void RefreshCba(TBool aFocusedControlOnly = EFalse) =0;
    virtual void UpdateRightSoftkeyCommandAndObserver(TInt aCommandId,
            const TDesC& aText, MEikCommandObserver& aCommandObserver) =0;
    virtual TBool HandleEnterKeyL() =0;
#ifdef RD_SCALABLE_UI_V2
    /**
     * Check if the control has a stylus popup menu i.e. if it has a popup menu items,
     * control menu or at least 1 OK/SELECT command.
     * @param aControl  The control
     * @return  ETrue, if the control has stylus popup memu
     */
    virtual TBool ControlHasStylusPopupMenu(const MSwtControl& aControl) const =0;

    /**
     * Gets the command menu for stylus popup menu.
     * @aControl   The control for which the stylus popup will be shown.
     * @return     A pointer to menu including the OK/SELECT commands that will be shown in stylus popup menu.
     */
    virtual const MSwtMenu* GetStylusPopupCommandsMenu(const MSwtControl& aControl) =0;
#endif
};


/**
 * Structure containing the data needed for the
 * org.eclipse.ercp.swt.mobile.ListBoxItem class.
 */
NONSHARABLE_CLASS(CSwtListBoxItem)
        : public CSwtItem
{
public:
    CSwtListBoxItem(HBufC* aString, const MSwtImage* aImage,
    HBufC* aHeadingString, const MSwtImage* aHeadingIcon)
            : CSwtItem(aString, aImage)
            , iHeadingString(aHeadingString)
            , iHeadingIcon(aHeadingIcon)
            , iHeadingIconSize(TSize::EUninitialized)
    {
        if (iHeadingIcon != NULL)
        {
            iHeadingIcon->AddRef();
        }
    }

    CSwtListBoxItem(HBufC* aString, const MSwtImage* aImage, const TSize& aImageSize,
                    HBufC* aHeadingString, const MSwtImage* aHeadingIcon, const TSize& aHeadingIconSize)
            : CSwtItem(aString, aImage, aImageSize)
            , iHeadingString(aHeadingString)
            , iHeadingIcon(aHeadingIcon)
            , iHeadingIconSize(aHeadingIconSize)
    {
        if (iHeadingIcon != NULL)
            iHeadingIcon->AddRef();
    }

    ~CSwtListBoxItem()
    {
        delete iHeadingString;
        if (iHeadingIcon != NULL)
            iHeadingIcon->RemoveRef();
    }

    inline const TDesC& HeadingString() const
    {
        return (iHeadingString!=NULL) ? static_cast<const TDesC&>(*iHeadingString) : KNullDesC;
    }

    inline void SetHeadingString(HBufC* aString)
    {
        delete iHeadingString;
        iHeadingString = aString;
    }

    inline const MSwtImage* HeadingIcon() const
    {
        return iHeadingIcon;
    }

    inline void SetHeadingIconSize(const TSize& aNewSize)
    {
        iHeadingIconSize = aNewSize;
    }

    inline TSize HeadingIconSize() const
    {
        return iHeadingIconSize;
    }

    inline void SetHeadingIcon(const MSwtImage* aImage)
    {
        if (iHeadingIcon != NULL)
            iHeadingIcon->RemoveRef();
        iHeadingIcon = aImage;
        if (iHeadingIcon != NULL)
            iHeadingIcon->AddRef();
    }

private:
    HBufC*           iHeadingString;  // Owned
    const MSwtImage* iHeadingIcon;    // Ref counted
    TSize            iHeadingIconSize;
};

typedef RPointerArray<CSwtListBoxItem> RSwtListBoxItemsArray;


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.ListBox class
 * A class that implements MSwtListBox must also implement MSwtScrollable.
 */
class MSwtListBox
{
public :
    enum ELayoutStyle
    {
        ELbStyleNoHeadingText = 0,
        ELbStyle1LineItem     = 1,
        ELbStyle2LineItem     = 2
    };

    enum ELayoutModifiers
    {
        KLbModShowSelectionNumber = 0x10,
        KLbModShowHeadingIcons    = 0x20,
        KLbModShowDetailIcons     = 0x40,
    };

protected:
    virtual ~MSwtListBox() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtScrollable interface
     */
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Scrollable()->Control();
    }

    /**
     * Sets the LayoutStyle.
     */
    virtual void SetLayoutStyleL(TInt aLayoutStyle) =0;

    /**
     * Deselects the items defined in the aIndices array from the <a>List</a>.
     * @param aIndices  The array of the items to deselect.
     * @param aCount   The number of items of this array.
     */
    virtual void DeselectItemsL(const TInt* aIndices, TInt aCount) =0;

    /**
     * Deselects the item indexed as aIndex from the <a>List</a>.
     * @param aIndex    The index of the item to deselect.
     */
    virtual void DeselectItemL(TInt aIndex) =0;

    /**
     * Deselects the items indexed from aStart to aEnd from the <a>List</a>.
     * @param aStart    The first element to deselect.
     * @param aEnd      The last element to deselect.
     */
    virtual void DeselectRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Deselects all the items from the <a>List</a>.
     */
    virtual void DeselectAllL() =0;

    /**
     * Gets the current item.
     */
    virtual TInt GetFocusIndex() const =0;

    /**
     * Returns the number of selected elements.
     */
    virtual TInt GetSelectionCount() const =0;

    /**
     * Get the selected items indices.
     */
    virtual const CArrayFix<TInt>* GetSelectionIndices() const =0;

    /**
     * Refresh a specific item.
     */
    virtual void RefreshItemL(TInt aIndex, const CSwtListBoxItem* aListBoxItem) =0;

    /**
     * Refresh the whole list.
     */
    virtual void RefreshListL(const RSwtListBoxItemsArray& aItemsArray) =0;

    /**
     * Selects a single item.
     *
     * @param aIndex    The item to select.
     */
    virtual void SelectItemL(TInt aIndex, TBool aScroll) =0;

    /**
     * Selects a range of items.
     * @param aStart    The first item to select.
     * @param aEnd      The last item to select.
     */
    virtual void SelectRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Selects all items.
     */
    virtual void SelectAllL() =0;

    /**
     * Sets the current focused item.
     *
     * @param aIndex    The item to focus on.
     */
    virtual void SetFocusIndex(TInt aIndex) =0;

    /**
     * Sets the receiver's items to be the given array of items.
     * @param aItemsArray The array to set.
     */
    virtual void SetDataModelL(const RSwtListBoxItemsArray& aItemsArray) =0;

    /**
     * Sets the font for the heading text.
     * @param aFont The font to use, if NULL a default font is to be used.
     */
    virtual void SetHeadingFontL(const MSwtFont* aFont) =0;

    /**
     * Shows the selection.
     */
    virtual void ShowSelection() =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.ListView class.
 * A class that implements MSwtListView must also implement MSwtScrollable
 */
class MSwtListView
{
protected:
    virtual ~MSwtListView() {} // Made protected to prevent destruction throuth the interface

public:
    /**
     * Returns the MSwtScrollable interface
     */
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Returns the MSwtControl interface
     */
    inline MSwtControl* Control()
    {
        return Scrollable()->Control();
    }

    /**
     * Appends a string element to the <a>ListView</a>.
     * @param aPtr      A reference to the descriptor to be appended.
     * @param aImage    A pointer to the image to be appended.
     */
    virtual void AppendL(const TDesC& aPtr, const MSwtImage* aImage) =0;

    /**
     * Inserts a string element into the <a>List</a> at the specified position.
     * @param aPos      The position ( relative to zero ) where the string element is to be inserted.
     * @param aPtr      A reference to the descriptor to be inserted.
     * @param aImage    A pointer to the image to be appended.
    */
    virtual void InsertL(TInt aPos, const TDesC& aPtr, const MSwtImage* aImage) =0;

    /**
     * Deselects the item indexed as aIndex from the <a>ListView</a>.
     * @param aIndex    The index of the item to deselect.
     */
    virtual void DeselectItem(TInt aIndex) =0;

    /**
     * Deselects the items indexed from aStart to aEnd from the <a>ListView</a>.
     * @param aStart    The first element to deselect.
     * @param aEnd      The last element to deselect.
     */
    virtual void DeselectRange(TInt aStart, TInt aEnd) =0;

    /**
     * Deselects the items defined in the aIndices array from the <a>ListView</a>.
     * @param aIndices  The array of the items to deselect.
     * @param aCount   The number of items of this array.
     */
    virtual void DeselectItems(const TInt* aIndices, TInt aCount) =0;

    /**
     * Deselects all the items from the <a>ListView</a>.
     */
    virtual void DeselectAll() =0;

    /**
     * Gets the current item.
     */
    virtual TInt GetFocusIndex() const =0;

    /**
     * Gets the text of the specified item.
     * @param aItemIndex The specified item.
     * @param aString    The descriptor that will hold the item's text upon return.
     *                   Must be empty upon call.
     */
    virtual void GetItemL(TInt aItemIndex, TPtrC& aString) const =0;

    /**
     * Gets the size of the icon according to the style.
     * @param aStyle The Layout style.
     * @return The icon size.
     */
    virtual TPoint GetIconSize(TInt aStyle) const =0;

    /**
     * Returns the number of elements of the <a>ListView</a>.
     */
    virtual TInt GetItemCount() const =0;

    /**
     * Returns the array of items from the <a>ListView</a>
     */
    virtual CPtrCArray* GetItemsL() const =0;

    /**
     * Get the selected items strings.
     */
    virtual CPtrCArray* GetSelectionL() const =0;

    /**
     * Returns the number of selected elements.
     */
    virtual TInt GetSelectionCount() const =0;

    /**
     * Get the selected items indices.
     */
    virtual const CArrayFix<TInt>* GetSelectionIndices() const =0;

    /**
     * Returns the index of the top element.
     */
    virtual TInt GetTopIndex() const =0;

    /**
     * Searches for a text item from a given position.
     * @param aString   The string to search for.
     * @param aStart    The position from which we start.
     */
    virtual TInt IndexOf(const TDesC& aString, TInt& aStart) const =0;

    /**
     * Returns if the specified item is selected.
     * @param aIndex The specified item.
     */
    virtual TBool IsSelected(TInt aIndex) const =0;

    /**
     * Removes the specified item from the <a>ListView</a>.
     * @param aIndex    The specified item.
     */
    virtual void RemoveItemL(TInt aIndex) =0;

    /**
     * Removes a range of items from the <a>ListView</a>.
     * @param aStart    The first element to remove.
     * @param aEnd      The last element to remove.
     */
    virtual void RemoveRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Removes the items defined in the aIndices array from the <a>ListView</a>.
     * @param aIndices  The array of the items to remove.
     * @param aCount    The number of items of this array.
     */
    virtual void RemoveItemsL(const TInt* aIndices, TInt aCount) =0;

    /**
     * Removes all items from the <a>ListView</a>.
     */
    virtual void RemoveAllL() =0;

    /**
     * Selects a single item.
     * @param aIndex    The item to select.
     */
    virtual void SelectItemL(TInt aIndex, TBool aScroll) =0;

    /**
     * Selects a range of items.
     * @param aStart    The first item to select.
     * @param aEnd      The last item to select.
     */
    virtual void SelectRangeL(TInt aStart, TInt aEnd) =0;

    /**
     * Selects all items.
     */
    virtual void SelectAllL() =0;

    /**
    * Sets the current focused item.
    *
    * @param aIndex    The item to focus on.
    */
    virtual void SetFocusIndex(TInt aIndex) =0;

    /**
     * Sets the receiver's items to be the given array of items.
     * @param aStringArray      The array to set.
     * @param aImagesArray      The icons array.
     */
    virtual void SetItemsL(MDesCArray* aStringArray,
                           RPointerArray<MSwtImage>& aImagesArray) =0;

    /**
     * Sets the Layout style.
     */
    virtual void SetLayoutL(TInt aLayout) =0;

    /**
     * Sets the Layout density.
     */
    virtual void SetLayoutDensityL(TInt aDensity) =0;

    /**
     * Shows the selection.
     */
    virtual void ShowSelection() =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.DateEditor class.
 * A class that implements MSwtDateEditor must also implement MSwtControl.
 */
class MSwtDateEditor
{
public:
    enum TStyle
    {
        ESwtDate     = 1,
        ESwtTime     = 2,
        ESwtDateTime = 3,
        ESwtDuration = 4,
        ESwtOffset   = 5,
        ESwtCompact  = 0x10,
        ESwtFull     = 0x20
    };

protected:
    virtual ~MSwtDateEditor() {} // Made protected to prevent destruction through the interface

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtControl* Control() =0;

    /**
     * Returns the number of milliseconds since January 1st, 1970,
     * stored by the MSwtDateEditor
     */
    virtual TInt64 GetDate() =0;

    /**
     * Returns the number of seconds displayed by the MSwtDateEditor
     * if its style is ESwtDuration or ESwtOffset, 0 otherwise.
     */
    virtual TInt GetOffset() const =0;

    /**
     * Sets the number of milliseconds since January 1st, 1970,
     * stored by the MSwtDateEditor
     */
    virtual void SetDate(const TInt64& aJavaDate) =0;

    /**
     * Sets the date editor style of the MSwtDateEditor object
     * The Date editor is inialised to the current Home Time.
     */
    virtual void SetDateEditorStyleL(TInt aDateEditorStyle) =0;

    /**
     * Sets duration/offset of the the MSwtDateEditor object
     */
    virtual void SetOffset(TInt aSeconds) =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.SortedList class.
 * A class that implements MSwtSortedList must also implement MSwtScrollable.
 */
class MSwtSortedList
{
public:
    enum TStyle { ESwtFilter = 1 };

protected:
    virtual ~MSwtSortedList() {} // Made protected to prevent destruction through the interface

public:

    /**
     * Appends a string element to the <a>SortedList</a>.
     * @param aPtr A reference to the descriptor to be appended.
     */
    virtual void AppendL(const TDesC& aPtr) =0;

    /**
     * Returns the string of the currently focused item.
     */
    virtual void GetFocusedItem(TPtrC& aItem) const =0;

    /**
     * Returns the height of an item.
     */
    virtual TInt GetItemHeight() const =0;

    /**
     * Returns the number of items currently selected.
     */
    virtual TInt GetSelectionCount() const =0;

    /**
     * Returns the selected items of the <a>SortedList</a>.
     */
    virtual CPtrCArray* GetSelectionL() const =0;

    /**
     * Remove all items from the <a>SortedList</a>.
     */
    virtual void RemoveAllL() =0;

    /**
     * Removes a string element from the <a>SortedList</a>.
     * @param  aPtr A reference to the descriptor to be appended.
     * @return EFalse if the string has been found and removed.
     */
    virtual TBool RemoveL(const HBufC& aPtr) =0;

    /**
     * Returns the MSwtScrollable interface
     */
    virtual MSwtScrollable* Scrollable() =0;

    /**
     * Selects an item of the <a>SortedList</a>.
     */
    virtual TBool SelectItemL(const TDesC* aString) =0;

    /**
     * Sets the receiver's items to be the given array of items.
     * @param aStringArray The array to set.
     */
    virtual void SetItemsL(MDesCArray* aStringArray) =0;

    /**
     * Set the modeStyle of the SortedList.
     */
    virtual void SetModeStyleL(TInt aModeStyle) =0;

    /**
     * Sets the selection of the <a>SortedList</a>.
     */
    virtual void SetSelectionL(MDesCArray* aStringArray) =0;

    /**
     * Scroll the MSwtSortedList to show the first item selected.
     */
    virtual void ShowSelection() =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.TaskTip class.
 */
class MSwtTaskTip
{
protected:
    virtual ~MSwtTaskTip() {} // Made protected to prevent destruction through the interface

public:
    virtual TSwtPeer Dispose() =0;
    virtual TRect GetBarDefaultBounds(TBool aHasText) const=0;
    virtual TRect GetLabelDefaultBounds(TBool aHasText) const=0;
    virtual TRect GetShellDefaultBounds(TBool aHasText) const=0;
    virtual TSwtPeer JavaPeer() const =0;
};


/**
 * Interface class for the org.eclipse.ercp.swt.mobile.TextExtension class.
 */
class MSwtTextExtension
{
public:
    enum TTypeStyle
    {
        EEmailAddress   = 1 << 1,
        EUrl            = 1 << 2,
        ENonPredictive  = 1 << 3,
        ELatinInputOnly = 1 << 4
    };

    enum TCasingModifier
    {
        EOriginalCase = 0,
        EUpperCase    = 1,
        ELowerCase    = 2,
        ETextCase     = 3,
        ETitleCase    = 4
    };

public:
    /**
     * Returns the MSwtControl interface
     */
    virtual MSwtText* Text() =0;

    /**
     * Sets the type style
     * This method will be called right after construction and only once.
     */
    virtual void SetTypeStyleL(TInt aTypeStyle) =0;

    /**
     * Hints to the implementation as to the input mode that should be
     * used when the user initiates editing of this control.
     * @param aCasingModifier A lettercase modifier attribute.
     * @param aMode A string naming a Unicode character block,
     *              input subset or NULL
     */
    virtual void SetInitialInputModeL(TCasingModifier aCasingModifier,
                                      const TDesC* aMode) =0;
};


#endif // ESWTMOBILEEXTENSIONS_H
