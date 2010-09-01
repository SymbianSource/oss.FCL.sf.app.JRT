/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the lcdui List component for series 60
*
*/


// INCLUDE FILES
#include "CMIDList.h"
// CMIDDisplayable API for iDisplayable
#include "CMIDDisplayable.h"
// CMIDUtils::CopyBitmapL used in CreateIconLC
#include "CMIDUtils.h"
// using MMIDBitmapImage API in CreateIconLC
#include "lcdgr.h"
#include "lcdui.h"
#include "CMIDFont.h"
// using TMifAvkon enumeration, constants for icons
#include <avkon.mbg>
// classes definitions for creating specific list boxes
#include <aknlists.h>
// CAknSingle2GraphicStyleListBox class for creating specific list box
// CAknDouble2GraphicLargeStyleListBox class for creating specific list box
#include <aknjavalists.h>
// API for ColumnListBoxData
#include <eikclbd.h>
#include <eiklbv.h>
#include <AknBidiTextUtils.h>
// using TDisplayMode in CreateIconLC function for creating alpha bitmap
#include <gdi.h>
// using constant related to list box
#include <avkon.hrh>
#include <AknsDrawUtils.h>
#include <AknsFrameBackgroundControlContext.h>
#include <AknPictographInterface.h>
#include <skinlayout.cdl.h>
// LAF
#include <aknlayoutscalable_avkon.cdl.h> // for layout data
// using CGulIcon API for icons
#include <gulicon.h>
// CEikTextListBox API for iListBox
#include <eiktxlbx.h>
// Predefined directory paths for the use of ROM based sw
// used for retrieving selection icons
#include <data_caging_path_literals.hrh>
#include <j2me/jdebug.h>

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// API for Popup controller (iPopupController)
#include "CMIDPopupNoteController.h"

/** This macro is executed each time a trapped call returns an error code different than KErrNone */
#undef  TRAP_INSTRUMENTATION_LEAVE
#define TRAP_INSTRUMENTATION_LEAVE(aResult) DEBUG_INT2("In CMIDList.cpp, trapped method was called at line %D and got exception %D", __LINE__, aResult);

// CONSTANTS
_LIT(KNoIconTab,    "\t");
_LIT(KIconTab,      "1\t");
_LIT(KTabulator,    "\t");
_LIT(KSpace,        " ");

// CONSTANTS used for filtering new lines from text
// Do not change sequence
_LIT(KCarriageReturnLineFeedLit,  "\r\n");
_LIT(KCarriageReturnLit,          "\r");
_LIT(KLineFeedLit,                "\n");
_LIT(KLineSeparatorLit,           "\u2028");
_LIT(KEllipsisLit,                "\u2026");

const TUint16 KExtraTabCharacters = 3;
const TUint16 KMaxNum = 10;
const TUint16 KBlankIconIndex = 0;

const TUint16 KUnselected   = '1';
const TUint16 KSelected     = '2';

const TUint16 KIconsImplicit = 1;
const TUint16 KIconsExplicit = 3;

const TUint16 KIconsNotSelected = 1;
const TUint16 KIconsSelected    = 2;

//The first text column in each list type (TListBoxType).
//Index 0 is undefined
const TInt KListTextColumn[] = {-1, 1, 1, 2, 1, 1, 1, 2, 1, 2};

/** The timeout in microseconds for visual updates. When items are added or removed
or modified, the list is not redrawn immediately but only after this timeout. This way
if more items are being added or removed or changed we reduce the number of redraws.
*/
const TInt KVisualUpdateTimeout = 60000; //60 msecs

/*If a visual update has been requested for a period longer than the cap then perform layout
even if the midlet is still performing list update operations. */
const TInt KVisualUpdateCap = 600000; //600 msecs


// ============================ MEMBER FUNCTIONS ===============================

//
// CMIDList::CMIDListItem
//


CMIDList::CMIDListItem* CMIDList::CMIDListItem::NewLC(const TDesC& aText, TInt aIconIndex)
{
    CMIDListItem* self = new(ELeave) CMIDListItem(aIconIndex);
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    return self;
}

CMIDList::CMIDListItem::~CMIDListItem()
{
    delete iText;
    delete iFormatedText;
    iFont = NULL; //not owned
}

/**
 * Return the formatted text of the item. Formatted text contains icon indexes and item text
 * separated by tab characters. Each listbox type requires a different formatted text. See
 * aknlist.h for the details of each listbox.
 *
 * @param aListBoxType The type of avkon list box - this is owned by CMIDList
 * @param aChoiceType  The type of java list box - this is also owned by CMIDList
 *
 * According to aChoiceType we use avkon listboxes differently. And also accordingly to
 * the avkon listbox the formatted text may be different. Hence there is a first if
 * statement on the choice type and then a switch inside each if clause.
 **/
HBufC* CMIDList::CMIDListItem::FormattedTextLC
(const CMIDList::TListBoxType& aListBoxType,
 const MMIDChoiceGroup::TChoiceType& aChoiceType) const
{
    TBuf<KMaxNum> index;

    if (iIconIndex != KBlankIconIndex)
    {
        index.AppendNum(iIconIndex);
    }

    index.Append(KTabulator);

    // single styled ListBox types, fit policy is wrap off, iFormatedText used
    if (aListBoxType == ESingleStyle || aListBoxType == ESingleGraphicStyle
            || aListBoxType == ESingleLargeStyle || aListBoxType == ESingle2GraphicStyle
            || aListBoxType == ESingleGraphicLargeStyle)
    {
        HBufC* text = HBufC::NewMaxLC(KExtraTabCharacters + index.Size() + iFormatedText->Length());
        TPtr textPtr = text->Des();

        if (aChoiceType == MMIDChoiceGroup::EImplicit)
        {
            switch (aListBoxType)
            {
            case ESingleStyle:
                textPtr = KNoIconTab;
                textPtr += *iFormatedText;
                break;
            case ESingleGraphicStyle:
            case ESingleLargeStyle:
                textPtr = index;
                textPtr += *iFormatedText;
                break;
            default:
                User::Invariant();
            }
        }
        else
        {
            switch (aListBoxType)
            {
            case ESingleGraphicStyle:
                textPtr = KIconTab;
                textPtr += *iFormatedText;
                break;
            case ESingle2GraphicStyle:
                textPtr = KIconTab;
                textPtr += index;
                textPtr += *iFormatedText;
                break;
            case ESingleGraphicLargeStyle:
                textPtr = KIconTab;
                textPtr += index;
                textPtr += *iFormatedText;
                textPtr += KTabulator;
                textPtr += KSpace;
                break;
            default:
                User::Invariant();
            }
        }
        return text;
    }
    // double styled ListBox types, fit policy is wrap on, iText used
    else
    {
        HBufC* text = HBufC::NewMaxLC(KExtraTabCharacters + index.Size() + iText->Length());
        TPtr textPtr = text->Des();

        if (aChoiceType == MMIDChoiceGroup::EImplicit)
        {
            switch (aListBoxType)
            {
            case EDouble2Style:
                textPtr = KNoIconTab;
                textPtr += *iText;
                textPtr += KTabulator;
                break;
            case EDouble2GraphicStyle:
            case EDouble2LargeStyle:
                textPtr = index;
                textPtr += *iText;
                textPtr += KTabulator;
                break;
            default:
                User::Invariant();
            }
        }
        else
        {
            switch (aListBoxType)
            {
            case EDouble2GraphicStyle:
                textPtr = KIconTab;
                textPtr += *iText;
                textPtr += KTabulator;
                break;
            case EDouble2GraphicLargeStyle:
                textPtr = KIconTab;
                textPtr += index;
                textPtr += *iText;
                textPtr += KTabulator;
                break;
            default:
                User::Invariant();
            }
        }
        return text;
    }
}

TDesC16& CMIDList::CMIDListItem::Text()
{
    ASSERT(iText);
    return *iText;
}

TInt CMIDList::CMIDListItem::IconIndex() const
{
    return iIconIndex;
}

MMIDFont* CMIDList::CMIDListItem::Font() const
{
    return iFont;
}

TBool CMIDList::CMIDListItem::Selected() const
{
    return iSelected;
}

TBool CMIDList::CMIDListItem::GetPopupNeeded() const
{
    return iPopupNeeded;
}

void CMIDList::CMIDListItem::SetIconIndex(TInt aIconIndex)
{
    ASSERT(aIconIndex >= 0);
    iIconIndex = aIconIndex;
}

/** Replace the text of the item. Delete the memory of the old text. */
void CMIDList::CMIDListItem::SetTextL(const TDesC& aText)
{
    HBufC* newText = aText.AllocL();
    CleanupStack::PushL(newText);
    HBufC* newFormatedText = aText.AllocL();
    CleanupStack::Pop(newText);
    delete iText;
    iText = newText;
    delete iFormatedText;
    iFormatedText = newFormatedText;

    iPopupNeeded = EFalse;

    RemoveNewLineCharactersL();
    RemoveTabulators();
}

void CMIDList::CMIDListItem::SetFont(MMIDFont* aFont)
{
    iFont = aFont;
}

void CMIDList::CMIDListItem::SetSelected(TBool aSelected)
{
    iSelected = aSelected;
}

CMIDList::CMIDListItem::CMIDListItem(TInt aIconIndex)
        : iIconIndex(aIconIndex), iSelected(EFalse)
{
}

void CMIDList::CMIDListItem::ConstructL(const TDesC& aText)
{
    iPopupNeeded = EFalse;

    iText = aText.AllocL();
    RemoveTabulators();

    iFormatedText = iText->Des().AllocL();
    RemoveNewLineCharactersL();
}

void CMIDList::CMIDListItem::RemoveTabulators()
{
    TPtr textPtr = iText->Des();
    TInt tabPosition = textPtr.Find(KTabulator);

    while (tabPosition != KErrNotFound)
    {
        ASSERT(KSpace().Length() <= KTabulator().Length());
        textPtr.Replace(tabPosition, KTabulator().Length(), KSpace);
        tabPosition = textPtr.Find(KTabulator);
    }
}


