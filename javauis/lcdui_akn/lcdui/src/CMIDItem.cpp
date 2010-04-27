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


// CEikonEnv API used in CheckRequestedSize to obtain screen size
#include <eikenv.h>

#include "CMIDComponentFactory.h"
#include "CMIDItem.h"
// API related to command list
#include "CMIDCommandList.h"
#include "CMIDCommand.h"
#include "CMIDUtils.h"
// API related to form (iForm)
#include "CMIDForm.h"
// TlsStruct for retrieving menu handler from it
// CMIDMenuHandler::GetInstance in NotifyFormAboutCommandChange
#include "CMIDMenuHandler.h"
#include "CMIDUIManager.h"

/** If the application requests a preferred size for an item,
then this size cannot be bigger than this factor times the screen
width or height. This prevents OOM situations if applications require
crazy sizes. Note: some items may further limit this maximum values,
for example custom item does. @see CheckRequestedSize */
const TInt KMaxScreenSizeFactor = 10;

void CMIDItem::ConstructL()
//
// Zero length descriptor created, even though when a label is set this will
// be subsequently destroyed. This is to simplify code, i.e. don't need to check
// for null. When all Items set their labels in their construction, then this won't
// be necessary
//
{
    iCommandList = new(ELeave) CMIDCommandList();
    iLabel = TPtrC().AllocL();
    ASSERT(iUIManager);
    iMenuHandler = iUIManager->OpenMenuHandlerL();
}

void CMIDItem::SetLabelL(const TDesC& aLabel)
{
    delete iLabel;
    iLabel = NULL;
    iLabel = aLabel.AllocL();
}

CMIDItem::CMIDItem(MMIDItem::TLayout aLayout, CMIDUIManager* aUIManager)
        :iLayout(aLayout),
        iDefaultCommand(NULL),
        iPreferredSize(-1,-1),
        iRequestedPreferredSize(-1, -1),
        iUIManager(aUIManager)
{
}

CMIDItem::~CMIDItem()
{
    delete iLabel;
    delete iCommandList;
}

void CMIDItem::AddCommandL(MMIDCommand* aCommand, TBool aFormNotification)
{
    iCommandList->AddL(aCommand);
    if (iCommandList->Count() == 1)
    {
        SetFocusing(ETrue);
        if (iForm)
        {
            iForm->SetFocusedItemIfNone(this);
        }
    }
    if (aFormNotification)
    {
        NotifyFormAboutCommandChange();
    }
}

void CMIDItem::RemoveCommand(MMIDCommand* aCommand, TBool aFormNotification)
{
    iCommandList->Remove(aCommand);
    if (iCommandList->Count() == 0)
    {
        if ((iMMidItem->Type() == MMIDComponent::EImageItem) ||
                (iMMidItem->Type() == MMIDComponent::ECustomItem) ||
                (iMMidItem->Type() == MMIDComponent::EStringItem))
        {
            SetFocusing(EFalse);
            if (iForm)
            {
                iForm->RemoveFocusIfFocused(this);
            }
        }
        if (iForm)
        {
            DrawDeferred(); //because these items change appearance when no cmds
        }
    }
    if (aFormNotification)
    {
        NotifyFormAboutCommandChange();
    }
}

/**
 * Default implementation is empty. Subclasses provide meaningful implementations.
 **/
TBool CMIDItem::ProcessCommandL(CMIDCommand* /* aCommand */)
{
    return ETrue;
}

CMIDUIManager* CMIDItem::GetUIManager() const
{
    return iUIManager;
}

void CMIDItem::SetDefaultCommand(MMIDCommand* aCommand)
{
    iDefaultCommand = static_cast<CMIDCommand*>(aCommand);
    NotifyFormAboutCommandChange();
}

void CMIDItem::SetBuiltInMSKCommand(CMIDCommand* aCommand)
{
    iBuiltInMSKCommand = aCommand;
    NotifyFormAboutCommandChange();
}

CMIDCommand* CMIDItem::GetMSKCommand() const
{
    CMIDCommand* mskCmd = iBuiltInMSKCommand ? iBuiltInMSKCommand : iDefaultCommand;
    return mskCmd;
}

void CMIDItem::NotifyFormAboutCommandChange()
{
    if (iForm && iForm->IsCurrentItem(this))
    {
        iForm->UpdateItemCommands(iCommandList, GetMSKCommand());
        TRAP_IGNORE(iMenuHandler->UpdateMenuIfVisibleL());
    }
}

void CMIDItem::SetPreferredSize(const TSize& aSize,const TSize& aMinimumSize)
{
    iRequestedPreferredSize = CheckRequestedSize(aSize);

    // If aSize is less than aMinimumSize (per dimension), then the minimum size is used
    iPreferredSize.iHeight = Max(aSize.iHeight,aMinimumSize.iHeight);
    iPreferredSize.iWidth = Max(aSize.iWidth,aMinimumSize.iWidth);
    // Once the item has been added to a form, it's preferred size can be clipped
    // to the width of the form.
    if (iForm)
    {
        iPreferredSize.iWidth = Min(iForm->Width(),iPreferredSize.iWidth);
    }
}

void CMIDItem::SetForm(CMIDForm* aForm)
{
    iForm = aForm;
}

/** If the specified height or width or the requested size are bigger that the
screen height or width times KMaxScreenSizeFactor then return these limit values.
Otherwise return the size unchanged. @see KMaxScreenSizeFactor */
TSize CMIDItem::CheckRequestedSize(const TSize& aRequestedSize) const
{
    TSize size = aRequestedSize;
    const TSize screenSize = iEikonEnv->ScreenDevice()->SizeInPixels();

    size.iWidth = size.iWidth < KMaxScreenSizeFactor * screenSize.iWidth ?
                  size.iWidth : KMaxScreenSizeFactor * screenSize.iWidth;

    size.iHeight = size.iHeight < KMaxScreenSizeFactor * screenSize.iHeight ?
                   size.iHeight : KMaxScreenSizeFactor * screenSize.iHeight;

    return size;
}

TPtrC CMIDItem::Label() const
{
    return *iLabel;
}

void CMIDItem::SetLayoutL(MMIDItem::TLayout aLayout)
{
    iLayout = aLayout;
}

TInt CMIDItem::Type()
{
    if (iMMidItem)
    {
        return (TInt)(iMMidItem->Type());
    }
    return 0;
}

MMIDItem::TLayout CMIDItem::Layout() const
{
    return iLayout;
}

CMIDCommandList* CMIDItem::CommandList() const
{
    return iCommandList;
}

CMIDCommand* CMIDItem::DefaultCommand() const
{
    return iDefaultCommand;
}

CMIDForm* CMIDItem::Form() const
{
    return iForm;
}

void CMIDItem::ItemAddedToFormL()
{
}

void CMIDItem::ItemRemovedFromForm()
{
}

void CMIDItem::HandleCurrentL(TBool /*aCurrent*/)
{
}

void CMIDItem::NotifyScrollingCompleted()
{
}

TSize CMIDItem::PreferredSize()
{
    TInt height = Max(iPreferredSize.iHeight, MinimumSize().iHeight);
    TInt width = Max(iPreferredSize.iWidth, MinimumSize().iWidth);
    return TSize(width, height);
}

MMIDItem::TLayout CMIDItem::Layout()
{
    return iLayout;
}

void CMIDItem::SetPreferredWidth(TInt aWidth)
{
    iPreferredSize.iWidth = aWidth;
}

void CMIDItem::SetPreferredHeight(TInt aHeight)
{
    iPreferredSize.iHeight = aHeight;
}

// End of File
