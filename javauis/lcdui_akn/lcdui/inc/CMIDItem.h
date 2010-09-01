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


#ifndef CMIDITEM_H
#define CMIDITEM_H

// CEikBorderedControl inheritance in class declaration
#include <eikbctrl.h>
#include "lcdui.h"
// API for using commands that item owns
#include "CMIDCommand.h"

class CMIDCommandList;
class CMIDForm;
class CMIDMenuHandler;
class CMIDUIManager;

// Base class for all Items

NONSHARABLE_CLASS(CMIDItem) : public CEikBorderedControl, public MMIDCommandObserver
{
public:
    virtual ~CMIDItem();
    //
    // Returns basic properties shared by all items
    //
    virtual TPtrC Label() const;
    MMIDItem::TLayout Layout() const;
    CMIDCommandList* CommandList() const;
    virtual TBool IsSelectable() const = 0;
    /**
     * Returns the appropriate command for the MSK at the moment. It can be a
     * built-in command if set or a default command, if set.
     **/
    CMIDCommand* GetMSKCommand() const;
    CMIDCommand* DefaultCommand() const;
    virtual void SetForm(CMIDForm* aForm);
    CMIDForm* Form() const;
    virtual void ItemAddedToFormL();
    virtual void ItemRemovedFromForm();
    virtual void HandleCurrentL(TBool aCurrent);

    /**
     * Used to notify item that form scrolling has been completed and
     * new position has been set for the item.
     *
     * @since S60 3.2
     */
    virtual void NotifyScrollingCompleted();

    //
    // Get functions used by form
    //
    TSize PreferredSize();
    MMIDItem::TLayout Layout();
    virtual TInt ItemPreferredHeightWithoutLabel() = 0;
    //
    // Set functions used by form
    //
    void SetPreferredWidth(TInt aWidth);
    void SetPreferredHeight(TInt aHeight);
    void SetLayoutL(MMIDItem::TLayout aLayout);
    //
    virtual TInt Type();

    /**
     * Command handling function from MMIDCommandObserver. Default implementation
     * in CMIDItem is empty, but subclasses can override the method if they
     * provide commands that need to be processed internally rather than sent
     * to Java side (e.g. build-in MSK commands).
     **/
    TBool ProcessCommandL(CMIDCommand* aCommand);

    /**
     * Get UI manager
     * @return CMIDUIManager instance
     *
     * @since s60
     */
    virtual CMIDUIManager* GetUIManager() const;

protected:
    CMIDItem(MMIDItem::TLayout aLayout, CMIDUIManager* aUIManager);
    void ConstructL();
    //
    // Sets some basic item properties from subclasses
    //
    virtual void SetLabelL(const TDesC& aLabel);
    /**
     * Adds command for item
     * @param aCommand - command to add
     * @param aFormNotification - form notification flag
     *          ETrue (default) - form is notified about command change
     *          EFalse - avoid form notification
     */
    void AddCommandL(MMIDCommand* aCommand, TBool aFormNotification = ETrue);
    /**
     * Remove command from item
     * @param aCommand - command to remove
     * @param aFormNotification - form notification flag
     *          ETrue (default) - form is notified about command change
     *          EFalse - avoid form notification
     */
    void RemoveCommand(MMIDCommand* aCommand, TBool aFormNotification = ETrue);
    void SetDefaultCommand(MMIDCommand* aCommand);
    /**
     * Sets a new command to be displayed in the MSK. If the command is
     * not NULL, it will override the default command set by the application
     * programmer.
     * Built-in MSK command is used e.g. in lists for selecting and in date
     * field items to toggle the am/pm field.
     **/
    void SetBuiltInMSKCommand(CMIDCommand* aCommand);

    void NotifyFormAboutCommandChange();

    void SetPreferredSize(const TSize& aSize,const TSize& aMinimumSize);
    virtual TSize CheckRequestedSize(const TSize& aRequestedSize) const;

protected:
    //
    // Layout and label are stored here even though, especially for label, it may
    // be stored in the subclass instead/as well
    //
    MMIDItem::TLayout iLayout;
    HBufC* iLabel;
    //
    CMIDForm* iForm;
    //
    CMIDCommandList* iCommandList;
    //
    // The default command
    //
    CMIDCommand* iDefaultCommand;
    //
    // The built-in MSK command that will override default command if not null.
    CMIDCommand* iBuiltInMSKCommand;
    //
    // This can be used to store the actual preferred size that the item
    // reports
    //
    TSize iPreferredSize;
    //
    // This is set to the size requested by the MIDlet and may not be the
    // actual preferred size reported by the item. It needs to be stored since
    // any recalculation of size needs to take this into account
    //
    TSize iRequestedPreferredSize;
    //
    // Set when this Item is added to a Form.
    // Used to restrict the preferred size.
    //
    TInt iFormWidth;
    // UI manager instance
    CMIDUIManager* iUIManager;
public:
    //
    // For layout algorithm. Calculated by Form
    //
    MMIDItem::TLayout iEffectiveLayout;
    //
    // This defines whether there's a new line BEFORE the item is added
    // to the form. Calculated by Form
    //
    TBool iNewLine;
    //
    // we can use this to get the type
    //
    MMIDItem* iMMidItem;

    CMIDMenuHandler* iMenuHandler; // we don't own this
};

#endif // CMIDITEM_H