// Searching new line characters in formatedText, which is copy of item text.
// If new line is found then:
// - new line character is replaced by ellipsis (...),
// - iPopupNeeded is updated to show popup window in
//   CMIDList::ShowInfoPopupL() when needed.
void CMIDList::CMIDListItem::RemoveNewLineCharactersL()
{
    const TInt KNewLineCharsArrayLength = 4;
    const TPtrC KNewLineCharsArray[KNewLineCharsArrayLength] =
    {
        KCarriageReturnLineFeedLit(),
        KCarriageReturnLit(),
        KLineFeedLit(),
        KLineSeparatorLit()
    };

    const TInt ellipsisLen = KEllipsisLit().Length();

    TBool addedEllipsis = EFalse;
    TPtr textPtrNL = iFormatedText->Des();
    TInt textPtrNLLen = textPtrNL.Length();

    // position of first new line separator
    TInt firstNewLinePos = KErrNotFound;

    // position after first new line separator
    TInt endPos = KErrNotFound;

    // count of line separator
    TInt countChar = 0;

    for (TInt i = 0; i < KNewLineCharsArrayLength; i++)
    {
        firstNewLinePos = textPtrNL.Find(KNewLineCharsArray[i]);

        // if found new line separator
        if (firstNewLinePos != KErrNotFound)
        {
            // if layout alignment is left
            if (!AknLayoutUtils::LayoutMirrored())
            {
                endPos = firstNewLinePos + KNewLineCharsArray[i].Length();

                // There must be a memory reallocation when formated text
                // contains: a piece of text till new line character and
                // ellipsis added, is longer than unformated text.
                if (firstNewLinePos + ellipsisLen + 2 > textPtrNLLen)
                {
                    textPtrNL.Delete(firstNewLinePos,
                                     textPtrNL.Length() - firstNewLinePos);

                    iFormatedText = iFormatedText->ReAllocL(
                                        firstNewLinePos + ellipsisLen);

                    textPtrNL.Set(iFormatedText->Des());

                    if (countChar != 0)
                    {
                        textPtrNL += KEllipsisLit;
                    }

                    ++countChar;
                }
                // when memory reallocation isn't needed
                else
                {
                    textPtrNL.Delete(endPos, textPtrNL.Length() - endPos);

                    textPtrNL.Replace(
                        firstNewLinePos,
                        KNewLineCharsArray[i].Length(),
                        KEllipsisLit);

                    ++countChar;
                }
            }
            // if layout alignment is right
            else
            {
                // Contains: Delete ellipsis if before has been added and
                // a piece of text till new line character
                if (countChar > 0 && addedEllipsis)
                {
                    textPtrNL.Delete(0, ellipsisLen);
                    textPtrNLLen = textPtrNL.Length();

                    textPtrNL.Delete(
                        firstNewLinePos - ellipsisLen,
                        textPtrNLLen - firstNewLinePos + ellipsisLen);

                    firstNewLinePos -= ellipsisLen;
                }
                else
                {
                    textPtrNLLen = textPtrNL.Length();

                    textPtrNL.Delete(
                        firstNewLinePos,
                        textPtrNLLen - firstNewLinePos);
                }

                iFormatedText = iFormatedText->ReAllocL(
                                    textPtrNL.Length() + ellipsisLen);

                textPtrNL.Set(iFormatedText->Des());

                // If new line character is last character of the text and
                // if count of new line character is greater than zero then
                // add ellipsis.
                if (countChar != 0 &&
                        firstNewLinePos + ellipsisLen + 2 > textPtrNLLen)
                {
                    textPtrNL.Insert(0, KEllipsisLit);
                    addedEllipsis = ETrue;
                }
                else
                    // If new line character is not last character of the text
                    // then add ellipsis.
                {
                    if (firstNewLinePos + ellipsisLen + 2 <= textPtrNLLen)
                    {
                        textPtrNL.Insert(0, KEllipsisLit);
                        addedEllipsis = ETrue;
                    }
                    else
                    {
                        addedEllipsis = EFalse;
                    }
                }

                ++countChar;
            }

            iPopupNeeded = addedEllipsis;
        }
    }
}


//
// CMIDList
//

//
// Public construction and destruction methods
//

CMIDList* CMIDList::NewL(MMIDEnv& aEnv, MMIDDisplayable& aDisplayable, TInt aType,
                         RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray)
{
    CMIDList* self = new(ELeave) CMIDList(aEnv,aType);
    CleanupStack::PushL(self);
    self->ConstructL(aDisplayable, aStringArray, aImageArray);
    CleanupStack::Pop(self);
    return self;
}

CMIDList::~CMIDList()
{
    TRAP_IGNORE(DeleteAllL());
    delete iPopupController;

    iItems.ResetAndDestroy();

    delete iListBox;
    iListBox = NULL;
    if (iIconArray && iIconArrayOwned)
    {
        iIconArray->ResetAndDestroy();
        delete iIconArray;
    }

    if (iBuiltInSelectCommand)
    {
        iBuiltInSelectCommand->SetObserver(NULL);
        iBuiltInSelectCommand->Dispose();
        iBuiltInSelectCommand = NULL;
    }
    if (iMarkCommand)
    {
        iMarkCommand->SetObserver(NULL);
        iMarkCommand->Dispose();
        iMarkCommand = NULL;
    }
    if (iUnmarkCommand)
    {
        iUnmarkCommand->SetObserver(NULL);
        iUnmarkCommand->Dispose();
        iUnmarkCommand = NULL;
    }

    delete iVisualUpdateTimer;

    // Displayable is notified about content control deletion
    if (iDisplayable)
    {
        iDisplayable->NotifyContentDestroyed();
    }

}

//
// Plugin internal public API
//

CMIDDisplayable* CMIDList::Displayable()
{
    return iDisplayable;
}


/**
 * Insert an element in the listbox. First of all request a visual update. This will
 * start or re-start the visual update timer. Then manually update the highlight position.
 * Because we may need to recreate the listbox later on during the visual update, we must
 * store the current highlight position. Then call DoInsertElementL() which takes care of
 * updating list box parameters (remove an item from the model of the current list box)
 * and also set the flag for recreating the listbox during the visual update, if needed
 * at all, see DoInsertElementL() for details. @see RequestVisualUpdateL().
 **/
void CMIDList::InsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    RequestVisualUpdateL();

    if (aIndex <= iHighlightPosition)
    {
        iHighlightPosition++;
    }

    DoInsertElementL(aIndex, aText, aImage);


    // This is first item added to the list
    if (iItems.Count() == 1)
    {
        if (iSelectCommand && iSelectCommand != iBuiltInSelectCommand &&
                iDisplayable->FindCommandWithId(iSelectCommandId) == NULL)
        { //Select command is user defined and it's not in the command list
            // The select command needs to be enabled and added to the command list
            iDisplayable->SetSelectCommandState(ETrue);
            iDisplayable->AddCommandL(iSelectCommand);

        }
        else if ((iChoiceType == MMIDChoiceGroup::EExclusive))
        {//If there is only one item in the Exclusive list, select it.
            SelectElementL(0, ETrue);
            // MSK label needs to be updated
            UpdateCommands();
        }

    }
}

/**
 * Delete an element from the list box. See InsertElementL(),
 * similar considerations apply. @see RequestVisualUpdateL().
 **/
void CMIDList::DeleteElementL(TInt aIndex)
{
    //Transfer the selection to the proper element before deleting current selection
    if ((iChoiceType == MMIDChoiceGroup::EExclusive) && (iItems.Count() > 1))
    {
        if (iItems[aIndex]->Selected())
        {
            if (aIndex == (iItems.Count() -1))
            {
                SelectElementL(aIndex-1, ETrue);
            }
            else
            {
                SelectElementL(aIndex+1, ETrue);
            }
        }
    }

    RequestVisualUpdateL();
    iPopupController->HidePopup();

    if (aIndex < iHighlightPosition)
    {
        iHighlightPosition--;
    }

    DoDeleteElement(aIndex);

#ifdef RD_JAVA_S60_RELEASE_9_2
    if (!iItems.Count())
    {
        // Last element was deleted, highlight is removed at all.
        // In case of IMPLICIT List without highlight, CBA has to be updated,
        // so that no ITEM or OK commands are mapped to soft keys
        // or populated to Options menu.
        iHighlight = EFalse;
        if (iChoiceType == MMIDChoiceGroup::EImplicit && iDisplayable)
        {
            iDisplayable->InitializeCbasL();
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

/**
 * Delete all elements from the existing list box.
 *
 * After requesting a visual update, all elements in the listbox model
 * are removed and all items are destroyed.
 *
 * Also, the icon array is cleared (which the exception of the first
 * icons that are used for the checkbox in the IMPLICIT listboxes).
 * After clearing the icon array we call SetMaxIconSize(). This will reset
 * the max icon size. @see RequestVisualUpdateL().
 **/
void CMIDList::DeleteAllL()
{
    RequestVisualUpdateL();
    iPopupController->HidePopup();

    static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray())->Reset();
    iItems.ResetAndDestroy();
    iHighlightPosition = 0;

    TInt firstIcon = KIconsExplicit; // blank icon + selection icons (on/off)
    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        firstIcon = KIconsImplicit; // blank icon
    }

    iIconArray->Delete(firstIcon, iIconArray->Count() - firstIcon);
    SetMaxIconSize();

#ifdef RD_JAVA_S60_RELEASE_9_2
    // All items are deleted, highlight is removed.
    // In case of IMPLICIT List without highlight, CBA has to be updated,
    // so that no ITEM or OK commands are mapped to soft keys
    // or populated to Options menu.
    iHighlight = EFalse;
    if (iChoiceType == MMIDChoiceGroup::EImplicit && iDisplayable)
    {
        iDisplayable->InitializeCbasL();
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

/**
 * Change the image and text of an existing element. After requesting a
 * visual update, change the item text and delete any exising icon. Create
 * a new icon (if the new image is not null)
 * and see if the max icon size has changed by calling SetMaxIconSize().
 * Set iNeedToRecreateListBox so that if needed the LB will be recreated
 * during the visual update. Setting this flag does not mean the listbox will
 * be recreated. It will only be recreated if during the visual update this
 * is required. It only means we will check if the listbox must be recreated
 * at all.
 *
 * Then set the item text in the old list box. If the LB is recreated,
 * this is not needed. @see RequestVisualUpdateL().
 **/
void CMIDList::SetElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    ASSERT(aIndex >= 0 && aIndex < iItems.Count());
    RequestVisualUpdateL();

    CMIDListItem* item = iItems[aIndex];
    item->SetTextL(aText);

    TInt iconIndex = item->IconIndex();

    if (iconIndex != KBlankIconIndex)
    {
        delete(iIconArray->At(iconIndex));
        iIconArray->At(iconIndex) = NULL;
    }

    item->SetIconIndex(InsertNewIconL(aImage));

    SetMaxIconSize();
    iNeedToRecreateListBox = ETrue;

    static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray())->Delete(aIndex);

    HBufC* text = item->FormattedTextLC(iListBoxType, iChoiceType);
    static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray())->InsertL(aIndex, *text);
    CleanupStack::PopAndDestroy(text);
}

