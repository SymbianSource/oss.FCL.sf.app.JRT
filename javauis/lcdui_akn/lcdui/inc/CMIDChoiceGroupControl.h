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
* Description:  The CMIDChoiceGroup control container. Owns and manipulates
*                the choicegroup model and creates / deletes the actual
*                control as needed.
*
*/

#ifndef CMIDCHOICEGROUPCONTROL_H
#define CMIDCHOICEGROUPCONTROL_H

#include <eiklbo.h>
#include <lcdui.h>

#include <AknUtils.h>
// CAknPopupList inheritance in CMIDChoiceGroupPopupList declaration
#include <aknPopup.h>

#include "CMIDChoiceGroupListBox.h"
#include "CMIDChoiceGroupModel.h"

#ifdef RD_SCALABLE_UI_V2
#include "CMIDForm.h"
#endif


#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif


class CMIDChoiceGroupItem;

// Forward declaration for the observer interface
class CMIDChoiceGroupControl;

// Choicegroup control observer interface. The observer
// is notified when the control size changes, so that it can adjust
// its size accordingly
class MMIDChoiceGroupControlObserver
{
public:
    enum TChoiceGroupControlEvent
    {
        ESizeChanged,
        EScrollRequest
    };
public:
    virtual void HandleChoiceGroupControlEventL(
        CMIDChoiceGroupControl* aControl,
        TChoiceGroupControlEvent aEvent,
        TAny* aParam) = 0;
};

// Popup list that shows the contents of the popup choice group.
// This derived class is needed in order that opened popup can be
// properly resized if its content changes.
NONSHARABLE_CLASS(CMIDChoiceGroupPopupList) : public CAknPopupList
{
public:
    static CMIDChoiceGroupPopupList* NewL(CEikListBox* aListBox, TInt aCbaResource,
    AknPopupLayouts::TAknPopupLayouts aType);
    virtual ~CMIDChoiceGroupPopupList();

    // Resizes the popup list
    void ResizeList();

#ifdef RD_SCALABLE_UI_V2
public: // from CCoeControl
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2

private:
    CMIDChoiceGroupPopupList();
};

// Choice group control. Encapsulates the actual control and popup
NONSHARABLE_CLASS(CMIDChoiceGroupControl) :
        public CCoeControl,
        public MEikListBoxObserver,
        public MMIDChoiceGroupModelObserver
{
public:
    // Constants identifying columns of column listobox data used
    // when choice group is created and layouted
    enum TColumnDataIndexes
    {
        ERadioButtonOrCheckBoxColumn,
        EUserDefinedImagesColumn,
        ETextColumn
    };

    CMIDChoiceGroupControl(
        MMIDChoiceGroup::TChoiceType aType,
        CMIDChoiceGroupItem* aItem);

    virtual ~CMIDChoiceGroupControl();

    // Constructs the ChoiceGroup control and the choicegroup model,
    // but does not yet create the actual listbox. The listbox is created
    // by a call to CreateActualControlL(), when the parent window
    // is known (ie when the CMIDChoiceGroupItem is added to a CMIDForm
    void ConstructL(
        RArray<TPtrC>& aStringArray,
        RArray<MMIDImage*>& aImageArray);

    // Creates the listbox control according to choice type and
    // fills it with elements, if the model already contains any
    void CreateControlL(const CCoeControl* aParent, const TRect& aRect);

    // Deletes the listbox control (when removed from a form). Does
    // not empty the model, however
    void DeleteControl();

    // Set coitrol observer associated with this control
    void SetObserver(MMIDChoiceGroupControlObserver* aObserver)
    {
        iObserver = aObserver;
    }

    // Get observer associated with this control
    MMIDChoiceGroupControlObserver* Observer()
    {
        return iObserver;
    }

    void HiliteFirstElement(TBool aEnableHighlight);
    void HiliteLastElement(TBool aEnableHighlight);
    void EnableListHighlight(TBool aEnable);
    TBool IsCurrentItemSelected();
    void ToggleCurrentItemSelectionL();

    // Nofication method to inform that the curent item has changed in the list
    void CurrentItemChanged();

    // Pops up the popup choice
    void DoPopupL();

    /**
     * Returns the number of elements in the choice group
     **/
    TInt NumberOfElements() const;

public: // delegated from MMIDChoiceGroup
    void InsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage);
    void DeleteElementL(TInt aIndex);
    void DeleteAllL();
    void SetElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage);
    void SelectElementL(TInt aIndex, TBool aSelected);
    TBool IsSelected(TInt aIndex);
    void SetFontL(TInt aIndex, MMIDFont* aFont);
    void SetFitPolicyL(TInt aFitPolicy);

public: // from MMIDChoiceGroupModelObserver
    virtual void HandleChoiceGroupModelEventL(
        CMIDChoiceGroupModel* aModel,
        TChoiceGroupModelEvent aEvent);

public: // from MEikListBoxObserver
    virtual void HandleListBoxEventL(CEikListBox* aControl, TListBoxEvent aEventType);

public: // from CCoeControl
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif //RD_SCALABLE_UI_V2

//    void SetContainerWindowL(const CCoeControl& aContainer);
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;

    void SizeChanged();
    void PositionChanged();
    // Calculates and returns the minimum size required by the control
    TSize MinimumSize() const;

    // Draws the control (only used in popup mode)
    virtual void Draw(const TRect& aRect) const;

    // Responds to focus change.
    void FocusChanged(TDrawNow aDrawNow);

    // Handles a change to control's resources.
    void HandleResourceChange(TInt aType);

    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

