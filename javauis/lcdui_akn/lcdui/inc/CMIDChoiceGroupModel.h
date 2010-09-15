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
* Description:  The CMIDChoiceGroup listbox model
*
*/

#ifndef CMIDCHOICEGROUPMODEL_H
#define CMIDCHOICEGROUPMODEL_H

#include <lcdui.h>

#include <coecntrl.h>

#include <eikenv.h>
// CAknFilteredTextListBoxModel inheritance in declaration
#include <AknUtils.h>

// using CMIDChoiceGroupElement for array of elements
#include "CMIDChoiceGroupElement.h"

// 3 seems like a reasonable default amount of elements in a choice
const TInt KCGElementArrayGranularity = 3;

// Forward declaration for the observer interface
class CMIDChoiceGroupModel;

// Choicegroup listbox model observer interface. The observer
// is notified about element addition, removal and modification
class MMIDChoiceGroupModelObserver
{
public:
    enum TChoiceGroupModelEvent
    {
        EElementAdded,
        EElementDeleted,
        EElementModified,
        EElementSelected,
        EUpdateStarted,
        EUpdateEnded
    };
public:
    virtual void HandleChoiceGroupModelEventL(
        CMIDChoiceGroupModel* aModel,
        TChoiceGroupModelEvent aEvent) = 0;
#ifdef RD_JAVA_S60_RELEASE_9_2
    virtual TBool IsControlOnFormHighlighted() = 0;
#endif // RD_JAVA_S60_RELEASE_9_2
};


/**
 * ChoiceGroup listbox model. Contains ChoiceGroup elements.
 * Figures out and provides strings with correct indices to the
 * listbox, and creates an image array for the column listbox data
 */
NONSHARABLE_CLASS(CMIDChoiceGroupModel) : public CAknFilteredTextListBoxModel
{
public:
    // Default constructor & destructor
    CMIDChoiceGroupModel(MMIDChoiceGroup::TChoiceType aType);
    virtual ~CMIDChoiceGroupModel();

public:
    // Two-phase constructor, initialises member arrays and stuff
    virtual void ConstructL(CEikonEnv* aEikonEnv);

public: // Base class overrides
    // The model contains more than just the strings - need to override
    // number of items query
    virtual TInt NumberOfItems() const;

    // Provides a string with correct indices to images
    virtual TPtrC ItemText(TInt aItemIndex) const;

    // Initialises the element array with given strings
    void SetItemTextArray(MDesCArray* aItemTextArray);

    // Returns a pointer to the item text array (needs to create one
    // first, really. Actually this is not needed, and should be
    // overridden, if only to return NULL.
    MDesCArray* ItemTextArray() const;

public:  // New functions
    // Add an element to the end of the array
    void AppendElementL(CMIDChoiceGroupElement* aElement);

    // Insert an element at <aIndex>. Space must be reserved.
    void InsertElementL(TInt aIndex, CMIDChoiceGroupElement* aElement);

    // Delete element at <anIndex>.
    void DeleteElementL(TInt aIndex);

    // Delete all elements
    void DeleteAllL();

    // Set properties of element at <aIndex>
    void SetElementL(
        TInt aIndex,
        const TDesC& aText,
        CFbsBitmap* aBitmap,
        CFbsBitmap* aMask);

    // Get element at position <aIndex>
    CMIDChoiceGroupElement* ElementAt(TInt aIndex) const;

    // Reserve space for <aCount> elements
    void SetReserveL(TInt aCount);

    // Get icon array, recreate if requested
    CArrayPtr<CGulIcon>* IconArray(TBool aReCreate = EFalse);

    // Set model observer associated with this model
    void SetObserver(MMIDChoiceGroupModelObserver* aObserver)
    {
        iObserver = aObserver;
    }

    // Get control associated with this model
    MMIDChoiceGroupModelObserver* Observer()
    {
        return iObserver;
    }

    // Set selection state of item at <index>.
    // If an item is set selected in an exclusive type choice, also
    // deselects other items.
    // Deselection of an item in an exlusive choice has no effect.
    // NOTE: Currently does not leave, but the name was 'L' in Choice interface
    void SelectElementL(TInt aIndex, TBool aSelected);

    // Begin model update. During the update the icon array will not be
    // updated nor the observer notified of element actions.
    // Once EndUpdate() is called, icon array is rebuilt and notification
    // will occur.
    void BeginUpdate();

    // End a model update. At the end the icon array will be
    // recreated, and the associated observer notified.
    void EndUpdate();

    // Returns the index of the selected element in an exclusive
    // choice, or -1
    TInt SelectedElement();

    //Constructs radio or checkbox on and off icons
    void ReConstructSelectionIconsL();

    // Create an icon array to be given to the lbox column data
    // This should be done every time the contents of the model are changed,
    // or icon array is retrieved with create flag set.
    // NOTE that the indices in the array and the item strings should
    // be synchronised. The selection icon is always at index
    // 0 (selected) and 1 (not selected)
#ifdef RD_JAVA_S60_RELEASE_9_2
    void UpdateIconArrayL(TBool aHighlighted);
#else
    void UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

    // Returns true if there is at least one item that has icon
    TBool HasIcons() const;

private:  // Utility functions

    // Clears all selections
    void ClearSelection();

    // Report choicegroup event to observer
    void ReportEventL(
        MMIDChoiceGroupModelObserver::TChoiceGroupModelEvent aEvent);

private:
    // Type of the choicegroup
    // (model is only really interested in selection type)
    MMIDChoiceGroup::TChoiceType iType;

    // Icon to display the selection state. Depending on the
    // choice type this is either a radiobutton or a checkbox
    CGulIcon* iIconSelected;
    CGulIcon* iIconNotSelected;

#ifdef RD_JAVA_S60_RELEASE_9_2
    // The same icons as above, but used when whole ChoiceGroup
    // gets highlighted on Form
    CGulIcon* iIconSelectedHighlighted;
    CGulIcon* iIconNotSelectedHighlighted;
#endif // RD_JAVA_S60_RELEASE_9_2

    // Dummy icon for elements that have no icon
    CGulIcon* iIconDummy;

    // Element array
    CArrayPtrFlat<CMIDChoiceGroupElement>* iElements;

    // Indicates that the model is being "bulk-updated", ie
    // BeginUpdate() has been called. Icon array and control updates
    // are supressed during an update operation, until EndUpdate().
    TBool iUpdating;

    // Icon array, recreated when elements are added or
    // removed. Owned by the model, not listbox column data.
    CArrayPtrFlat<CGulIcon>* iIconArray;

    // Observer associated with this model. Observer is
    // reported about changes in the model, if set
    MMIDChoiceGroupModelObserver* iObserver;
};


#endif // CMIDCHOICEGROUPMODEL_H