/**
 * Requests a visual update. Check if there already is a visual update request
 * outstanding (iUpdateRequestTime has been set to a value different that -1). If
 * this is the case check how long ago the visual update was requested. If it is
 * longer than KVisualUpdateCap then do the visual update, see UpdateOrRecreateListBoxL().
 * Otherwise just cancel the timer. If this is the first visual update then set
 * iUpdateRequestTime and the current highlight position in the listbox.
 * Note: if the user changes the highlight position
 * between now and the actual visual update the correct highlight position will be
 * lost. This is very unlikely but possible.
 *
 * Finally restart the visual update timer unless there is no need for it
 * (UpdateOrRecreateListBoxL()) already called. When the timer expires UpdateOrRecreateListBoxL()
 * will be called.
 **/
void CMIDList::RequestVisualUpdateL()
{
    if (iUpdateRequestTime != -1)
    {
        TTime now;
        now.HomeTime();
        if (now.MicroSecondsFrom(iUpdateRequestTime) > KVisualUpdateCap)
        {
            UpdateOrRecreateListBoxL();
        }
        else
        {
            iVisualUpdateTimer->Cancel();
        }
    }
    else
    {
        iUpdateRequestTime.HomeTime();
        iHighlightPosition = iListBox->View()->CurrentItemIndex();
    }


    if (iUpdateRequestTime != -1)
    {
        iVisualUpdateTimer->Start(KVisualUpdateTimeout,
                                  0,
                                  TCallBack(VisualUpdateTimerCallback, this));
    }
}

/** Executed at the expiry of the visual update timer. Call UpdateOrRecreateListBoxL(). */
TInt CMIDList::VisualUpdateTimerCallback(TAny* aThis)
{
    CMIDList* self = static_cast<CMIDList*>(aThis);
    ASSERT(self);

    TRAPD(err,self->UpdateOrRecreateListBoxL());
    return err;
}

/**
 * Peroform a visual update. First stop the visual update timer. If iNeedToRecreateListBox has
 * been set call CreateListBoxL() - which will only create the list box if needed.
 * Then call PopulateListBoxL() if the listbox has been recreated. Finally call
 * UpdateListBoxL() to update the listbox and reset iNeedToRecreateListBox, iUpdateRequestTime and
 * highlight position. @see  CreateListBoxL(), PopulateListBoxL() and UpdateListBoxL().
 **/
void CMIDList::UpdateOrRecreateListBoxL()
{
    iVisualUpdateTimer->Cancel();

    if (iNeedToRecreateListBox)
    {
        iNeedToRecreateListBox = EFalse;
        if (CreateListBoxL())
        {
            PopulateListBoxL();
        }
    }

    iUpdateRequestTime = -1;

    UpdateListBoxL();

    // If there are some items, ensure that Select command is added
    // ie. if there is a select command added while the VisualUpdateTimer is running
    if (iItems.Count() > 0)
    {
        if (iSelectCommand && iSelectCommand != iBuiltInSelectCommand &&
                (!iDisplayable->FindCommandWithId(iSelectCommandId)))
        { //Select command is user defined and it's not in the command list
            // The select command needs to be enabled and added to the command list
            iDisplayable->SetSelectCommandState(ETrue);
            iDisplayable->AddCommandL(iSelectCommand);
        }
    }

    UpdateCommands();
}

/** Update iListBox. This is either a listbox that has been existing for some time
 * and has had some items added and/or a listbox that has just been created: we
 * do not make this distinction to keep the code simpler. It does mean some
 * operations are redundant however. We set the highlight position into the listbox
 * by calling SetCurrentItemIndex() and using iHighlightPosition. HandleItemRemovalL()
 * and HandleItemAdditionL() are always called even if this is not strictly needed.
 * For each listbox element, its font and select status must be re-applied. Once again,
 * this is strictly needed only for new listboxes. Finally, request a draw by
 * calling DrawDeferred().
 **/
void CMIDList::UpdateListBoxL()
{
    if (iHighlightPosition < 0)
    {
        iHighlightPosition = 0;
    }
    if (iHighlightPosition >= iItems.Count())
    {
        iHighlightPosition = iItems.Count() > 0 ? iItems.Count() - 1 : 0;
    }

    iListBox->SetCurrentItemIndex(iHighlightPosition);
    iListBox->HandleItemRemovalL();
    iListBox->HandleItemAdditionL();

    TInt numItems = iItems.Count();
    for (TInt i=0; i < numItems; i++)
    {
        UpdateFontToElementL(i);

        //Update selection status for element in the current list box,
        //for choicetype EImplicit and EExclusive, when i == iHighlightPosition
        //set ETrue, otherwise EFalse
        if (iChoiceType == MMIDChoiceGroup::EExclusive ||
                iChoiceType == MMIDChoiceGroup::EImplicit)
        {
            SetSelectionIntoListBoxL(i, (i == iHighlightPosition ? ETrue : EFalse));
        }
        else if (iChoiceType == MMIDChoiceGroup::EMultiple)
        {
            SetSelectionIntoListBoxL(i, iItems[i]->Selected());
        }
    }

    DrawDeferred();
}

/**
 * Select or de-select an element. Items are selected and deselected by
 * storing a flag in CMIDListItem and by modifying the listbox visual appearance,
 * see SetSelectionIntoListBoxL(). Both things must be done when chaning the selection
 * status of an element. Therefore, if there is an outstanding listbox visual update this must
 * be performed, see UpdateOrRecreateListBoxL(). For exclusive choicetype listboxes, there
 * can only be one element selected at all times. Also, there must be one element selected
 * at all times. Therefore, when selecting a new element we check if there is another one
 * currently selected and if so we de-select it. Whereas when deselecting an element we make
 * sure there is another one selected. We choose the following element available unless it is the
 * last one in which case we choose the previous element.
 **/
void CMIDList::SelectElementL(TInt aIndex,TBool aSelected)
{
    ASSERT(aIndex >= 0 && aIndex < iItems.Count());

    if (iUpdateRequestTime != -1)
    {
        UpdateOrRecreateListBoxL();
    }

    if (iChoiceType == MMIDChoiceGroup::EExclusive && aSelected)
    { // exclusive listboxes can have one selected item at most so
        // deselect any previous selected items
        TInt numItems = iItems.Count();
        for (TInt i = 0; i < numItems; i++)
        {
            if (i != aIndex && iItems[i]->Selected())
            {
                SetSelectionIntoListBoxL(i, EFalse);
                iItems[i]->SetSelected(EFalse);
                break; //only one can be selected
            }
        }
    }

    // This flag is only used for exclusive lboxes - to make sure there is at
    //least one item selected
    TBool needToSelectAnotherItem = !aSelected && iItems[aIndex]->Selected();

    iItems[aIndex]->SetSelected(aSelected);
    SetSelectionIntoListBoxL(aIndex, aSelected);

    if (iChoiceType == MMIDChoiceGroup::EExclusive && needToSelectAnotherItem)
    { //exclusive listboxes must have at least one item selected
        TInt newSelIndex =
            (iItems.Count() > 1 ? (aIndex < (iItems.Count()-1) ? aIndex++ : aIndex--) : 0);

        if (newSelIndex >= 0)
        {
            SetSelectionIntoListBoxL(newSelIndex, ETrue);
            iItems[newSelIndex]->SetSelected(ETrue);
        }
    }
}


/**
 * Return true if the item at position aIndex is currently selected. We perform a
 * visual update if there is one outstanding, see UpdateOrRecreateListBoxL(). This is
 * because item selection depends on the visual listbox status (this is strictly true for
 * implicit choicetype only but we perform it for all types just to be consistent).
 * So to determine if an item is selected, implicit listboxes rely on the listbox
 * selected item (the position of the highlight) whereas the other two types of
 * listboxes rely on the information stored in the listbox items, see CMIDListItem.
 **/
TBool CMIDList::IsSelected(TInt aIndex)
{
    ASSERT(aIndex >= 0 && aIndex < iItems.Count());

    if (iUpdateRequestTime != -1)
    {
        TRAP_IGNORE(UpdateOrRecreateListBoxL());
    }

    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        return aIndex == iListBox->View()->CurrentItemIndex();
    }
    else
    {
        return iItems[aIndex]->Selected();
    }
}

/**
 * Set the font to a list element. The font could be NULL
 *  in which case the default font should be used.
 **/
void CMIDList::SetFontL(TInt aIndex, MMIDFont* aFont)
{
    if (aIndex < 0 || aIndex >= iItems.Count())
        return;

    iItems[aIndex]->SetFont(aFont);
    RequestVisualUpdateL();
}

