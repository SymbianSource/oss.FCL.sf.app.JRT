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


#include "CMIDChoiceGroupModel.h"
#include "CMIDDisplayable.h"

#include <e32def.h>

#include <aknconsts.h>
// using TMifAvkon enumeration
#include <avkon.mbg>
// LAF
// AknLayoutScalable_Avkon::list_single_2graphic_pane_g2_cp4()
#include <aknlayoutscalable_avkon.cdl.h>

#include <j2me/jdebug.h>

// Index where element icons start in the icon array. They're
// preceded by selection state icons (currently 2, selected and nonselected)
const TInt KElementIconStartIndex = 2;

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDChoiceGroupModel.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);


// C'tor
CMIDChoiceGroupModel::CMIDChoiceGroupModel(
    MMIDChoiceGroup::TChoiceType aType)
        : CAknFilteredTextListBoxModel(),
        iType(aType),
        iUpdating(EFalse)
{
}


// D'tor
CMIDChoiceGroupModel::~CMIDChoiceGroupModel()
{
    // Delete element array
    if (iElements)
    {
        //all elemnts in iElements array should be destroyed
        //to prevent memory leak
        iElements->ResetAndDestroy();
        delete iElements;
    }

    // Delete icon array, do not delete icons
    if (iIconArray)
    {
        iIconArray->Reset();
        delete iIconArray;
    }

    // Delete icons
    delete iIconSelected;
    delete iIconNotSelected;
#ifdef RD_JAVA_S60_RELEASE_9_2
    delete iIconSelectedHighlighted;
    delete iIconNotSelectedHighlighted;
#endif // RD_JAVA_S60_RELEASE_9_2
    delete iIconDummy;
}


// Two-phase constructor, initialises member arrays and stuff
void CMIDChoiceGroupModel::ConstructL(CEikonEnv* aEikonEnv)
{
    ASSERT(aEikonEnv);

    // Create element array
    iElements = new(ELeave) CArrayPtrFlat<CMIDChoiceGroupElement>(
        KCGElementArrayGranularity);

    // Create icon array
    iIconArray = new(ELeave) CArrayPtrFlat<CGulIcon>(
        KCGElementArrayGranularity);

    ReConstructSelectionIconsL();

    // Create the dummy icon (zero-sized bitmap)
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    bitmap->Create(TSize(0,0), aEikonEnv->DefaultDisplayMode());

    CleanupStack::PushL(bitmap);
    iIconDummy = CGulIcon::NewL(bitmap);
    CleanupStack::Pop(); // bitmap
}

/** Create selection icons according to type */
void CMIDChoiceGroupModel::ReConstructSelectionIconsL()
{
    TFileName avkonbmpFilename = AknIconUtils::AvkonIconFileName();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    const TAknsItemID& selectedSkinId = (iType == MMIDChoiceGroup::EMultiple) ? KAknsIIDQgnIndiCheckboxOn :
                                        KAknsIIDQgnIndiRadiobuttOn;

    const TAknsItemID& notSelectedSkinId = (iType == MMIDChoiceGroup::EMultiple) ? KAknsIIDQgnIndiCheckboxOff :
                                           KAknsIIDQgnIndiRadiobuttOff;

    TInt selectedIconId = (iType == MMIDChoiceGroup::EMultiple) ? EMbmAvkonQgn_indi_checkbox_on : EMbmAvkonQgn_indi_radiobutt_on;
    TInt selectedMaskId = (iType == MMIDChoiceGroup::EMultiple) ? EMbmAvkonQgn_indi_checkbox_on_mask : EMbmAvkonQgn_indi_radiobutt_on_mask;
    TInt notSelectedIconId = (iType == MMIDChoiceGroup::EMultiple) ? EMbmAvkonQgn_indi_checkbox_off : EMbmAvkonQgn_indi_radiobutt_off;
    TInt notSelectedMaskId = (iType == MMIDChoiceGroup::EMultiple) ? EMbmAvkonQgn_indi_checkbox_off_mask : EMbmAvkonQgn_indi_radiobutt_off_mask;

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;

    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);

    TAknLayoutRect layoutRect;
    // layout in respect of the whole screen as only the icon size is needed
    layoutRect.LayoutRect(screenRect, AknLayoutScalable_Avkon::list_single_2graphic_pane_g2_cp4().LayoutLine());

    TSize iconSize(layoutRect.Rect().Width(), layoutRect.Rect().Height());

    // create icons for non-highlighted mode

    AknsUtils::CreateColorIconLC(skin, notSelectedSkinId, KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG14, bitmap, mask, avkonbmpFilename, notSelectedIconId,
                                 notSelectedMaskId, KRgbBlue, iconSize, EAspectRatioPreservedAndUnusedSpaceRemoved);

    CGulIcon* iconNotSelected = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); //bitmap, mask

    delete iIconNotSelected;
    iIconNotSelected = iconNotSelected;

    AknsUtils::CreateColorIconLC(skin, selectedSkinId, KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG14, bitmap, mask, avkonbmpFilename, selectedIconId,
                                 selectedMaskId, KRgbBlue, iconSize, EAspectRatioPreservedAndUnusedSpaceRemoved);

    CGulIcon* iconSelected = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); //bitmap, mask

    delete iIconSelected;
    iIconSelected = iconSelected;

    // create icons for highlighted mode
