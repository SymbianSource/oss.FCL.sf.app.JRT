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
* Description:  CAppMngr2MidletSettingsContainer implementation
*
*/


// INCLUDE FILES
#include <aknlists.h>                           // CAknSettingStyleListBox
#include "javaapplicationsettings.hlp.hrh"

#include "appmngr2midletsettingscontainer.h"    // CAppMngr2MidletSettingsContainer
#include "appmngr2midletsettingsview.h"         // CAppMngr2MidletSettingsView
#include "javaapplicationsettings.hrh"                   // Midlet command IDs

#include "logger.h"                          // LOG

// LOCAL CONSTANTS AND MACROS
// UID of the application
const TUid KOwnUid = { 0x20016BF7 };  // Own Uid, used with help

using namespace std;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
CAppMngr2MidletSettingsContainer::CAppMngr2MidletSettingsContainer(
    CAppMngr2MidletSettingsView& aView) : iView(aView), iListbox(NULL)
{
    LOG(EJavaAppMngrPlugin, EInfo, "CAppMngr2MidletSettingsContainer::CAppMngr2MidletSettingsContainer");
    iListBoxContent.clear();
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsContainer::ConstructL(const TRect& aRect)
// Symbian OS two phased constructor
// Creates new window and listbox
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::ConstructL(const TRect& aRect)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::ConstructL ");
    CreateWindowL();

    iListbox = new(ELeave) CAknSettingStyleListBox;

    iListbox->SetContainerWindowL(*this);
    iListbox->ConstructL(this,EAknListBoxSelectionList | EAknListBoxItemSpecificMenuDisabled);

    // set up the listbox
    iListbox->CreateScrollBarFrameL(ETrue);
    iListbox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
            CEikScrollBarFrame::EAuto);

    iListbox->SetRect(aRect.Size());
    iListbox->ActivateL();

    SetRect(aRect);
    ActivateL();

    iListbox->SetCurrentItemIndex(0);

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::ConstructL ");
}

// Destructor
CAppMngr2MidletSettingsContainer::~CAppMngr2MidletSettingsContainer()
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::~CAppMngr2MidletSettingsContainer ");
    if (iListbox)
    {
        iListbox->Reset();
        delete iListbox;
    }
    iListBoxContent.clear();
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::~CAppMngr2MidletSettingsContainer ");
}


// ---------------------------------------------------------------------------
//  CAppMngr2MidletSettingsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
//  This handles the scrolling the list by arrows and the clear key and OK key
// ---------------------------------------------------------------------------
//
TKeyResponse CAppMngr2MidletSettingsContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::OfferKeyEventL ");
    TKeyResponse retval = EKeyWasNotConsumed;

    switch (aKeyEvent.iCode)
    {
    case EKeyOK:
        // This key event is handeled by main view as a command.
        iView.HandleCommandL(EAknSoftkeyChange);
        break;

    case EKeyUpArrow:   // fallthrough
    case EKeyDownArrow:
    default:
        retval = iListbox->OfferKeyEventL(aKeyEvent, aType);
        break;
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::OfferKeyEventL ");
    return retval;
}


// ---------------------------------------------------------
// CAppMngr2MidletSettingsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::SizeChanged()
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsContainer::SizeChanged ");
    iListbox->SetRect(Rect());
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CAppMngr2MidletSettingsContainer::CountComponentControls() const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsContainer::CountComponentControls ");
    return iListbox ? 1 : 0;
}

// ---------------------------------------------------------
// CAppMngr2MidletSettingsContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CAppMngr2MidletSettingsContainer::ComponentControl(TInt aIndex) const
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsContainer::ComponentControl ");
    switch (aIndex)
    {
    case 0:
        return iListbox;
    default:
        return NULL;
    }
}