/** Sets the new fit policy and recreates the list box if needed. @see CreateListBoxL() */
void CMIDList::SetFitPolicyL(TInt aFitPolicy)
{
    if (iFitPolicy != MMIDChoiceGroup::TFitPolicy(aFitPolicy))
    {
        iFitPolicy = MMIDChoiceGroup::TFitPolicy(aFitPolicy);
        iNeedToRecreateListBox = ETrue;
        iHighlightPosition = iListBox->View()->CurrentItemIndex();
        UpdateOrRecreateListBoxL();
    }
}

#ifdef RD_JAVA_S60_RELEASE_9_2
TInt CMIDList::IsHighlighted()
{
    return iHighlight;
}
#endif // RD_JAVA_S60_RELEASE_9_2

/**
 * Notifies this MMIDList that the select command has changed.
 * This is only called on IMPLICIT Lists.
 *
 * This is only relevant to MMIDList implementing classes that provide a
 * visual means of invoking the select operation, e.g. a soft key.
 * If this is the case, the visual means should be removed when the select
 * command is set to null or to an application provided command.
 *
 * @param    aCommandID, one of:
 *           The ID of a command that has been added to the list via AddCommandL
 *           - in this case the List should look up the command in its collection
 *             and use the label for its selection mechanism.
 *
 *           MMIDCommand::EListSelectCommand
 *           - in this case the List should use the default label for its
 *             selection mechanism.
 *
 *           MMIDCommand::ENullCommand
 *           - in this case the list should not support selection. This value will
 *             be passed in response to the application calling: List.setSelectCommand(null);
 *
 *  MMIDCommand::ENullCommand is correctly received when setSelectCommand(null) is called.
 *  However in the other cases the command id is passed instead of MMIDCommand::EListSelectCommand.
 **/
void CMIDList::SetSelectCommand(TInt aCommandID)
{
    iSelectCommandId = aCommandID;
    if (aCommandID == CMIDCommand::ENullCommand)
    { // enable context sensitive menu
        iSelectCommand = NULL;
    }
    else
    { // enable msk command
        // Try to find the command from displayable:
        // - if found, command is set by the application -> use it to display correct label
        // - if not found, use built-in command
        iSelectCommand = iDisplayable->FindCommandWithId(aCommandID);
        if (!iSelectCommand)
        {
            iSelectCommand = iBuiltInSelectCommand;
        }
    }

    iDisplayable->SetSelectCommand(iSelectCommand);
    UpdateCommands();
}

#ifdef RD_JAVA_S60_RELEASE_9_2
void CMIDList::PostPendingUpEventL()
{
    if (iUpEventPending)
    {
        iListBox->HandlePointerEventL(iPendingUpEvent);
        iUpEventPending = EFalse;
        iLongTapDetected = EFalse;
    }
}

MMIDChoiceGroup::TChoiceType CMIDList::ListChoiceType()
{
    return iChoiceType;
}
#endif // RD_JAVA_S60_RELEASE_9_2

void CMIDList::UpdateCommands()
{
    if (!iListBox)
    {
        return;
    }

    TInt index = iListBox->CurrentItemIndex();
    if (index == KErrNotFound)
    { // the list must be empty
        iDisplayable->SetSelectCommandState(EFalse); // MSK must not appear
        iDisplayable->SetMSKCommand(NULL);
        return;
    }
    TBool selected = IsSelected(index);
    CMIDCommand* mskCommand = NULL;
    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        mskCommand = iSelectCommand;
    }
    else if (iChoiceType == MMIDChoiceGroup::EExclusive)
    {
        // if the current item is selected, enable context menu by setting
        // MSK cmd to NULL, othewise set it to appropriate select command;
        // either application provided (iSelectCommand) if not null or to
        // builtIn select command if application provided is null
        CMIDCommand* selectCommand = (iSelectCommand ? iSelectCommand : iBuiltInSelectCommand);
        mskCommand = selected ? NULL : selectCommand;
    }
    else if (iChoiceType == MMIDChoiceGroup::EMultiple)
    {
        mskCommand = selected ? iUnmarkCommand : iMarkCommand;
    }
    iDisplayable->SetSelectCommandState(ETrue); // MSK may appear
    iDisplayable->SetMSKCommand(mskCommand);
}

void CMIDList::ShowInfoPopupL()
{
    if (iListBox
            && iDisplayable->IsActive()
            && iPopupController
            && iListBox->CurrentItemIndex() != -1
            && iListBox->CurrentItemIndex() < iItems.Count())
    {
        CColumnListBoxData* colListBoxData;
        CFormattedCellListBoxData* formattedCellListBoxData;

        TBool clipped(EFalse);
        switch (iListBoxType)
        {
        case ESingleStyle:
        case ESingleGraphicStyle:
        case ESingle2GraphicStyle:
        case ESingleLargeStyle:
            colListBoxData =
                (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData();
            clipped = colListBoxData->CurrentItemTextWasClipped();
            break;

        case EDouble2Style:
        case EDouble2GraphicStyle:
        case EDouble2GraphicLargeStyle:
        case EDouble2LargeStyle:
        case ESingleGraphicLargeStyle:
            formattedCellListBoxData=
                (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
            clipped = formattedCellListBoxData->CurrentItemTextWasClipped();
            break;
        default:
            User::Invariant();
        }

        iPopupController->HidePopup();
        DEBUG_INT("Clipped value: %d", clipped);
        if (clipped || iItems[ iListBox->CurrentItemIndex()]->GetPopupNeeded())
        {
            TBool dragged(EFalse);
#ifdef RD_SCALABLE_UI_V2
            dragged = iPointerDragHasStarted;
#endif // RD_SCALABLE_UI_V2
            TRAP_IGNORE(iPopupController->ShowPopupL(
                            iItems[ iListBox->CurrentItemIndex()]->Text(),
                            iListBox->HighlightRect(), dragged));
            iInfoPopupNoteIsShown = ETrue;
        }
    }
}

TBool CMIDList::ProcessCommandL(CMIDCommand* /*aCommand*/)
{
    // process internal command -> toggle selection in the list
    if (iListBox)
    {
        HandleListBoxEventL(iListBox, MEikListBoxObserver::EEventEnterKeyPressed);
    }
    return ETrue;
}

//
//  MMIDComponent interface methods
//

CBase* CMIDList::Ptr()
{
    return this;
}

void CMIDList::Dispose()
{
    delete this;
}


//
// CCoeControl methods
//

TInt CMIDList::CountComponentControls() const
{
    return 1;
}


CCoeControl* CMIDList::ComponentControl(TInt /*aIndex*/) const
{
    return iListBox;
}

/**
 * Handle a key event.
 * - If it is the ok key then call HandleListBoxEventL().
 * - Otherwise pass the key event to the listbox. See HandleListBoxEventL().
 **/
TKeyResponse CMIDList::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    if (aType == EEventKey)
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        if (aKeyEvent.iCode == EKeyEscape)
        {
            iPopupController->HidePopup();
            iInfoPopupNoteIsShown = EFalse;
            return EKeyWasConsumed;
        }
#else
        if (aKeyEvent.iCode == EKeyOK)
        {
            HandleListBoxEventL(iListBox, MEikListBoxObserver::EEventEnterKeyPressed);
            return EKeyWasConsumed;
        }
        else if (aKeyEvent.iCode == EKeyEscape)
        {
            iPopupController->HidePopup();
            iInfoPopupNoteIsShown = EFalse;
            return EKeyWasConsumed;
        }
#endif // RD_JAVA_S60_RELEASE_9_2
    }
    else
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
        return iListBox->OfferKeyEventL(aKeyEvent, aType);
#else
        return EKeyWasConsumed;
#endif // RD_JAVA_S60_RELEASE_9_2
    }

    TInt currentIndex = iListBox->View()->CurrentItemIndex();
    //if list is empty then open Context Sensitive Options after EnterKey pressing
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (iListBox->View()->TopItemIndex() == 0 && currentIndex == -1 &&
            (aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK))
#else
    if (iListBox->View()->TopItemIndex() == 0 && currentIndex == -1 && aKeyEvent.iScanCode == EStdKeyEnter)