#ifdef RD_JAVA_S60_RELEASE_9_2
    AknsUtils::CreateColorIconLC(skin, notSelectedSkinId, KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG15, bitmap, mask, avkonbmpFilename, notSelectedIconId,
                                 notSelectedMaskId, KRgbBlue, iconSize, EAspectRatioPreservedAndUnusedSpaceRemoved);

    CGulIcon* iconNotSelectedHighlighted = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); //bitmap, mask

    delete iIconNotSelectedHighlighted;
    iIconNotSelectedHighlighted = iconNotSelectedHighlighted;

    AknsUtils::CreateColorIconLC(skin, selectedSkinId, KAknsIIDQsnIconColors,
                                 EAknsCIQsnIconColorsCG15, bitmap, mask, avkonbmpFilename, selectedIconId,
                                 selectedMaskId, KRgbBlue, iconSize, EAspectRatioPreservedAndUnusedSpaceRemoved);

    CGulIcon* iconSelectedHighlighted = CGulIcon::NewL(bitmap, mask);
    CleanupStack::Pop(2); //bitmap, mask

    delete iIconSelectedHighlighted;
    iIconSelectedHighlighted = iconSelectedHighlighted;
#endif // RD_JAVA_S60_RELEASE_9_2
}

// Base class overrides

// The model contains more than just the strings - need to override
// number of items query
TInt CMIDChoiceGroupModel::NumberOfItems() const
{
    return iElements ? iElements->Count() : 0;
}


// Provides a string with correct indices to images
TPtrC CMIDChoiceGroupModel::ItemText(TInt aItemIndex) const
{
    CMIDChoiceGroupElement* pElement = ElementAt(aItemIndex);

    if (pElement)
    {
        return (pElement->LboxText(aItemIndex + KElementIconStartIndex));
    }

    return TPtrC(KNullDesC);
}


// Base class override
void CMIDChoiceGroupModel::SetItemTextArray(MDesCArray* /*aItemTextArray*/)
{
    // Strings are stored in elements, so this is just to make sure
    // the base class does nothing.
}


// Returns a pointer to the item text array, which is not used,
// because strings are stored inside elements.
MDesCArray* CMIDChoiceGroupModel::ItemTextArray() const
{
    return NULL;
}


// Add an element to the end of the array
void CMIDChoiceGroupModel::AppendElementL(CMIDChoiceGroupElement* aElement)
{
    // Sanity check
    if (!iElements)
    {
        ASSERT(EFalse);
        return;
    }

    // If this is the first added element and choice is single selection
    // type, initially make the first item selected
    if ((iType != MMIDChoiceGroup::EMultiple) && (NumberOfItems() == 0))
    {
        aElement->SetSelected(ETrue);
    }

    iElements->AppendL(aElement);

    // If not in the middle of an update, refresh a bit
    if (!iUpdating)
    {
        // Refresh icon array
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (iObserver)
        {
            UpdateIconArrayL(iObserver->IsControlOnFormHighlighted());
        }
#else
        UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

        // Report event
        ReportEventL(MMIDChoiceGroupModelObserver::EElementAdded);
    }
}


// Insert an element at <aIndex>. Space must be reserved.
void CMIDChoiceGroupModel::InsertElementL(
    TInt aIndex,
    CMIDChoiceGroupElement* aElement)
{
    if ((iElements) && (aIndex >= 0) && (aIndex <= NumberOfItems()))
    {
        // If this is the first added element and choice is single selection
        // type, initially make the first inserted item selected
        if ((iType != MMIDChoiceGroup::EMultiple) && (NumberOfItems() == 0))
        {
            aElement->SetSelected(ETrue);
        }

        iElements->InsertL(aIndex, aElement);

        // If not in the middle of an update, refresh a bit
        if (!iUpdating)
        {
            // Refresh icon array
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (iObserver)
            {
                UpdateIconArrayL(iObserver->IsControlOnFormHighlighted());
            }
#else
            UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

            // Report element addition
            ReportEventL(MMIDChoiceGroupModelObserver::EElementAdded);
        }
    }
    else
    {
        ASSERT(EFalse);
    }
}