void CAppMngr2MidletSettingsContainer::RefreshListBoxContentL(int aListBoxContentIndex, int aSelectedItemIndex, bool aAllEnabled)
{
    // refresh the rest
    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    // refresh the current item
    RefreshListBoxContentL(aListBoxContentIndex, aSelectedItemIndex);

    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = static_cast<CDesCArray*>(itemList);

    HBufC* itemValue = HBufC::NewLC(KMaxDataTypeLength);
    TPtr itemPtr = itemValue->Des();

    for (int i=(aListBoxContentIndex+1); i<iListBoxContent.size(); i++)
    {
        bool wasEnabled = iListBoxContent[i].getEnabled();
        iListBoxContent[i].setEnabled(aAllEnabled);
        if (aAllEnabled)
        {
            if (wasEnabled)
            {
                continue;
            }
            // add it to the list
            // add name
            itemPtr.Zero();
            itemPtr += _L("\t");
            wstring name = iListBoxContent[i].getName().getValue();
            TPtr ptr((unsigned short*)name.c_str(), name.size());
            ptr.SetLength(name.size());
            itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
            itemPtr += _L("\t");
            itemPtr += _L("\t");
            // add value
            wstring value = iListBoxContent[i].getValue().getValue();
            ptr.Set((unsigned short*)value.c_str(), value.size(), value.size());
            ptr.SetLength(value.size());
            itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
            itemArray->AppendL(itemPtr);
        }
        else
        {
            itemArray->Delete(lastListBoxPos + 1);
        }
    }

    CleanupStack::PopAndDestroy(itemValue);

    // Handle content changes
    iListbox->Reset();
    // set indexes
    if (lastListBoxPos != -1)
    {
        iListbox->SetCurrentItemIndex(lastListBoxPos);
        iListbox->SetTopItemIndex(lastListBoxTopPos);
    }
    // Draw it
    iListbox->DrawDeferred();
}
void CAppMngr2MidletSettingsContainer::RefreshListBoxContentL(int aListBoxContentIndex, int aSelectedItemIndex, const ListItem& aListItem)
{
    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = static_cast<CDesCArray*>(itemList);

    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    HBufC* itemValue = HBufC::NewLC(KMaxDataTypeLength);
    TPtr itemPtr = itemValue->Des();
    // add name
    iListBoxContent[aListBoxContentIndex] = aListItem;
    wstring name = aListItem.getName().getValue();
    itemPtr.Zero();
    itemPtr += _L("\t");
    TPtr ptr((unsigned short*)name.c_str(), name.size());
    ptr.SetLength(name.size());
    itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
    itemPtr += _L("\t");
    itemPtr += _L("\t");
    // add value
    wstring value = aListItem.getValue(aSelectedItemIndex).getValue();
    ptr.Set((unsigned short*)value.c_str(), value.size(), value.size());
    ptr.SetLength(value.size());
    itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
    itemArray->Delete(lastListBoxPos);
    itemArray->InsertL(lastListBoxPos, itemPtr);

    CleanupStack::PopAndDestroy(itemValue);

    // Handle content changes
    iListbox->Reset();
    // set indexes
    if (lastListBoxPos != -1)
    {
        iListbox->SetCurrentItemIndex(lastListBoxPos);
        iListbox->SetTopItemIndex(lastListBoxTopPos);
    }
    // Draw it
    iListbox->DrawDeferred();
}

void CAppMngr2MidletSettingsContainer::RefreshListBoxContentL(int aListBoxContentIndex ,int aSelectedItemIndex)
{
    RefreshListBoxContentL(aListBoxContentIndex ,aSelectedItemIndex, iListBoxContent[aListBoxContentIndex]);
}