#endif // RD_JAVA_S60_RELEASE_9_2        
    {
        iDisplayable->MenuHandler()->ShowMenuL(CMIDMenuHandler::EOkMenu);
        return EKeyWasConsumed;
    }

    TKeyResponse response = iListBox->OfferKeyEventL(aKeyEvent, aType);
    TInt newCurrent = iListBox->View()->CurrentItemIndex();

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Highlight is true when KeyDown,KeyUp, Enter or MSK was pressed and current
    // selected item changed (or current selected item is first element)
    if ((aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow ||
            aKeyEvent.iCode == EKeyEnter || aKeyEvent.iCode == EKeyOK) &&
            ((!currentIndex && currentIndex == newCurrent) ||
             currentIndex != newCurrent) &&
            iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        iHighlight = ETrue;
        if (iDisplayable)
        {
            iDisplayable->InitializeCbasL();
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    if (currentIndex != newCurrent)
    { // current item has changed
        UpdateCommands();
        ShowInfoPopupL();
    }
    return response;
}

#ifdef RD_SCALABLE_UI_V2
void CMIDList::HandlePointerEventL(const TPointerEvent &aPointerEvent)
{
    DEBUG_INT("CMIDList::HandlePointerEventL(): type=%d", aPointerEvent.iType);

    if (!AknLayoutUtils::PenEnabled() || !iListBox)
    {
        return;
    }

    TInt pointedItemIndex = KErrNotFound;
    TBool listItemFound = iListBox->View()->XYPosToItemIndex(
                              aPointerEvent.iPosition, pointedItemIndex);

    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
    {
        iPointerDragHasStarted = EFalse;
        iPopupController->HidePopup();
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        if (iFeedback && !listItemFound && iInfoPopupNoteIsShown)
        {
            //do feedback only when user tapped to empty space of List
            //(= when user doesn't tap to some item)
            iFeedback->InstantFeedback(ETouchFeedbackPopUp);
        }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
        iInfoPopupNoteIsShown = EFalse;
        iLongTapDetected = EFalse;
    }

    // Popup menu can be opened anywhere in empty list and in non-empty list
    // it is opened on top of items only. Need to send all drag and up events
    // to long tap detector so that it is cancelled properly when needed.
    if (listItemFound || iItems.Count() == 0 ||
            aPointerEvent.iType != TPointerEvent::EButton1Down)
    {
        // Forward pointer event to avkon list also in long tap case,
        // so that it can restore the list highlight when needed.
        iLongTapDetected = iDisplayable->TryDetectLongTapL(aPointerEvent);
    }

    TInt oldCurrent = iListBox->View()->CurrentItemIndex();

    TBool upEventOnListItem = aPointerEvent.iType == TPointerEvent::EButton1Up &&
                              listItemFound;

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    if (iListBox && upEventOnListItem &&
            iFeedback && iChoiceType == MMIDChoiceGroup::EMultiple)
    {
        //Because we dont use flags for multiselection lists, we have
        //to do specific feedback for checkbox by ourselves:
        //1. Disable feedback for native control.
        //   According to specification, this will disable also feedback
        //   invoked by overloaded function for instant feedback, so
        //   we can rely ont it
        iFeedback->EnableFeedbackForControl(iListBox, FALSE);
        //2. Do specific feedback for check/uncheck of checkbox
        //   (Other types of List have the same feedback as native control)
        iFeedback->InstantFeedback(ETouchFeedbackCheckbox);
        //3. After feedback we pass pointer event to native control
        //   (see below)
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#ifndef RD_JAVA_S60_RELEASE_9_2
    // iPanning may change after forwarding the event to Avkon list
    TBool panning = iPanning;
#endif // RD_JAVA_S60_RELEASE_9_2

#ifdef RD_JAVA_S60_RELEASE_9_2
    // If long tap occurs on top of list element, then pop-up menu is
    // opened and list element is kept highlighted. For that reason,
    // up event is not forwarded to listbox before the pop-up menu is closed.
    if (iLongTapDetected && upEventOnListItem)
    {
        iPendingUpEvent = aPointerEvent;
        iUpEventPending = ETrue;
    }
    else
    {
        CCoeControl::HandlePointerEventL(aPointerEvent);
    }
#else
    CCoeControl::HandlePointerEventL(aPointerEvent);
#endif // RD_JAVA_S60_RELEASE_9_2    

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    if (iListBox && iFeedback)
    {
        //4. Finally restore feedback for native control
        iFeedback->EnableFeedbackForControl(iListBox, TRUE);
    }
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

#ifdef RD_JAVA_S60_RELEASE_9_2
    // Highlight is false when there is tap but it is not a long tap
    if (iChoiceType == MMIDChoiceGroup::EImplicit && ((!iLongTapDetected &&
            aPointerEvent.iType == TPointerEvent::EButton1Down) ||
            (iLongTapDetected && aPointerEvent.iType == TPointerEvent::EButton1Up)))
    {
        iHighlight = EFalse;
        if (iDisplayable)
        {
            iDisplayable->InitializeCbasL();
        }
    }
#endif // RD_JAVA_S60_RELEASE_9_2

    if (iLongTapDetected)
    {
        return;
    }

    TBool currentChanged = iListBox->View()->CurrentItemIndex() != oldCurrent;

    if (currentChanged)
    {
        UpdateCommands();
        if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
            iPointerDragHasStarted = ETrue;
        }
    }
#ifndef RD_JAVA_S60_RELEASE_9_2
    if (!panning && (currentChanged || upEventOnListItem))
    {
        ShowInfoPopupL();
    }
#endif // RD_JAVA_S60_RELEASE_9_2
}

#endif // RD_SCALABLE_UI_V2

void CMIDList::Draw(const TRect& aRect) const
{
    CWindowGc& gc = SystemGc();
    gc.Clear(aRect);
}

/**
 * Handle the make visible event, this is called when our displayable becomes
 * active or inactive. If we become visible and we have a visual update request
 * then execute it immediately to prevent a double redraw. This is not only a
 * performance optimization, sometimes if the listbox must be recreated there
 * could be a big difference in the appearance of the first and second redraw.
 **/
void CMIDList::MakeVisible(TBool aVisible)
{
    CCoeControl::MakeVisible(aVisible);

    if (aVisible && iUpdateRequestTime != -1)
    {
        TRAP_IGNORE(UpdateOrRecreateListBoxL());
    }
    if (aVisible)
    {
        //Show infopopup after displayable change
    }
    else
    {
        iPopupController->HidePopup();
        iInfoPopupNoteIsShown = EFalse;
    }
}

/**
 * Handle a size changed event. Make sure list box and scroll bar are laid-out correctly.
 **/
void CMIDList::SizeChanged()
{
    if (iListBox)
    {
        iListBox->SetRect(Rect());
    }
}

//
// From MEikListBoxObserver methods
//

/**
 * This can be called either by the listbox itself or when we intercept an
 * ok key event. When the enter key is received, we post a java event for implicit
 * listboxes and we call SelectElementL() for other listboxes.
 **/
void CMIDList::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
{
#ifdef RD_SCALABLE_UI_V2
    if (iLongTapDetected)
    {
        return;
    }
#endif

    switch (aEventType)
    {
#ifdef RD_JAVA_S60_RELEASE_9_2
    case MEikListBoxObserver::EEventItemSingleClicked:
#else
    case MEikListBoxObserver::EEventItemDoubleClicked:
#endif // RD_JAVA_S60_RELEASE_9_2                        
    case MEikListBoxObserver::EEventEnterKeyPressed:
    case MEikListBoxObserver::EEventItemClicked:
        if (iChoiceType == MMIDChoiceGroup::EImplicit)
        {
#ifndef RD_JAVA_S60_RELEASE_9_2
            // no action for single click in Implicit lists
            if (aEventType == MEikListBoxObserver::EEventItemClicked)
            {
                return;
            }
#endif // RD_JAVA_S60_RELEASE_9_2

            if (iSelectCommandId == MMIDCommand::ENullCommand)
            { //setSelectCommand(null) has been called by application
#ifndef RD_JAVA_S60_RELEASE_9_2
                iDisplayable->ShowOkOptionsMenuL();
#else
                // If ShowOkOptionsMenuL returned false then we can try to open screen(help) menu
                if (iHighlight && iDisplayable && !iDisplayable->ShowOkOptionsMenuL() &&
                        aEventType == MEikListBoxObserver::EEventEnterKeyPressed)
                {
                    // Invoke the SCREEN or HELP command with the highest priority if exists
                    iDisplayable->HandleHighestPriorityScreenOrHelpCommandL();
                }
#endif // RD_JAVA_S60_RELEASE_9_2
            }
            else
            { //either default select cmd - which we may not know
                //if setSelectCommand() has not been called, in this case
                //iSelectCommandId will be -1 as initialised in the c.tor -
                //or application has set a cmd for which we have the ID
                iEnv.PostJavaEvent(*this, EDisplayable);
            }
        }
        else
        {
#ifndef RD_JAVA_S60_RELEASE_9_2
            // no action for double click in Exclusive/Multiple lists
            if (aEventType == MEikListBoxObserver::EEventItemDoubleClicked)
            {
                return;
            }
#endif // RD_JAVA_S60_RELEASE_9_2                
            TInt index = iListBox->CurrentItemIndex();
            if (index == KErrNotFound)
            {
                // the list must be empty
                break;
            }
            TBool selected = iListBox->View()->ItemIsSelected(index);
            if (iChoiceType == MMIDChoiceGroup::EMultiple)
            {
                // Sometimes EEventItemClicked is sent while panning. Ignore that.
                if (!iPanning)
                {
                    SelectElementL(index, !selected);
                }
            }
            else if (iChoiceType == MMIDChoiceGroup::EExclusive && !selected)
            {
                // Sometimes EEventItemClicked is sent while panning. Ignore that.
                if (!iPanning)
                {
                    SelectElementL(index, ETrue);
                }
            }
            // OkOptions menu is never shown when exclusive list is tapped with pointer
            else if (iChoiceType == MMIDChoiceGroup::EExclusive && selected &&
                     aEventType == MEikListBoxObserver::EEventEnterKeyPressed)
            {
                // current item is selected -> we can try to open the context sensitive menu.
                // If ShowOkOptionsMenuL returned false then we can try to open screen(help) menu
                if (!iDisplayable->ShowOkOptionsMenuL())
                {
                    // Invoke the SCREEN or HELP command with the highest priority if exists
                    iDisplayable->HandleHighestPriorityScreenOrHelpCommandL();
                }
            }

            if (iListBox->View()->ItemIsSelected(index) != selected)
            { // selection changed
                UpdateCommands();
#ifdef RD_JAVA_S60_RELEASE_9_2
                if (aEventType != MEikListBoxObserver::EEventItemSingleClicked)
                {
                    ShowInfoPopupL();
                }
#else
                ShowInfoPopupL();
#endif // RD_JAVA_S60_RELEASE_9_2                
            }
        }
        break;
#ifndef RD_JAVA_FOR_S60_5_0_w49_2008
#ifdef RD_SCALABLE_UI_V2
    case MEikListBoxObserver::EEventPanningStarted:
        // When panning is started, the list hightlight may change to another item
        // with delay (=highlight timer). Need to update commands based on this event
        // order that commands are set correctly when panning/flick ends.
        UpdateCommands();
        iPanning = ETrue;
        break;
    case MEikListBoxObserver::EEventPanningStopped:
        iPanning = EFalse;
        break;
#endif // RD_SCALABLE_UI_V2
#endif // RD_JAVA_FOR_S60_5_0_w49_2008

    default: //ignore
        break;
    }
}

CEikTextListBox* CMIDList::ListBox()
{
    return iListBox;
}

//
// Private Construction methods
//

CMIDList::CMIDList(MMIDEnv& aEnv, TInt aType)
        : iEnv(aEnv),
        iListBoxType(EUndefined),
        iIconArrayOwned(ETrue),
        iChoiceType(MMIDChoiceGroup::TChoiceType(aType)),
        iFitPolicy(MMIDChoiceGroup::EWrapDefault),
        iSelectCommandId(-1),
        iUpdateRequestTime(-1),
        iHighlightPosition(0),
        iBackCommandPosition(-1),
        iInfoPopupNoteIsShown(EFalse)
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    iHighlight = EFalse;
#endif // RD_JAVA_S60_RELEASE_9_2
}

