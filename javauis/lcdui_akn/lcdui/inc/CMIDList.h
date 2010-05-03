/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  List handling
*
*/



#ifndef CMIDLIST_H
#define CMIDLIST_H

//  INCLUDES
#include <e32base.h>
// CCoeControl inheritance in class declaration
#include <coecntrl.h>
// MEikListBoxObserver interface in class declaration
// and API related to list box
#include <eiklbo.h>
// MMIDCommandObserver interface in class declaration
#include <eikcmobs.h>
#include <AknPictographDrawerInterface.h>
#include <AknUtils.h>
#include "lcdui.h"
#include "CMIDCommand.h"


// MACROS

// DATA TYPES

// FORWARD DECLARATIONS
class CAknColumnListBox;
class CMIDPopupNoteController;
class CMIDDisplayable;
class CAknAppUi;
class CAknsFrameBackgroundControlContext;
class CAknPictographInterface;
class CEikLabel;
class CGulIcon;
class CEikTextListBox;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
class MTouchFeedback;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// CLASS DECLARATION

/**
*  List handling
*
*  @lib lcdui.dll
*  @since S60 3.1
*/
NONSHARABLE_CLASS(CMIDList) : public CCoeControl, public MEikListBoxObserver,
        public MMIDList, public MMIDCommandObserver,
        public MEikScrollBarObserver
{
public:

    enum TListBoxType
    {
        // Column ListBox types
        EUndefined = 0,
        ESingleStyle,
        ESingleGraphicStyle,
        ESingle2GraphicStyle,
        ESingleLargeStyle,

        // Formatted cell listboxes
        EDouble2Style,
        EDouble2GraphicStyle,
        EDouble2GraphicLargeStyle,
        EDouble2LargeStyle,
        ESingleGraphicLargeStyle
    };

public:
    /** A list item. Contains information such as text, icon, font, etc */
    NONSHARABLE_CLASS(CMIDListItem) : public CBase
    {
public:
        static CMIDListItem* NewLC(const TDesC& aText, TInt aIconIndex);
        ~CMIDListItem();

        HBufC* FormattedTextLC(const TListBoxType&, const MMIDChoiceGroup::TChoiceType&) const;
        TDesC16& Text();

        TInt IconIndex() const;
        MMIDFont* Font() const;
        TBool Selected() const ;

        void SetIconIndex(TInt aIconIndex);
        void SetTextL(const TDesC& aText);

        void SetFont(MMIDFont* aFont);
        void SetSelected(TBool aSelected);
        TBool GetPopupNeeded() const;
private:
        CMIDListItem(TInt aIconIndex);
        void ConstructL(const TDesC& aText);

        CMIDListItem(const CMIDListItem& aCopy); //copy c.tor not allowed
        CMIDListItem& operator=(const CMIDListItem& aCopy); //assignment not allowed
        void CMIDList::CMIDListItem::RemoveNewLineCharactersL(); // removes new line characters from copy of item text
        void RemoveTabulators(); // removes tabulators from item text

private:
        TBool iPopupNeeded;
        HBufC* iFormatedText;      // copy of item text for new line characters filtering
        HBufC* iText;          // the item text without lbox formatting
        TInt iIconIndex;       // the index into the icon array
        MMIDFont* iFont;       // either a custom font or NULL, owned java side
        TBool     iSelected; // true if this item is selected
    };

public:  // Public construction and destruction

    static CMIDList* NewL(MMIDEnv& aEnv,MMIDDisplayable& aDisplayable,TInt aType,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray);

    virtual ~CMIDList();

public: // Plugin internal public API

    CMIDDisplayable* Displayable();

public: // MMIDList interface

    void InsertElementL(TInt aIndex,const TDesC& aText, MMIDImage* aImage);
    void DeleteElementL(TInt aIndex);
    void DeleteAllL();
    void SetElementL(TInt aIndex,const TDesC& aText, MMIDImage* aImage);
    void SelectElementL(TInt aIndex,TBool aSelected);
    TBool IsSelected(TInt aIndex);
    void SetFontL(TInt aIndex, MMIDFont* aFont);
    void SetFitPolicyL(TInt aFitPolicy);
    void SetSelectCommand(TInt aCommandID);

public: // MMIDComponent interface

    CBase* Ptr();
    void Dispose();

public: // MEikScrollBarObserver
    /*virtual*/
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

private: // From CCoeControl

    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void MakeVisible(TBool aVisible);
    void SizeChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    void HandleResourceChange(TInt aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2

    // Responds to focus change.
    void FocusChanged(TDrawNow aDrawNow);

public: // From MEikListBoxObserver

    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    CEikTextListBox* ListBox();

public: // From MEikCommandOberver
    void ProcessCommandL(TInt aCommandId);

public:
    // From MMIDCommandObserver
    TBool ProcessCommandL(CMIDCommand* aCommand);

private: // Private Construction

    CMIDList(MMIDEnv& aEnv,TInt aType);
    void ConstructL(MMIDDisplayable& aDisplayable,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray);

    // Prohibit copy constructor if not deriving from CBase.
    CMIDList(const CMIDList&);
    // Prohibit assigment operator if not deriving from CBase.
    CMIDList& operator=(const CMIDList&);

#ifdef RD_JAVA_S60_RELEASE_9_2
public:
    /**
     * Posts pending up event in order to remove highlight after pop-up menu has been dismissed.
     * (List element highlight is shown during pop-menu occurance).
     */
    void PostPendingUpEventL();

    /**
     * Returns Java MIDP List ChoiceType.
     */
    MMIDChoiceGroup::TChoiceType ListChoiceType();
    TBool IsHighlighted();
#endif // RD_JAVA_S60_RELEASE_9_2

private: // Implementation
    /**
     * Updates the labels and the availablity of commands in displayable.
     * Most notably, the command in the MSK needs updating according to
     * selection status of the list. This method needs to be called within this
     * class whenever the list selection or highlight position has changed.
     */
    void UpdateCommands();

    void SetMaxIconSize();

    TBool CreateListBoxL();
    TListBoxType ListBoxType();

    void CreateIconArrayL();
    CGulIcon* CreateCheckOrRadioIconL(TBool aState, TInt aStyle) const;
    void CreateSelectionIconsL();

    CGulIcon* CreateIconLC(MMIDImage* aImage);
    TInt InsertNewIconL(MMIDImage* aImage);

    void CreateItemsL(RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray);
    void PopulateListBoxL();

    void SetSelectionIntoListBoxL(TInt aIndex,TBool aSelected);
    void SetSelectionIconL(CEikTextListBox* aListBox, TInt aIndex, TBool aSelected);

    void DoInsertElementL(TInt aIndex,const TDesC& aText, MMIDImage* aImage);
    void DoDeleteElement(TInt aIndex);

    void UpdateFontToElementL(TInt aIndex);

    void RequestVisualUpdateL();
    static TInt VisualUpdateTimerCallback(TAny* aThis);

    void UpdateOrRecreateListBoxL();
    void UpdateListBoxL();

    void ShowInfoPopupL();
    TBool CheckIfPopupIsNeeded(); // check if popup should be displayed


private:    // Data

    MMIDEnv& iEnv;  // The MIDP environment

    CEikTextListBox* iListBox;  // The list box

    TListBoxType iListBoxType; // The type of list box determines the avkon list box to be created

    TSize iMaxIconSize; // The maximum icon size determines the avkon list box to be created

    CArrayPtr<CGulIcon>* iIconArray; //The icon array.
    TBool iIconArrayOwned;           // Defines if the icon array is owned by this class

    RPointerArray<CMIDListItem> iItems; // The list box items

    MMIDChoiceGroup::TChoiceType iChoiceType; // Multiple, Exclusive, Implicit, Popup

    MMIDChoiceGroup::TFitPolicy iFitPolicy; // Will the text be wrapped or not


    CMIDPopupNoteController* iPopupController;

    CMIDDisplayable* iDisplayable; // The displayable that owns us

    // msk: this may be obsolete
    TInt iSelectCommandId;  // The command to invoke on select

    CMIDCommand* iSelectCommand;        // Application provide command to invoke on select
    CMIDCommand* iBuiltInSelectCommand; // Built-in command to select items on exclusive/implicit list
    CMIDCommand* iMarkCommand;          // Built-in command to mark items on multiple list
    CMIDCommand* iUnmarkCommand;        // Built-in command to unmark items on multiple list

    TBool iNeedToRecreateListBox; //True when we need to recreate the list box

    TTime iUpdateRequestTime; //The time we requested an update to the list box

    TInt iHighlightPosition; //The LB highlight pos

    CPeriodic* iVisualUpdateTimer; //The timer for visually updating the list box

    TInt iBackCommandPosition; //The position of the Back soft key in the cba when info popup is shown
#ifdef RD_SCALABLE_UI_V2
    TBool iPointerDragHasStarted; //True if drag has started
    TBool iPanning;
    TBool iLongTapDetected;

#endif //RD_SCALABLE_UI_V2

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    MTouchFeedback *iFeedback;
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

    TBool iInfoPopupNoteIsShown;

#ifdef RD_JAVA_S60_RELEASE_9_2
    TBool iHighlight; //The LB highlight
    TPointerEvent iPendingUpEvent; // Stores pointer up event to be sent later.
    TBool         iUpEventPending;
#endif // RD_JAVA_S60_RELEASE_9_2    
};

#endif // CMIDLIST_H

// End of File
