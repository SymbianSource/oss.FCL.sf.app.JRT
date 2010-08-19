/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include <akntabgrp.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <eikappui.h>
#include <aknappui.h>
#include <AknsControlContext.h>
#include <swtlaffacade.h>
#include "swtcontrolhelper.h"
#include "swtmultipagedialog.h"


// ======== MEMBER FUNCTIONS ========


/**
 * Destructor
 */
CSwtMultiPageDialog::~CSwtMultiPageDialog()
{
    Close();
    iTabItems.Reset();
    iTabItems.Close();
    delete iNaviDecorator;
    delete iRedirector;
}

/**
 * 1st and 2nd phase constructor wrapper.
 */
CSwtMultiPageDialog* CSwtMultiPageDialog::NewL(MSwtDisplay& aDisplay,
        TSwtPeer aPeer, MSwtShell& aParent)
{
    CSwtMultiPageDialog * self = new(ELeave) CSwtMultiPageDialog(
        aDisplay, aPeer, aParent);
    CleanupStack::PushL(self);

    self->ConstructL();
    self->InitControlBaseL();

    CleanupStack::Pop(self);
    return self;
}

/**
 * 1st phase constructor.
 */
CSwtMultiPageDialog::CSwtMultiPageDialog(MSwtDisplay& aDisplay,
        TSwtPeer aPeer, MSwtShell& aParent)
        : ASwtControlBase(aDisplay, aPeer, aParent.Composite(), KSwtNone)
        , iVisibleTab(-1)
        , iTitleCount(0)
{}

/**
 * 2nd phase constructor.
 */
void CSwtMultiPageDialog::ConstructL()
{
    // We create the TabGroup here
    CEikStatusPane * sp = ((CAknAppUi*) iEikonEnv->EikAppUi()) ->StatusPane();
    ASSERT(sp);
    iNaviCtrlCtnr = static_cast<CAknNavigationControlContainer *>(
                        sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    ASSERT(iNaviCtrlCtnr);
    iNaviDecorator = iNaviCtrlCtnr->CreateTabGroupL();
    ASSERT(iNaviDecorator);
    iTabGroup = static_cast<CAknTabGroup*>(iNaviDecorator->DecoratedControl());
    ASSERT(iTabGroup);
    iTabGroup->SetObserver(this);

    // create edit redirection flag
    iRedirector = new(ELeave) CSwtEditIndicatorRedictor();
    // We initialize the control that will receive the events
    CCoeControl& coeParent = GetParent() ->Control() ->CoeControl();
    SetContainerWindowL(coeParent);

    CCoeControl::MakeVisible(coeParent.IsVisible());
    CCoeControl::SetDimmed(coeParent.IsDimmed());
    CCoeControl::SetFocusing(EFalse);
    SetComponentsToInheritVisibility(ETrue);
    ActivateL();
}

/**
 * Calcuates the size of the tabs
 */
void CSwtMultiPageDialog::SizeChanged()
{
    HandleSizeChanged();
}


//
// Virtual methods from MSwtControl
//
TRect CSwtMultiPageDialog::ClientRect() const
{
    return Rect();
}

CCoeControl& CSwtMultiPageDialog::CoeControl()
{
    return *this;
}

const CCoeControl& CSwtMultiPageDialog::CoeControl() const
{
    return *this;
}

TSize CSwtMultiPageDialog::ComputeSizeL(TInt /*aWHint*/, TInt /*aHHint*/)
{
    return MinimumSize();
}

TInt CSwtMultiPageDialog::GetBorderWidth() const
{
    return 0;
}


void CSwtMultiPageDialog::ProcessKeyEventL(const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/)
{}

TTypeUid::Ptr CSwtMultiPageDialog::MopSupplyObject(TTypeUid aId)
{
    if (iRedirector && aId.iUid == CSwtEditIndicatorRedictor::ETypeId)
    {
        return aId.MakePtr(iRedirector);
    }

    if (aId.iUid == MAknsControlContext::ETypeId)
    {
        return MAknsControlContext::SupplyMopObject(aId,
                GetParent()->Control()->GetControlContext());
    }

    return CCoeControl::MopSupplyObject(aId);
}

//
// Virtual methods from MSwtMultiPageDialog
//

MSwtControl* CSwtMultiPageDialog::Control()
{
    return this;
}

void CSwtMultiPageDialog::Close()
{
    ASSERT(iNaviCtrlCtnr);
    if (iIsOpen)
    {
        iIsOpen = EFalse;
        iNaviCtrlCtnr->Pop();
        ((CAknAppUi*) iEikonEnv->EikAppUi()) ->StatusPane() ->DrawNow();
    }
}

void CSwtMultiPageDialog::CreatePageL(MSwtComposite* aComposite,
                                      const TDesC& aTitle, MSwtImage* aImage)
{
    ASSERT(aComposite);

    //Lets change the MOP Parent so that we can serve children with objects
    aComposite->Control()->CoeControl().SetMopParent(this);
    aComposite->Control()->CoeControl().MakeVisible(EFalse);

    TInt newId = 0;
    if (iTabItems.Count() > 0)
    {
        newId = iTabItems[ iTabItems.Count() - 1 ].iTabId + 1;
    }

    if (aImage)
    {
        TAknLayoutText lrect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::ETabs4ActivePaneT1, iTabGroup->Rect());
        TSize size = SwtControlHelper::GetAspectRatioScaledBitmapSize(aImage->GetBounds().Size(), lrect.TextRect().Size());
        CFbsBitmap* bmp =  SwtControlHelper::GetCopyOfBitmapLC(&aImage->Bitmap(), size);
        CFbsBitmap* mask = NULL;
        iTabGroup->AddTabL(newId, bmp , mask);// Bitmap ownership transfered
        iTabGroup->SetTabMultiColorMode(newId, ETrue);
        CleanupStack::Pop(bmp);
    }
    else
    {
        iTabGroup->AddTabL(newId, aTitle);
    }

    iTitleCount++;

    TTabItem tabItem;
    tabItem.iComposite = aComposite;
    tabItem.iTabId = newId;
    tabItem.iHasTitle = aTitle.Length() > 0 ? ETrue : EFalse;
    iTabItems.AppendL(tabItem);

    iTabGroup->SetTabFixedWidthL(ComputeTabStyle());
    RefreshStatusPane();
}