/**
 * Second phase construction: construct some elements once and for all such as the
 * visual update timer and the icon array. Then create the items using the string and
 * image array. Call SetMaxIconSize() to determine the max image size. Finally
 * create the list box by setting iNeedToRecreateListBox to true and calling
 **/
void CMIDList::ConstructL(MMIDDisplayable& aDisplayable, RArray<TPtrC>& aStringArray,
                          RArray<MMIDImage*>& aImageArray)
{
    iVisualUpdateTimer = CPeriodic::NewL(CActive::EPriorityIdle);

    iDisplayable = static_cast<CMIDDisplayable*>(&aDisplayable);
    CCoeControl::SetContainerWindowL(*iDisplayable);
    SetRect(iDisplayable->Rect());

    CreateIconArrayL();
    CreateItemsL(aStringArray, aImageArray);

    SetMaxIconSize();

    iNeedToRecreateListBox = ETrue;
    UpdateOrRecreateListBoxL();

    iDisplayable->SetComponentL(*this);

    iPopupController = CMIDPopupNoteController::NewL();
    // Create built-in commands to make list selections
    HBufC* label = NULL;
    label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_SELECT);
    CleanupStack::PushL(label);
    iBuiltInSelectCommand = CMIDCommand::NewBuiltInCommandL(label->Des(),
                            MMIDCommand::EItem, CMIDCommand::EListSelectCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the label
    label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_MARK);
    CleanupStack::PushL(label);
    iMarkCommand = CMIDCommand::NewBuiltInCommandL(label->Des(), MMIDCommand::EItem,
                   CMIDCommand::EListMarkCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the label
    label = iEikonEnv->AllocReadResourceL(R_QTN_MSK_UNMARK);
    CleanupStack::PushL(label);
    iUnmarkCommand = CMIDCommand::NewBuiltInCommandL(label->Des(), MMIDCommand::EItem,
                     CMIDCommand::EListUnmarkCommandId);
    CleanupStack::PopAndDestroy(label); // command creates a copy of the label
    // set command observers
    iBuiltInSelectCommand->SetObserver(this);
    iMarkCommand->SetObserver(this);
    iUnmarkCommand->SetObserver(this);
    // initialize commands
    SetSelectCommand(MMIDCommand::EListSelectCommand);
    Window().SetPointerGrab(ETrue);

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    iFeedback = MTouchFeedback::Instance();
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK    
}

//
// Implementation methods
//

/**
 * Got through the icon array, iIconArray, and store the biggest icon size into
 * iMaxIconSize. Do not consider the first icons in the array, which are used to
 * display the check box for MULTIPLE listboxes or the select icon for EXCLUSIVE
 * listboxes.
 **/
void CMIDList::SetMaxIconSize()
{
    iMaxIconSize = TSize(0, 0);

    TInt firstIcon = KIconsExplicit; // blank icon + selection icons (on/off)
    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        firstIcon = KIconsImplicit; // blank icon
    }

    for (TInt i=firstIcon; i < iIconArray->Count(); i++)
    {
        if (iIconArray->At(i))
        {
            TSize size = iIconArray->At(i)->Bitmap()->SizeInPixels();
            if (size.iWidth > iMaxIconSize.iWidth)
            {
                iMaxIconSize.iWidth = size.iWidth;
            }
            if (size.iHeight > iMaxIconSize.iHeight)
            {
                iMaxIconSize.iHeight = size.iHeight;
            }
        }
    }
}




/**
 * Recreate the listbox. This is done only if the type has changed (due to a
 * different fit policy or a bigger max icon size), see ListBoxType(). If a
 * previous listbox existed, then transfer the icon array to the new listbox.
 * Then delete the old listbox and set iListBox to point to the new one. Also
 * take care of basic listbox methods, such as setting the listbox observer or
 * creatign the listbox scroll bar.
 *
 * Return true if a listbox has been created, false otherwise.
 *
 * @see iListBox, iListBoxType and ListBoxType()
 **/
TBool CMIDList::CreateListBoxL()
{
    TListBoxType type = ListBoxType();
    TBool listCreated = EFalse;

    if (iListBoxType != type)
    {
        CEikTextListBox* listBox = NULL;

        if (iChoiceType != MMIDChoiceGroup::EImplicit)
        {
            CreateSelectionIconsL();
        }

        switch (type)
        {
        case ESingleStyle:
            listBox = new(ELeave) CAknSingleStyleListBox;
            break;
        case ESingleGraphicStyle:
            listBox = new(ELeave) CAknSingleGraphicStyleListBox;
            break;
        case ESingle2GraphicStyle:
            listBox = new(ELeave) CAknSingle2GraphicStyleListBox;
            break;
        case ESingleLargeStyle:
            listBox = new(ELeave) CAknSingleLargeStyleListBox;
            break;
        case EDouble2Style:
            listBox = new(ELeave) CAknDoubleStyle2ListBox;
            break;
        case EDouble2GraphicStyle:
            listBox = new(ELeave) CAknDouble2GraphicStyleListBox;
            break;
        case ESingleGraphicLargeStyle:
        case EDouble2GraphicLargeStyle:
            listBox = new(ELeave) CAknDouble2GraphicLargeStyleListBox;
            break;
        case EDouble2LargeStyle:
            listBox = new(ELeave) CAknDouble2LargeStyleListBox;
            break;
        default:
            User::Invariant();
            return listCreated;
        }

        CleanupStack::PushL(listBox);

        listBox->SetContainerWindowL(*this);
#ifdef RD_JAVA_S60_RELEASE_9_2
        listBox->ConstructL(this, EAknListBoxSelectionList | CEikListBox::ENoExtendedSelection);
#else
        listBox->ConstructL(this, EAknListBoxSelectionList);
#endif //RD_JAVA_S60_RELEASE_9_2

        listBox->SetRect(Rect());

        listBox->CreateScrollBarFrameL(ETrue);
        listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                CEikScrollBarFrame::EAuto);

        listBox->SetListBoxObserver(this);
        listBox->ScrollBarFrame()->SetScrollBarFrameObserver(this);

        // We need avoid stretching list's elements. This prevents problems
        // with icon overlaying and bad wrapping.
        if (type < EDouble2Style)
        {
            CEikColumnListBox* clb =
                (static_cast<CEikColumnListBox*>(listBox));
            if (clb)
            {
                clb->EnableStretching(EFalse);
            }
        }
        else
        {
            CEikFormattedCellListBox* fclb =
                (static_cast<CEikFormattedCellListBox*>(listBox));
            if (fclb)
            {
                fclb->EnableStretching(EFalse);
            }
        }


        if (iListBox)
        {//Remove icon array before deleting old list box
            if (iListBoxType < EDouble2Style)
            {
                (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetIconArray(NULL);
            }
            else
            {
                (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetIconArray(NULL);
            }

            delete iListBox;
            iListBox = NULL;
        }

        listBox->ActivateL();

        iListBox = listBox;
        CleanupStack::Pop(listBox);

        iListBoxType = type;
        listCreated = ETrue;

        iIconArrayOwned = EFalse;
        if (type < EDouble2Style)
        {
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetIconArray(iIconArray);
        }
        else
        {
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetIconArray(iIconArray);
        }

        //Set listbox columns that can be underlined.
        TBitFlags32 underlineFlags; //Flags indicating listbox columns that can be underlined
        underlineFlags.ClearAll();

        switch (iListBoxType)
        {
        case ESingleStyle:
            underlineFlags.Set(KListTextColumn[ESingleStyle]);
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetColumnUnderlined(underlineFlags);
            break;
        case ESingleGraphicStyle:
            underlineFlags.Set(KListTextColumn[ESingleGraphicStyle]);
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetColumnUnderlined(underlineFlags);
            break;
        case ESingle2GraphicStyle:
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetGraphicSubCellL(
                0,AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine());
            underlineFlags.Set(KListTextColumn[ESingle2GraphicStyle]);
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetColumnUnderlined(underlineFlags);
            break;
        case ESingleLargeStyle:
            underlineFlags.Set(KListTextColumn[ESingleLargeStyle]);
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData()->SetColumnUnderlined(underlineFlags);
            break;
        case EDouble2Style:
            underlineFlags.Set(KListTextColumn[EDouble2Style]);
            underlineFlags.Set(KListTextColumn[EDouble2Style]+1);
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetSubcellUnderlined(underlineFlags);
            break;
        case EDouble2GraphicStyle:
            underlineFlags.Set(KListTextColumn[EDouble2GraphicStyle]);
            underlineFlags.Set(KListTextColumn[EDouble2GraphicStyle]+1);
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetSubcellUnderlined(underlineFlags);
            break;
        case ESingleGraphicLargeStyle:
            underlineFlags.Set(KListTextColumn[ESingleGraphicLargeStyle]);
            //Do not enable underlining to the second row of the element if iFitPolicy equals EWrapOff.
            //Enabling underlining would cause small line to the second row,
            //because the row consists of a 'space' character. (see function CMIDListItem::FormattedTextLC)
            if (iFitPolicy != MMIDChoiceGroup::EWrapOff)
                underlineFlags.Set(KListTextColumn[ESingleGraphicLargeStyle]+1);
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetSubcellUnderlined(underlineFlags);
            break;
        case EDouble2GraphicLargeStyle:
            underlineFlags.Set(KListTextColumn[EDouble2GraphicLargeStyle]);
            underlineFlags.Set(KListTextColumn[EDouble2GraphicLargeStyle]+1);
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetSubcellUnderlined(underlineFlags);
            break;
        case EDouble2LargeStyle:
            underlineFlags.Set(KListTextColumn[EDouble2LargeStyle]);
            underlineFlags.Set(KListTextColumn[EDouble2LargeStyle]+1);
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData()->SetSubcellUnderlined(underlineFlags);
            break;
        default:
            User::Invariant();

        }
    }

    return listCreated;
}




/**
 * Determine the avkon list box type depending on:
 *  @li The java listbox type (iChoiceType): IMPLICIT, EXCLUSIVE or MULTIPLE
 *  @li The max icon size: either bigger or smaller than KSmallIconMaxWidth
 *  @li The fit policy: text wrapping on or off.
 *
 *  This is according to the internal component specs.
 *
 *  @see CMIDList::TListBoxType
 **/
CMIDList::TListBoxType CMIDList::ListBoxType()
{
    // Only the icon size (not the coordinates) has to be read from LAF, so no need to
    // get LAF data hierarchically. Just pass the the rect of this class to LayoutRect method.
    // Icon size is the same for all lists, use list_single_graphic_pane here
    TAknLayoutRect iconRect;
    iconRect.LayoutRect(Rect(),
                        AknLayoutScalable_Avkon::list_single_graphic_pane_g1(0).LayoutLine());

    TSize smallIconMaxSize = iconRect.Rect().Size();

    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        if (iMaxIconSize.iWidth == 0 || iMaxIconSize.iHeight == 0)
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn)
            {
                return EDouble2Style;
            }
            else
            {
                return ESingleStyle;
            }
        }
        else if (iMaxIconSize.iWidth <= smallIconMaxSize.iWidth &&
                 iMaxIconSize.iHeight <= smallIconMaxSize.iHeight)
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn)
            {
                return EDouble2GraphicStyle;
            }
            else
            {
                return ESingleGraphicStyle;
            }
        }
        else
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn ||
                    iFitPolicy == MMIDChoiceGroup::EWrapDefault)
            {
                return EDouble2LargeStyle;
            }
            else
            {
                return ESingleLargeStyle;
            }
        }
    }
    else // exclusive or multiple list
    {
        if (iMaxIconSize.iWidth == 0 || iMaxIconSize.iHeight == 0)
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn)
            {
                return EDouble2GraphicStyle;
            }
            else
            {
                return ESingleGraphicStyle;
            }
        }
        else if (iMaxIconSize.iWidth <= smallIconMaxSize.iWidth &&
                 iMaxIconSize.iHeight <= smallIconMaxSize.iHeight)
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn)
            {
                return EDouble2GraphicLargeStyle;
            }
            else
            {
                return ESingle2GraphicStyle;
            }
        }
        else
        {
            if (iFitPolicy == MMIDChoiceGroup::EWrapOn ||
                    iFitPolicy == MMIDChoiceGroup::EWrapDefault)
            {
                return EDouble2GraphicLargeStyle;
            }
            else
            {
                return ESingleGraphicLargeStyle;
            }
        }
    }
}