// Delete element at <aIndex>
void CMIDChoiceGroupModel::DeleteElementL(TInt aIndex)
{
    if ((iElements) && (aIndex >= 0) && (aIndex <= NumberOfItems()))
    {

        if (iType != MMIDChoiceGroup::EMultiple && NumberOfItems() > 1)
        {
            CMIDChoiceGroupElement* pElement = ElementAt(aIndex);

            if (pElement)
            {
                if (pElement->IsSelected())
                {
                    if ((aIndex + 1) < NumberOfItems())
                    {
                        CMIDChoiceGroupElement* pNextElement = ElementAt(aIndex+1);
                        pNextElement->SetSelected(ETrue);
                    }
                    else
                    {
                        CMIDChoiceGroupElement* pPrevElement = ElementAt(aIndex-1);
                        pPrevElement->SetSelected(ETrue);
                    }
                }
            }
        }

        // Grab hold of the element for deletion, remove from array
        // and compress the array
        CMIDChoiceGroupElement* ptr = iElements->At(aIndex);
        iElements->Delete(aIndex);
        delete ptr;
        iElements->Compress();

        // If not in the middle of an update, refresh a bit
        if (!iUpdating)
        {
            // Refresh icon array.
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (iObserver)
            {
                UpdateIconArrayL(iObserver->IsControlOnFormHighlighted());
            }
#else
            UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

            // Update the looks of the control
            ReportEventL(MMIDChoiceGroupModelObserver::EElementDeleted);
        }
    }
    else
    {
        ASSERT(EFalse);
    }
}


// Delete all elements
void CMIDChoiceGroupModel::DeleteAllL()
{
    if (iElements)
    {
        iElements->ResetAndDestroy();

        // If not updating, refresh now
        if (!iUpdating)
        {
            // Recreate icon array
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (iObserver)
            {
                UpdateIconArrayL(iObserver->IsControlOnFormHighlighted());
            }
#else
            UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

            // Report event
            ReportEventL(MMIDChoiceGroupModelObserver::EElementDeleted);
        }
    }
}


// Set properties of element at <aIndex>
void CMIDChoiceGroupModel::SetElementL(
    TInt aIndex,
    const TDesC& aText,
    CFbsBitmap* aBitmap,
    CFbsBitmap* aMask)
{
    CMIDChoiceGroupElement* pElement = ElementAt(aIndex);

    if (pElement)
    {
        // Set new properties
        pElement->SetTextL(aText);
        if (aBitmap)
        {
            pElement->SetIconL(*aBitmap, aMask);
        }
        else
        {
            pElement->SetIcon(NULL, EFalse);
        }

        // Refresh stuff, if not in the middle of an update
        if (!iUpdating)
        {
            // Icon may have changed, recreate array.
#ifdef RD_JAVA_S60_RELEASE_9_2
            if (iObserver)
            {
                UpdateIconArrayL(iObserver->IsControlOnFormHighlighted());
            }
#else
            UpdateIconArrayL();
#endif // RD_JAVA_S60_RELEASE_9_2

            // Report event
            ReportEventL(MMIDChoiceGroupModelObserver::EElementModified);
        }
    }
}


// Get element at position <aIndex>
CMIDChoiceGroupElement* CMIDChoiceGroupModel::ElementAt(TInt aIndex) const
{
    if ((iElements) && (aIndex >= 0) && (aIndex < NumberOfItems()))
    {
        return iElements->At(aIndex);
    }
    else
    {
        return NULL;
    }
}


// Reserve space for <aCount> elements
void CMIDChoiceGroupModel::SetReserveL(TInt aCount)
{
    if ((iElements) && (aCount >= NumberOfItems()))
    {
        iElements->SetReserveL(aCount);
    }
}


// Retrieves the icon array, recreates if flag is set
CArrayPtr<CGulIcon>* CMIDChoiceGroupModel::IconArray(TBool aReCreate)
{
    if (aReCreate)
    {
        // Recreate, trap & ignore leaves
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (iObserver)
        {
            TRAP_IGNORE(
                UpdateIconArrayL(iObserver->IsControlOnFormHighlighted()));
        }
#else
        TRAP_IGNORE(UpdateIconArrayL());
#endif // RD_JAVA_S60_RELEASE_9_2
    }

    return iIconArray;
}


// Set selection state of item at <index>.
// If an item is set selected in an exclusive type choice, also
// deselects other items.
// Deselection of an item in an exlusive choice has no effect.
void CMIDChoiceGroupModel::SelectElementL(TInt aIndex, TBool aSelected)
{
    ASSERT(iElements);

    // Quick sanity check
    if ((aIndex < 0) || (aIndex >= NumberOfItems()))
    {
        return;
    }

    // Exclusive and popup are both single-selection types
    if (iType != MMIDChoiceGroup::EMultiple)
    {
        if (!aSelected)
        {
            // Deselecting in an exclusive - void op
            return;
        }

        // Remove any previous selection
        ClearSelection();
    }

    // Set the selection state of the element
    CMIDChoiceGroupElement* pElement = ElementAt(aIndex);

    if (pElement)
    {
        pElement->SetSelected(aSelected);
    }

    // Report selection
    ReportEventL(MMIDChoiceGroupModelObserver::EElementSelected);
}