void CSwtMultiPageDialog::DeletePageL(TInt aIndex)
{
    ASSERT(aIndex > -1);
    ASSERT(aIndex < iTabItems.Count());
    iTabGroup->DeleteTabL(iTabItems[aIndex].iTabId);
    if (iTabItems[aIndex].iHasTitle)
    {
        iTitleCount--;
    }
    iTabItems.Remove(aIndex);
    iTabGroup->SetTabFixedWidthL(ComputeTabStyle());
    RefreshStatusPane();
    // in case the ActivePage has changed
    if (aIndex == iVisibleTab)
    {
        iVisibleTab = -1;
        if (iTabItems.Count() > 0)
        {
            ShowTab(iTabGroup->ActiveTabIndex());
        }
    }
}

TInt CSwtMultiPageDialog::GetSelectedIndex()
{
    return iTabGroup->ActiveTabIndex();
}

void CSwtMultiPageDialog::OpenL()
{
    ASSERT(iNaviCtrlCtnr);
    if (!iIsOpen)
    {
        iNaviCtrlCtnr->PushL(*iNaviDecorator);
        iIsOpen = ETrue;
        if (iVisibleTab == -1 && iTabItems.Count() > 0)
        {
            SetSelection(0);
        }
        RefreshStatusPane();
    }
}

void CSwtMultiPageDialog::SetSelection(TInt aIndex)
{
    ShowTab(aIndex);
    iTabGroup->SetActiveTabByIndex(aIndex);
}

void CSwtMultiPageDialog::SetTitleAndImageL(TInt aIndex, const TDesC& aText, MSwtImage* aImage)
{
    ASSERT(aIndex > -1);
    ASSERT(aIndex < iTabItems.Count());

    CFbsBitmap* bmp = NULL;
    CFbsBitmap* mask = NULL;
    if (aImage)
    {
        TAknLayoutText lrect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::ETabs4ActivePaneT1, iTabGroup->Rect());
        TSize size = SwtControlHelper::GetAspectRatioScaledBitmapSize(aImage->GetBounds().Size(), lrect.TextRect().Size());
        bmp = SwtControlHelper::GetCopyOfBitmapLC(&aImage->Bitmap(), size);
    }
    iTabGroup->ReplaceTabL(iTabGroup->TabIdFromIndex(aIndex), aText, bmp, mask);       // Bitmap ownership transfered
    if (bmp)
    {
        CleanupStack::Pop(bmp);
    }

    if (aText.Length() > 0 && !iTabItems[ aIndex ].iHasTitle)
    {
        iTabItems[ aIndex ].iHasTitle = ETrue;
        iTitleCount++;
        iTabGroup->SetTabFixedWidthL(ComputeTabStyle());
    }
    else if (aText.Length() < 0 && iTabItems[ aIndex ].iHasTitle)
    {
        iTabItems[ aIndex ].iHasTitle = EFalse;
        iTitleCount--;
        iTabGroup->SetTabFixedWidthL(ComputeTabStyle());
    }

    RefreshStatusPane();
}

void CSwtMultiPageDialog::SetTitleL(TInt aIndex, const TDesC& aText)
{
    ASSERT(aIndex > -1);
    ASSERT(aIndex < iTabItems.Count());

    iTabGroup->ReplaceTabTextL(aIndex, aText);
    if (!iTabItems[ aIndex ].iHasTitle)
    {
        iTabItems[ aIndex ].iHasTitle = ETrue;
        iTitleCount++;
        iTabGroup->SetTabFixedWidthL(ComputeTabStyle());
        RefreshStatusPane();
    }
}


//
// From MAknTabObserver
//
void CSwtMultiPageDialog::TabChangedL(TInt aIndex)
{
    ShowTab(aIndex);
    iDisplay.PostSelectionEventL(iPeer);
}


//
// New methods
//
void CSwtMultiPageDialog::ShowTab(TInt aIndex)
{
    ASSERT(aIndex > -1);
    ASSERT(aIndex < iTabItems.Count());

    if (aIndex == iVisibleTab) return;
    if (iVisibleTab >= 0 && iTabItems[ iVisibleTab ].iComposite)
    {
        iTabItems [iVisibleTab ].iComposite->Control() ->SetVisible(EFalse);
    }

    if (iTabItems[ aIndex ].iComposite)
    {
        iTabItems[ aIndex ].iComposite->Control() ->SetVisible(ETrue);
    }
    iVisibleTab = aIndex;
}

void CSwtMultiPageDialog::RefreshStatusPane() const
{
    if (iIsOpen)
    {
        ((CAknAppUi*) iEikonEnv->EikAppUi())->StatusPane()->DrawNow();
    }
}

TInt CSwtMultiPageDialog::ComputeTabStyle() const
{
    switch (iTabItems.Count())
    {
    case 1:
        return KTabWidthWithOneTab;
    case 2:
        return KTabWidthWithTwoTabs;
    case 3:
        return KTabWidthWithThreeTabs;
    default:
        return KTabWidthWithFourTabs;
    }
}