/**
 * Create the icon array with some default icons for multiple and exclusive styles.
 **/
void CMIDList::CreateIconArrayL()
{
    iIconArray = new(ELeave) CArrayPtrFlat<CGulIcon>(8);

    // Create blank icon
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);

    CFbsBitmap* mask = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(mask);

    bitmap->Create(TSize(2,2), EColor256);
    mask->Create(TSize(2,2), EColor256);

    CGulIcon* blankIcon =CGulIcon::NewL(bitmap, mask);
    CleanupStack::PushL(blankIcon);

    iIconArray->AppendL(blankIcon);
    CleanupStack::Pop(3); // mask, bitmap, blankIcon

    if (iChoiceType != MMIDChoiceGroup::EImplicit)
    {
        // append two nulls to reserve space for selection graphics
        iIconArray->AppendL(NULL);
        iIconArray->AppendL(NULL);
    }
}
// ---------------------------------------------------------------------------
// CMIDList::CreateCheckOrRadioIconL
// ---------------------------------------------------------------------------
//
CGulIcon* CMIDList::CreateCheckOrRadioIconL(
    TBool aState, TBool aStyle) const
{


    TInt bmpId(0);
    TInt maskId(0);
    TAknsItemID skinItemId;

    if (aStyle)
    {
        if (aState)
        {
            skinItemId.Set(KAknsIIDQgnPropCheckboxOn);
            bmpId = EMbmAvkonQgn_prop_checkbox_on;
            maskId = EMbmAvkonQgn_prop_checkbox_on_mask;
        }
        else
        {
            skinItemId.Set(KAknsIIDQgnPropCheckboxOff);
            bmpId = EMbmAvkonQgn_prop_checkbox_off;
            maskId = EMbmAvkonQgn_prop_checkbox_off_mask;
        }
    }
    else
    {
        if (aState)
        {
            skinItemId.Set(KAknsIIDQgnPropRadiobuttOn);
            bmpId = EMbmAvkonQgn_prop_radiobutt_on;
            maskId = EMbmAvkonQgn_prop_radiobutt_on_mask;
        }
        else
        {
            skinItemId.Set(KAknsIIDQgnPropRadiobuttOff);
            bmpId = EMbmAvkonQgn_prop_radiobutt_off;
            maskId = EMbmAvkonQgn_prop_radiobutt_off_mask;
        }
    }

    return AknsUtils::CreateGulIconL(
               AknsUtils::SkinInstance(),
               skinItemId,
               AknIconUtils::AvkonIconFileName(),
               bmpId,
               maskId);
}

void CMIDList::CreateSelectionIconsL()
{
    // there must be space reserved for the selection icons when this function is called
    ASSERT(iIconArray->Length() >= 3);

    TFileName avkonbmpFilename = AknIconUtils::AvkonIconFileName();

    TParse* parseObj = new(ELeave) TParse();
    TInt errInt = parseObj->Set(avkonbmpFilename,&KDC_APP_BITMAP_DIR,NULL);
    if (KErrNone != errInt)
    {
        delete parseObj;
        User::Leave(errInt);
    }
    avkonbmpFilename = parseObj->FullName();
    delete parseObj;

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();


    // delete old selection icons, if they exist
    for (TInt i = KIconsNotSelected; i <= KIconsSelected; ++i)
    {
        if (iIconArray->At(i))
        {
            delete iIconArray->At(i);
            iIconArray->At(i) = NULL;
        }
    }

    CGulIcon* notSelectedIcon;
    if (iChoiceType == MMIDChoiceGroup::EMultiple)
    {
        notSelectedIcon = CreateCheckOrRadioIconL(EFalse, ETrue);
    }
    else
    {
        notSelectedIcon = CreateCheckOrRadioIconL(EFalse, EFalse);
    }
    iIconArray->At(KIconsNotSelected) = notSelectedIcon;

    CGulIcon* selectedIcon;
    if (iChoiceType == MMIDChoiceGroup::EMultiple)
    {
        selectedIcon = CreateCheckOrRadioIconL(ETrue, ETrue);
    }
    else
    {
        selectedIcon = CreateCheckOrRadioIconL(ETrue, EFalse);
    }
    iIconArray->At(KIconsSelected) = selectedIcon;

}

/**
 * Create and return a new icon containing a java image. Put it on
 * the cleanup stack only if NOT NULL.
 * @see MMIDBitmapIamge, CGulIcon
 **/
CGulIcon* CMIDList::CreateIconLC(MMIDImage* aImage)
{
    CGulIcon* icon = NULL;

    if (aImage)
    {
        MMIDBitmapImage* bitmapImage =  aImage->BitmapImage();

        CFbsBitmap* bitmap = CMIDUtils::CopyBitmapL(bitmapImage->ColorBitmap());
        CleanupStack::PushL(bitmap);

        CFbsBitmap* mask = NULL;
        if (bitmapImage->AlphaBitmap())
        {
            TDisplayMode alphaDisplayMode = bitmapImage->AlphaBitmap()->DisplayMode();
            TBool invert = alphaDisplayMode == EGray2 ? ETrue : EFalse;
            mask = bitmapImage->CreateAlphaBitmapL(alphaDisplayMode, invert);
        }

        CleanupStack::PushL(mask);

        icon = CGulIcon::NewL(bitmap, mask);
        icon->SetBitmapsOwnedExternally(EFalse);
        CleanupStack::Pop(2); // mask, bitmap
        CleanupStack::PushL(icon);
        bitmapImage->RemoveRef();
    }

    return icon;
}

/**
 * Create a new icon and insert it into the icon array. Return the index in
 * the icon array containing the new icon. The icon array must have already been created.
 * @see CreateIconLC()
 **/
TInt CMIDList::InsertNewIconL(MMIDImage* aImage)
{
    ASSERT(iIconArray);
    TInt iconIndex = KBlankIconIndex;

    CGulIcon* icon = CreateIconLC(aImage);
    if (icon)
    {
        TInt numIcons = iIconArray->Count();
        // skip the blank icon and possible selection graphic icons
        TInt i = (iChoiceType == MMIDChoiceGroup::EImplicit) ? KIconsImplicit : KIconsExplicit;
        for (; i < numIcons; i++)
        {//if there is an existing gap due to a previously deleted icon,
            // then use the gap - see DoDeleteElement()
            if (iIconArray->At(i) == NULL)
            {
                iIconArray->At(i) = icon;
                iconIndex = i;
                break;
            }
        }

        if (iconIndex == KBlankIconIndex)
        {//if no gap found then append the icon at the end of the array
            iIconArray->AppendL(icon);
            iconIndex = iIconArray->Count() - 1;
        }

        CleanupStack::Pop(icon);
    }
    return iconIndex;
}

