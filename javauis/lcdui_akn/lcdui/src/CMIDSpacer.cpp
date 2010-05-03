/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include <coemain.h>
//
#include "CMIDSpacer.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDSpacer* CMIDSpacer::NewL(const TSize& aMinimumSize, CMIDUIManager* aUIManager)
{
    CMIDSpacer* item = new(ELeave) CMIDSpacer(aUIManager);
    CleanupStack::PushL(item);
    item->ConstructL(aMinimumSize);
    CleanupStack::Pop(item);
    return item;
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMIDSpacer::CMIDSpacer(CMIDUIManager* aUIManager)
        :CMIDControlItem(EDefault, aUIManager)
{
    iMMidItem = this;
}

void CMIDSpacer::ConstructL(const TSize& aMinimumSize)
{
    CMIDControlItem::ConstructL();
    SetMinimumSizeL(aMinimumSize);
    SetFocusing(EFalse);
}

TBool CMIDSpacer::IsSelectable() const
{
    return EFalse;
}

TSize CMIDSpacer::MinimumSize()
{
    return iMinimumSize;
}

void CMIDSpacer::Draw(const TRect& /*aRect*/) const
{
}

/**
 * If the preferred size is currently unlocked (i.e. has not been set)
 * this will return the minimum size. If the preferred size has been
 * locked then this will return the preferred size.
 */
TSize CMIDSpacer::PreferredSize() const
{
    if (iPreferredSize.iHeight == -1 && iPreferredSize.iHeight == -1)
    {
        return MinimumSize();
    }
    else
    {
        return iPreferredSize;
    }
}

/**
 * aSize is only ever TSize(0,0) or greater. If the new minimum size is
 * greater than the preferred size, then the preferred size needs to be
 * set equal to the min size.
 */
void CMIDSpacer::SetMinimumSizeL(const TSize& aSize)
{
    iMinimumSize = aSize;
}

void CMIDSpacer::SetLabelL(const TDesC& /*aLabel*/)
//
// Never called
//
{
    ASSERT(EFalse);
}

void CMIDSpacer::SetLayoutL(TLayout aLayout)
{
    CMIDControlItem::SetLayoutL(aLayout);
}

void CMIDSpacer::SetPreferredSizeL(const TSize& aSize)
{
    CMIDItem::SetPreferredSize(aSize,MinimumSize());
}

void CMIDSpacer::Dispose()
{
    delete this;
}

void CMIDSpacer::AddCommandL(MMIDCommand* /*aCommand*/)
//
// Never called
//
{
    ASSERT(EFalse);
}

void CMIDSpacer::RemoveCommand(MMIDCommand* /*aCommand*/)
//
// Never called
//
{
    ASSERT(EFalse);
}

void CMIDSpacer::SetDefaultCommand(MMIDCommand* /*aCommand*/)
//
// Never called
//
{
    ASSERT(EFalse);
}

TSize CMIDSpacer::MinimumSize() const
//
// Initially TSize(0,0) until actually set
//
{
    CCoeControl* control = const_cast<CMIDSpacer*>(this);
    return control->MinimumSize();
}

TInt CMIDSpacer::ItemPreferredHeightWithoutLabel()
{
    return iMinimumSize.iHeight;
}

void CMIDSpacer::SetUsedForStringFormatting(TBool iVal)
{
    iUsedForStringFormatting = iVal;
}

TBool CMIDSpacer::UsedForStringFormatting()
{
    return iUsedForStringFormatting;
}