public:
    inline MMIDChoiceGroup::TChoiceType ChoiceType() const;
    void ClosePopup();

    // Sets up the columndata columns
    void SetupColumnsL(CColumnListBoxData* aColumnData);

    /**
     * Shows info popup for the current item if text does not fit and was ellipsed
     */
    void ShowInfoPopup();

    /**
     * Scrolls the form in order to get the focused sub-item on screen
     */
    void RestoreFocus();

#ifdef RD_SCALABLE_UI_V2
    /**
     * In timed scrolling calculates the needed scroll amount and
     * requests (via CMIDChoiceGroupItem) the form to scroll.
     *
     * @param aDirection    The direction of the scroll
     */
    void TimedScroll(CMIDForm::TDirection aDirection);
#endif

#ifdef RD_JAVA_S60_RELEASE_9_2
    CMIDForm* Form() const;
#endif // RD_JAVA_S60_RELEASE_9_2    

#ifdef RD_TACTILE_FEEDBACK
private:
    MTouchFeedback* iFeedback;
#endif



protected:

private:
    void DrawIcon(CGulIcon* aIcon) const;
    void DrawText(const TDesC& aText) const;

    // Fill listbox model with items given at construct time
    void PopulateModelL(RArray<TPtrC>& aStringArray,RArray<MMIDImage*>& aImageArray);

    // Creates a new element with text and image
    CMIDChoiceGroupElement* NewElementLC(const TDesC& aString, MMIDImage* aImage);

    // Report control event to observer
    void ReportEventL(MMIDChoiceGroupControlObserver::TChoiceGroupControlEvent aEvent,
                      TAny* aParam = NULL);

    // Calculates, whether the current item in the choice is visible,
    // and requests a form scroll, if necessary
    void RequestScrollIfNeededL();

    // Functions for creating the choice listbox. Create() uses Allocate()
    // and Construct(). Split to enable using LB pointer as parameter to
    // AknPopup, and AknPopup in turn as parent to LB construction
    CMIDChoiceGroupListBox* CreateListBoxL(const CCoeControl* aParent = NULL);
    CMIDChoiceGroupListBox* AllocateListBoxL();
    void ConstructListBoxL(
        CMIDChoiceGroupListBox* aListBox,
        const CCoeControl* aParent = NULL);

    // Deletes the choice listbox (taking care not to destroy the icon array)
    void DeleteListBox(CMIDChoiceGroupListBox* aListBox);

    // Returns whether this is a popup
    inline TBool IsPopup() const;

    TRect ListBoxRect(const TRect& aRect) const;

    /**
     * Sets selection state of element at <aIndex> and posts a java event
     * if needed
     *
     * @param   aIndex      The index of the element
     * @param   aSelected   The new selection state of the element
     * @param   aPostEvent  If true, itemStateChanged event is sent to the midlet
     */
    void SelectElementL(TInt aIndex, TBool aSelected, TBool aPostEvent);

    void UpdateMargins();
    void UpdatePopupListAppearanceL(TChoiceGroupModelEvent aEvent);

#ifdef RD_SCALABLE_UI_V2
    /**
     * Finds the index of the first choicegroup element of which more than half is outside the
     * visible form area. aScrollDirection indicates if the element is searched on the top or
     * bottom of the form. The choicegroup listbox must be partially on visible form area in order
     * this function to return a valid index. If the listbox is fully invisible, KErrNotFound is returned.
     *
     * @param aScrollDireection     The direction to which the form is being scrolled
     * @return  Index of the element if element is found in wanted direction, KErrNotFound otherwise
     */
    TInt FirstInvisibleElement(CMIDForm::TDirection aScrollDirection) const;
#endif

private:
    // Type of the choicegroup (list, popup)
    MMIDChoiceGroup::TChoiceType iType;

    // The parent item
    CMIDChoiceGroupItem* iItem;

    // Observer (usually the form item)
    MMIDChoiceGroupControlObserver* iObserver;

    // Element container model (owned outside of the listbox, because
    // elements may be added before the control is added to a form, ie
    // before the listbox has a parent window and can be created)
    CMIDChoiceGroupModel* iModel;

    // The actual listbox control
    CMIDChoiceGroupListBox* iListBox;

    /** The rect for placing the icon of the popup choice group, see SizeChanged() */
    TRect iPopupIconRect;

    /** The layout object for drawing the popup choice group text, see SizeChanged() */
    TAknLayoutText iPopupTextLayout;

    // Margins for choicegroup content. This includes also the margin between the label and listbox.
    TMargins8 iContentMargins;

    // Pointer to the popup list. This is needed in order that popup can be properly updated
    // if the list box model is modified while the popup is open. No need to delete
    // iPopupList in destructor because popup list is created and deleted in DoPopupL().
    // iPopupList is valid only while popup list is open, otherwise it is NULL.
    CMIDChoiceGroupPopupList* iPopupList;
#ifdef RD_SCALABLE_UI_V2
    TBool iPointerDragHasStarted; //True if drag has started

    // True if focus has changed between TPointerEvent::EButton1Down and
    // TPointerEvent::EButton1Up events.
    TBool iFocusChangingWithPen;

#endif //RD_SCALABLE_UI_V2
};

// --- Inlines

// Return choice group type
inline MMIDChoiceGroup::TChoiceType CMIDChoiceGroupControl::ChoiceType() const
{
    return iType;
}

// Returns whether this is a popup
inline TBool CMIDChoiceGroupControl::IsPopup() const
{
    return (iType == MMIDChoiceGroup::EPopup);
}

#endif // CMIDCHOICEGROUPCONTROL_H