// Begin a model update. During an update, the observer will not
// reveive notifications about element actions.
void CMIDChoiceGroupModel::BeginUpdate()
{
    iUpdating = ETrue;

    // Report update start
    TRAP_IGNORE(ReportEventL(MMIDChoiceGroupModelObserver::EUpdateStarted));
}


// End a model update. At the end the icon array will be
// recreated, and the associated control refreshed.
void CMIDChoiceGroupModel::EndUpdate()
{
    iUpdating = EFalse;

    // Create array, refresh control
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iObserver)
    {
        TRAP_IGNORE(UpdateIconArrayL(iObserver->IsControlOnFormHighlighted()));
    }
#else
    TRAP_IGNORE(UpdateIconArrayL());
#endif // RD_JAVA_S60_RELEASE_9_2

    // Report update end (observer should redraw)
    TRAP_IGNORE(ReportEventL(MMIDChoiceGroupModelObserver::EUpdateEnded));
}


// Returns the index of the selected element in an exclusive
// choice, or -1
TInt CMIDChoiceGroupModel::SelectedElement()
{
    TInt nSelected = -1;

    // Exclusive & Popup are both single-selection
    if (iType != MMIDChoiceGroup::EMultiple)
    {
        // Loop through the elements, look for a selected one
        TInt nCount = NumberOfItems();

        for (TInt i = 0; i < nCount; i++)
        {
            CMIDChoiceGroupElement* pElement = ElementAt(i);

            if (pElement)
            {
                if (pElement->IsSelected())
                {
                    // Found, break out
                    nSelected = i;
                    break;
                }
            }
        }
    }

    return nSelected;
}


// --- private functions ---

// Clears all selections, does not report, as is used when
// subsequently a selection is set (in an exclusive choice)
void CMIDChoiceGroupModel::ClearSelection()
{
    // Loop through the elements and clear element selections
    TInt nCount = NumberOfItems();
    for (TInt i = 0; i < nCount; i++)
    {
        CMIDChoiceGroupElement* pElement = ElementAt(i);

        if (pElement)
        {
            pElement->SetSelected(EFalse);
        }
    }
}


// Reports element actions to observer
void CMIDChoiceGroupModel::ReportEventL(
    MMIDChoiceGroupModelObserver::TChoiceGroupModelEvent aEvent)
{
    if (iObserver)
    {
        iObserver->HandleChoiceGroupModelEventL(this, aEvent);
    }
}


// Create an icon array to be given to the lbox column data.
// This should be done every time the contents of the model are changed,
// or icon array is retrieved with the create flag set.
// NOTE that the indices in the array and the item strings should
// be synchronised. The selection icon is always at index
// 0 (selected) and 1 (not selected)
#ifdef RD_JAVA_S60_RELEASE_9_2
void CMIDChoiceGroupModel::UpdateIconArrayL(TBool aHighlighted)
#else
void CMIDChoiceGroupModel::UpdateIconArrayL()
#endif // RD_JAVA_S60_RELEASE_9_2
{
    ASSERT(iElements);
    ASSERT(iIconArray);

    // Clear array, DO NOT DELETE icons
    iIconArray->Reset();

    TInt nCount = NumberOfItems();

    if (nCount <= 0)
    {
        return;
    }

    // First add the selection icons
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aHighlighted)
    {
        iIconArray->AppendL(iIconSelectedHighlighted);
        iIconArray->AppendL(iIconNotSelectedHighlighted);
    }
    else
    {
        iIconArray->AppendL(iIconSelected);
        iIconArray->AppendL(iIconNotSelected);
    }
#else
    iIconArray->AppendL(iIconSelected);
    iIconArray->AppendL(iIconNotSelected);
#endif // RD_JAVA_S60_RELEASE_9_2

    // Then loop through the elements and add their icons
    for (TInt i = 0; i < nCount; i++)
    {
        CMIDChoiceGroupElement *pElement = ElementAt(i);

        if (pElement)
        {
            CGulIcon* icon = pElement->Icon();

            if (icon)
            {
                // Add the elements icon
                iIconArray->AppendL(icon);
            }
            else
            {
                // Add a zero-sized dummy icon
                iIconArray->AppendL(iIconDummy);
            }
        }
    }
}

TBool CMIDChoiceGroupModel::HasIcons() const
{
    TInt itemCount = NumberOfItems();
    for (TInt i = 0; i < itemCount; ++i)
    {
        CMIDChoiceGroupElement *element = ElementAt(i);

        if (element && element->Icon())
        {
            return ETrue;
        }
    }

    return EFalse;
}