/**
 * Create the array of items used to populate the list box. The icon array
 * must have already been created.
 **/
void CMIDList::CreateItemsL(RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray)
{
    ASSERT(iIconArray);
    ASSERT(iItems.Count() == 0);

    TInt numItems = aStringArray.Count();
    for (TInt i = 0; i < numItems; i++)
    {
        CMIDListItem* item = CMIDListItem::NewLC(aStringArray[i], InsertNewIconL(aImageArray[i]));
        User::LeaveIfError(iItems.Append(item));
        CleanupStack::Pop(item);
    }
}

/**
 * Populate the listbox. Listbox and items must have already been created.
 * For each item insert the formatted text into the listbox. @see CMIDListBoxItem and
 * CMIDListBoxItem::FormattedTextLC()
 **/
void CMIDList::PopulateListBoxL()
{
    ASSERT(iListBox);
    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray());

    TInt numItems = iItems.Count();

    for (TInt i=0; i < numItems; i++)
    {
        HBufC* text = iItems[i]->FormattedTextLC(iListBoxType, iChoiceType);
        itemTextArray->AppendL(*text);
        CleanupStack::PopAndDestroy(text);
    }
}

/** Update an element in the current list box to reflect its selection status */
void CMIDList::SetSelectionIntoListBoxL(TInt aIndex,TBool aSelected)
{
    CDesCArray* itemTextArray = static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray());

    if (iChoiceType == MMIDChoiceGroup::EMultiple)
    {
        SetSelectionIconL(iListBox, aIndex, aSelected);
        if (aSelected)
        {
            iListBox->View()->SelectItemL(aIndex);
        }
        else
        {
            iListBox->View()->DeselectItem(aIndex);
        }
    }
    else if (aSelected)
    {
        if (iChoiceType == MMIDChoiceGroup::EImplicit)
        {
            iListBox->SetCurrentItemIndex(aIndex);
        }
        else if (iChoiceType == MMIDChoiceGroup::EExclusive)
        {
            for (TInt i=0; i < itemTextArray->Count(); i++)
            {
                SetSelectionIconL(iListBox, i, EFalse);
            }
            iListBox->View()->ClearSelection();
            SetSelectionIconL(iListBox, aIndex, ETrue);
            iListBox->View()->SelectItemL(aIndex);
        }
    }

    DrawDeferred();
}

/** Change the icon index for an item of a non-implicit list box. */
void CMIDList::SetSelectionIconL(CEikTextListBox* aListBox, TInt aIndex, TBool aSelected)
{
    if (iChoiceType == MMIDChoiceGroup::EImplicit)
    {
        return;
    }

    CDesCArray* itemTextArray = static_cast<CDesCArray*>(aListBox->Model()->ItemTextArray());

    TPtrC ptrC = (*itemTextArray)[aIndex];
    HBufC* buf = ptrC.AllocLC();

    TPtr ptr = buf->Des();
    ptr[0] = aSelected ? KSelected : KUnselected;

    itemTextArray->Delete(aIndex);
    itemTextArray->InsertL(aIndex, *buf);
    CleanupStack::PopAndDestroy(buf);
}

/**
 * Create a new item and store it in iItems at the correct index position. See
 * if it has an image and if this image is bigger than existing images. This may
 * require recreating the listbox (needToRecreateListBox) as indicated in the internal
 * specs. If this is the case recreate and repopulate the list. Otherwise just add
 * the formatted text of the new item to the list box and call HandleItemAdditionL().
 **/
void CMIDList::DoInsertElementL(TInt aIndex, const TDesC& aText, MMIDImage* aImage)
{
    CMIDListItem* item = CMIDListItem::NewLC(aText, InsertNewIconL(aImage));
    User::LeaveIfError(iItems.Insert(item, aIndex));
    CleanupStack::Pop(item);

    HBufC* text = item->FormattedTextLC(iListBoxType, iChoiceType);
    static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray())->InsertL(aIndex, *text);
    CleanupStack::PopAndDestroy(text);

    if (aImage)
    {
        TSize size = aImage->Size();
        if (size.iWidth > iMaxIconSize.iWidth)
        {
            iMaxIconSize.iWidth = size.iWidth;
            iNeedToRecreateListBox = ETrue;
        }
        if (size.iHeight > iMaxIconSize.iHeight)
        {
            iMaxIconSize.iHeight = size.iHeight;
            iNeedToRecreateListBox = ETrue;
        }
    }
}

/**
 * Remove an element. If the element has an image then see if without that image
 * the max image size has changed and the list box need to be re-created as specified
 * in the internal specs. Then remove the element from iItems and either recreate the list
 * box or remove the item text form the existing list box and call HandleItemRemovalL().
 **/
void CMIDList::DoDeleteElement(TInt aIndex)
{
    TInt iconIndex = iItems[aIndex]->IconIndex();
    if (iconIndex != KBlankIconIndex)
    {
        delete(iIconArray->At(iconIndex));
        iIconArray->At(iconIndex) = NULL;

        SetMaxIconSize();
        iNeedToRecreateListBox = ETrue;
    }

    delete iItems[aIndex];
    iItems.Remove(aIndex);

    static_cast<CDesCArray*>(iListBox->Model()->ItemTextArray())->Delete(aIndex);
}

/** Ask the current list box to use a custom font for element at position aIndex */
void CMIDList::UpdateFontToElementL(TInt aIndex)
{
    ASSERT(aIndex >= 0 && aIndex < iItems.Count());

    CFont* font = iItems[aIndex]->Font() ? iItems[aIndex]->Font()->Font(ETrue) : NULL;

    CColumnListBoxData* ColumnListBoxData;
    CFormattedCellListBoxData* FormattedCellListBoxData;

    switch (iListBoxType)
    {
    case ESingleStyle:
        ColumnListBoxData =
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData();
        ColumnListBoxData->SetColumnFontForRowL(aIndex, KListTextColumn[ESingleStyle], font);
        break;

    case ESingleGraphicStyle:
        ColumnListBoxData =
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData();
        ColumnListBoxData->SetColumnFontForRowL(aIndex,
                                                KListTextColumn[ESingleGraphicStyle], font);
        break;

    case ESingle2GraphicStyle:
        ColumnListBoxData =
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData();
        ColumnListBoxData->SetColumnFontForRowL(aIndex,
                                                KListTextColumn[ESingle2GraphicStyle], font);
        break;

    case ESingleLargeStyle:
        ColumnListBoxData =
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->ColumnData();
        ColumnListBoxData->SetColumnFontForRowL(aIndex, KListTextColumn[ESingleLargeStyle], font);
        break;

    case EDouble2Style:
        FormattedCellListBoxData =
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2Style], font);
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2Style]+1, font);
        break;

    case EDouble2GraphicStyle:
        FormattedCellListBoxData =
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2GraphicStyle], font);
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2GraphicStyle]+1, font);
        break;

    case EDouble2GraphicLargeStyle:
        FormattedCellListBoxData =
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2GraphicLargeStyle], font);
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2GraphicLargeStyle]+1, font);
        break;

    case EDouble2LargeStyle:
        FormattedCellListBoxData =
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2LargeStyle], font);
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[EDouble2LargeStyle]+1, font);
        break;

    case ESingleGraphicLargeStyle:
        FormattedCellListBoxData=
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->FormattedCellData();
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[ESingleGraphicLargeStyle], font);
        FormattedCellListBoxData->SetSubCellFontForRowL(aIndex,
                KListTextColumn[ESingleGraphicLargeStyle]+1, font);
        break;
    default:
        User::Invariant();
        return;
    }


    //Set or remove underline
    TBool isUnderlined = iItems[aIndex]->Font() ? iItems[aIndex]->Font()->IsUnderlined() : EFalse;

    if (iListBoxType < EDouble2Style)
    {
        TListItemProperties ItemProperties =
            (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->Properties(aIndex);

        ItemProperties.SetUnderlined(isUnderlined);
        (static_cast<CEikColumnListBox*>(iListBox))->ItemDrawer()->SetPropertiesL(aIndex,ItemProperties);
    }
    else
    {
        TListItemProperties ItemProperties =
            (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->Properties(aIndex);

        ItemProperties.SetUnderlined(isUnderlined);
        (static_cast<CEikFormattedCellListBox*>(iListBox))->ItemDrawer()->SetPropertiesL(aIndex,ItemProperties);
    }
}


void CMIDList::HandleResourceChange(TInt aType)
{
    CCoeControl::HandleResourceChange(aType);

    if (aType == KAknsMessageSkinChange ||
            aType == KUidValueCoeColorSchemeChangeEvent ||
            aType == KEikColorResourceChange)
    {
        if (iChoiceType != MMIDChoiceGroup::EImplicit)
        {
            TRAP_IGNORE(CreateSelectionIconsL());
            TRAP_IGNORE(UpdateOrRecreateListBoxL());
        }
    }

#ifndef RD_JAVA_S60_RELEASE_9_2
    //Show infopopup after resolution change
    TRAP_IGNORE(ShowInfoPopupL());
#endif // RD_JAVA_S60_RELEASE_9_2
}

// Responds to focus change.
// From CCoeControl.
void CMIDList::FocusChanged(TDrawNow aDrawNow)
{
    // call default CCoeControl's implementation
    CCoeControl::FocusChanged(aDrawNow);

    // update focus state of listbox regarding to current focus state
    if (iListBox)
    {
        iListBox->SetFocus(IsFocused(), aDrawNow);
    }
}

void CMIDList::ProcessCommandL(TInt /*aCommandId*/)
{
}

void CMIDList::HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType)
{
    iListBox->HandleScrollEventL(aScrollBar, aEventType); // we have replaced the default handler with our own implementation, call it first
    iPopupController->HidePopup(); // popup info dialog is hidden immediately once user taps on scrollbar
    iInfoPopupNoteIsShown = EFalse;
}

//  End of File
