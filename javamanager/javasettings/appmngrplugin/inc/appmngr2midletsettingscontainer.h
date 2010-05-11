/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares container control for application.
*
*/


#ifndef APPMNGR2MIDLETSETTINGSCONTAINER_H
#define APPMNGR2MIDLETSETTINGSCONTAINER_H

// INCLUDES
#include "appmngr2midletsettingsview.h"         // CAppMngr2MidletSettingsView
#include "appmngr2midletsettingsutil.h"


// FORWARD DECLARATIONS
class CAppMngr2MidletSettingsView;

// CONSTANTS

// CLASS DECLARATION

/**
 * MIDlet suite settings container control class.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.2
 */
class CAppMngr2MidletSettingsContainer : public CCoeControl
{
public: // Constructors and destructor

    /**
    * C++ constructor.
    */
    CAppMngr2MidletSettingsContainer(CAppMngr2MidletSettingsView& aView);

    /**
    * EPOC default constructor.
    * @param aRect Frame rectangle for container.
    */
    void ConstructL(const TRect& aRect);

    /**
    * Destructor.
    */
    virtual ~CAppMngr2MidletSettingsContainer();

public: // New functions
    /**
        * Return pointer to the listbox
        */
    inline CEikTextListBox* ListBox() const
    {
        return iListbox;
    };

    /**
        * Initializes the contents of the list box
        */
    void InitListBoxContentL(const std::vector<ListItem>& aListItems);

    /**
    * Refreshes the contents of the listbox
    */
    void RefreshListBoxContentL(int aListBoxContentIndex, int aSelectedItemIndex);

    /**
    * Refreshes the contents of the listbox
    */
    void RefreshListBoxContentL(int aListBoxContentIndex, int aSelectedItemIndex, const ListItem& aListItem);

    /**
    * Refreshes the contents of the listbox
    */
    void RefreshListBoxContentL(int aListBoxContentIndex, int aSelectedItemIndex, bool aAllEnabled);

    /**
    * Refreshes the contents of the listbox
    */
    void RefreshListBoxContentL(const ListItem& aOldItem, const ListItem& aNewItem);

    /**
    * Getter for a single list box item
    */
    const ListItem& GetListBoxItem(int aListBoxItemIndex);

    /**
    * Setter for a single list box item
    */
    void SetListBoxItemValue(int aListBoxItemIndex, int aValue);

public: // Functions from base classes

    /**
    * From CCoeControl
    */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

private: // Functions from base classes

    /**
     * From CoeControl,SizeChanged.
     */
    void SizeChanged();

    /**
      * From CoeControl,FocusChanged.
      */
    void FocusChanged(TDrawNow aDrawNow);

    /**
     * From CoeControl,CountComponentControls.
     */
    TInt CountComponentControls() const;

    /**
     * From CCoeControl,ComponentControl.
     */
    CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl
     * Get the help context for this control.
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
    * From CCoeControl
    */
    void HandleResourceChange(TInt aType);

private: //data

    /**
     * MIDlet suite settings view
     * Not owned
     */
    CAppMngr2MidletSettingsView& iView;

    /**
     * List box
     * Owned
     */
    CAknSettingStyleListBox* iListbox;

    /**
     * List box content
     * Owned
     */
    std::vector<ListItem> iListBoxContent;
public:
};

#endif // APPMNGR2MIDLETSETTINGSCONTAINER_H

// End of File