void CAppMngr2MidletSettingsContainer::RefreshListBoxContentL(const ListItem& aOldItem, const ListItem& aNewItem)
{
    for (int i=0; i<iListBoxContent.size(); i++)
    {
        if (iListBoxContent[i].getName().getId() == aOldItem.getName().getId())
        {
            iListBoxContent[i] = aNewItem;

            MDesCArray* itemList = iListbox->Model()->ItemTextArray();
            CDesCArray* itemArray = static_cast<CDesCArray*>(itemList);

            int count = itemArray->Count();

            TPtr newValue((unsigned short*)aOldItem.getName().getValue().c_str(), aOldItem.getName().getValue().size());
            newValue.SetLength(aOldItem.getName().getValue().size());

            for (int i=0; i<count; i++)
            {
                TPtrC oldValue = (*itemArray)[i];
                if (oldValue.Find(newValue) > 0)
                {
                    TInt lastListBoxPos = iListbox->CurrentItemIndex();
                    TInt lastListBoxTopPos = iListbox->TopItemIndex();

                    HBufC* itemValue = HBufC::NewLC(KMaxDataTypeLength);
                    TPtr itemPtr = itemValue->Des();
                    // add name
                    wstring name = aNewItem.getName().getValue();
                    itemPtr.Zero();
                    itemPtr += _L("\t");
                    TPtr ptr((unsigned short*)name.c_str(), name.size());
                    ptr.SetLength(name.size());
                    itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
                    itemPtr += _L("\t");
                    itemPtr += _L("\t");
                    // add value
                    wstring value = aNewItem.getValue().getValue();
                    ptr.Set((unsigned short*)value.c_str(), value.size(), value.size());
                    ptr.SetLength(value.size());
                    itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);

                    itemArray->Delete(i);
                    itemArray->InsertL(i, itemPtr);

                    CleanupStack::PopAndDestroy(itemValue);

                    // Handle content changes
                    iListbox->Reset();
                    // set indexes
                    if (lastListBoxPos != -1)
                    {
                        iListbox->SetCurrentItemIndex(lastListBoxPos);
                        iListbox->SetTopItemIndex(lastListBoxTopPos);
                    }
                    // Draw it
                    iListbox->DrawDeferred();

                    break;
                }
            }

            break;
        }
    }
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsContainer::UpdateListBoxContentL
//
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::InitListBoxContentL(const vector<ListItem>& aListItems)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::UpdateListBoxContentL ");
    iListBoxContent = aListItems;

    TInt lastListBoxPos = iListbox->CurrentItemIndex();
    TInt lastListBoxTopPos = iListbox->TopItemIndex();

    MDesCArray* itemList = iListbox->Model()->ItemTextArray();
    CDesCArray* itemArray = static_cast<CDesCArray*>(itemList);
    itemArray->Reset();

    HBufC* itemValue = HBufC::NewLC(KMaxDataTypeLength);
    TPtr itemPtr = itemValue->Des();

    for (int i=0; i<aListItems.size(); i++)
    {
        if (!aListItems[i].getEnabled())
        {
            continue;
        }
        // add name
        itemPtr.Zero();
        itemPtr += _L("\t");
        wstring name = aListItems[i].getName().getValue();
        TPtr ptr((unsigned short*)name.c_str(), name.size());
        ptr.SetLength(name.size());
        itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
        itemPtr += _L("\t");
        itemPtr += _L("\t");
        // add value
        wstring value = aListItems[i].getValue().getValue();
        ptr.Set((unsigned short*)value.c_str(), value.size(), value.size());
        ptr.SetLength(value.size());
        itemPtr += AknTextUtils::ChooseScalableText(ptr,*iCoeEnv->NormalFont(), NULL);
        itemArray->AppendL(itemPtr);
    }

    CleanupStack::PopAndDestroy(itemValue);

    // Handle content changes
    iListbox->Reset();
    // set indexes
    if (lastListBoxPos != -1)
    {
        iListbox->SetCurrentItemIndex(lastListBoxPos);
        iListbox->SetTopItemIndex(lastListBoxTopPos);
    }
    // Draw it
    iListbox->DrawDeferred();

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::UpdateListBoxContentL ");
}

const ListItem& CAppMngr2MidletSettingsContainer::GetListBoxItem(int aListBoxItemIndex)
{
    return iListBoxContent[aListBoxItemIndex];
}

void CAppMngr2MidletSettingsContainer::SetListBoxItemValue(int aListBoxItemIndex, int aValue)
{
    iListBoxContent[aListBoxItemIndex].setCurrentValue(aValue);
}

// ---------------------------------------------------------------------------
// CAppMngr2MidletSettingsContainer::GetHelpContext(TCoeHelpContext& aContext)
// Gives the help context to be displayed
// ---------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::GetHelpContext(TCoeHelpContext& aContext) const
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::GetHelpContext ");
    aContext.iMajor = KOwnUid;
    aContext.iContext = HLP_JAVA_APPLICATION_SETTINGS;
    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::GetHelpContext ");
}


// ---------------------------------------------------------
// CAppMngr2MidletSettingsContainer::HandleResourceChange
// Handles a resource relative event
// ---------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::HandleResourceChange(TInt aType)
{
    LOG(EJavaAppMngrPlugin, EInfo, " + CAppMngr2MidletSettingsContainer::HandleResourceChange ");
    CCoeControl::HandleResourceChange(aType);

    if (aType == KEikDynamicLayoutVariantSwitch) //Handle change in layout orientation
    {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainPaneRect);
        SetRect(mainPaneRect);
        DrawNow();
    }

    LOG(EJavaAppMngrPlugin, EInfo, " - CAppMngr2MidletSettingsContainer::HandleResourceChange ");
}


// -----------------------------------------------------------------------------
// CAppMngr2MidletSettingsContainer::FocusChanged
// -----------------------------------------------------------------------------
//
void CAppMngr2MidletSettingsContainer::FocusChanged(TDrawNow aDrawNow)
{
    LOG(EJavaAppMngrPlugin, EInfo, " CAppMngr2MidletSettingsContainer::FocusChanged ");
    if (iListbox)
    {
        iListbox->SetFocus(IsFocused(), aDrawNow);
    }
}

// End of File
