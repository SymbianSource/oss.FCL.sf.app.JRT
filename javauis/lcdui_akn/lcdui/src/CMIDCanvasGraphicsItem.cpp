/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMIDCanvasGraphicsItem - Implements the native part Canvas Graphics Item class.
*
*/

// EXTERNAL INCLUDES
#include <MMIDCustomComponentContainer.h>
#include <lcdui.h>
#include <fbs.h>
#include <bitdev.h>
#include <bitstd.h>
#include <j2me/jdebug.h>

// INTERNAL INCLUDES
#include "CMIDCanvasGraphicsItem.h"
#include "CMIDCanvasGraphicsItemPainter.h"
#include "CMIDUtils.h"

// Count of compound components in this custom component.
const TInt KComponentCount      = 1;

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::NewL
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItem* CMIDCanvasGraphicsItem::NewL(
    const TCtorParams& aParams)
{
    CMIDCanvasGraphicsItem* self = new(ELeave) CMIDCanvasGraphicsItem();
    CleanupStack::PushL(self);
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::ConstructL
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::ConstructL(const TCtorParams& aParams)
{
    // Set painter
    iItemPainter = aParams.iPainterHandle;
    iItemPainter->SetItem(this);
    iComponentContainer = NULL;
    iUtils = aParams.iUtils;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::CMIDCanvasGraphicsItem
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItem::CMIDCanvasGraphicsItem() :
        iUtils(NULL)
{
    // No implementation.
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::~CMIDCanvasGraphicsItem
// ---------------------------------------------------------------------------
//
CMIDCanvasGraphicsItem::~CMIDCanvasGraphicsItem()
{
    DEBUG("CMIDCanvasGraphicsItem::~CMIDCanvasGraphicsItem +");

    // Remove this component from the container if set.
    if (iComponentContainer)
    {
        iComponentContainer->UnregisterComponent(this);
    }

    DEBUG("CMIDCanvasGraphicsItem::~CMIDCanvasGraphicsItem -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::CustomComponentControlCount
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDCanvasGraphicsItem::CustomComponentControlCount() const
{
    return KComponentCount;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::CustomComponentControl
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CCoeControl* CMIDCanvasGraphicsItem::CustomComponentControl(TInt aIndex)
{
    if (aIndex == 0)
    {
        return iItemPainter;
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::CustomComponentContainerDisposing
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::CustomComponentContainerDisposing()
{
    iComponentContainer = NULL;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetParentL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetParentL(
    MMIDCustomComponentContainer* aComponentContainer)
{
    DEBUG("CMIDCanvasGraphicsItem::SetParentL +");

    // Remove association from the previous container.
    if (iComponentContainer)
    {
        DEBUG("CMIDCanvasGraphicsItem::SetParentL, unregistering old association");

        iComponentContainer->UnregisterComponent(this);
        iComponentContainer = NULL;
    }

    // Associate this component to the new container.
    if (aComponentContainer)
    {
        DEBUG("CMIDCanvasGraphicsItem::SetParentL, registering new");

        // Register this custom component to the container.
        aComponentContainer->RegisterComponentL(this);
        // Initializes painter with the new parent
        iItemPainter->SetParentL(aComponentContainer);
    }

    // Store container. NULL is ok.
    iComponentContainer = aComponentContainer;

    // Set item size and position is needed here.
    TSize size = iItemPainter->Size();
    SetSizeL(size.iWidth, size.iHeight);
    TPoint position = iItemPainter->Position();
    SetPosition(position.iX, position.iY);

    if (iUtils)
    {
        // Set canvas fullscreen size is needed here.
        iItemPainter->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());
    }

    DEBUG("CMIDCanvasGraphicsItem::SetParentL -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetDirectContainerL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetDirectContainerL(
    MDirectContainer* aDirectContainer)
{
    iItemPainter->SetDirectContainerL(aDirectContainer);
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetElevationL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetElevationL(TInt aElevation)
{
    DEBUG_INT("CMIDCanvasGraphicsItem::SetElevationL +, aElevation=%d",
              aElevation);

    // Parent must have been set.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    // The elevation is handled in custom component container.
    iComponentContainer->SetComponentIndexL(this, aElevation);

    DEBUG("CMIDCanvasGraphicsItem::SetElevationL -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::Elevation
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDCanvasGraphicsItem::Elevation()
{
    DEBUG("CMIDCanvasGraphicsItem::Elevation +");

    // Parent must have been set.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    // The elevation is retrieved from custom component container
    TInt index = iComponentContainer->ComponentIndex(this);

    DEBUG_INT("CMIDCanvasGraphicsItem::Elevation - index=%d", index);

    return index;
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::Show
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetVisibleL(TBool aVisible)
{
    DEBUG_INT("CMIDCanvasGraphicsItem::SetVisibleL +, aVisible=%d", aVisible);

    // SetParentL must be called before Show.
    __ASSERT_DEBUG(iComponentContainer, User::Invariant());

    iItemPainter->SetVisibleL(aVisible);

    DEBUG("CMIDCanvasGraphicsItem::SetVisibleL -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetSizeL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetSizeL(const TInt aWidth, const TInt aHeight)
{
    // We need store non-scaled size for possible resolution
    // and fullscreen mode changes.
    iNonScaledSize = TSize(aWidth, aHeight);
    DEBUG_INT2(
        "CMIDCanvasGraphicsItem::SetItemSize +, aWidth=%d, aHeight=%d",
        aWidth, aHeight);

    // Set item size.
    iItemPainter->SetItemSizeL(aWidth, aHeight);

    // If parent is in fullscreen mode
    // then it tries scale viewing rect for painter.
    // Otherwise the non scaled size is set.
    if (IsScalingOn())
    {
        iItemPainter->SetViewSize(iUtils->DoScaling(iNonScaledSize));
    }
    else
    {
        iItemPainter->SetViewSize(iNonScaledSize);
    }

    DEBUG("CMIDCanvasGraphicsItem::SetItemSize -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::SetPosition
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::SetPosition(const TInt aX, const TInt aY)
{
    DEBUG_INT2("CMIDCanvasGraphicsItem::SetPosition +, aX=%d, aY=%d", aX, aY);

    // We need store non-scaled size for possible resolution
    // and fullscreen mode changes.
    iNonScaledPosition = TPoint(aX, aY);

    // If parent is in fullscreen mode
    // then it tries scale position of item.
    // Otherwise the non scaled size is set.
    if (IsScalingOn())
    {
        TPoint origin = iUtils->DoScalingAndPositioning(iNonScaledPosition);
        iItemPainter->SetPosition(origin.iX, origin.iY);
    }
    else
    {
        iItemPainter->SetPosition(aX, aY);
    }

    DEBUG("CMIDCanvasGraphicsItem::SetPosition -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::Dispose
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::Dispose()
{
    DEBUG("CMIDCanvasGraphicsItem::Dispose +");

    delete this;

    DEBUG("CMIDCanvasGraphicsItem::Dispose -");
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::HandleFullscreenModeChange
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::HandleFullscreenModeChange()
{
    // Calling all functions which set size and position.
    TRAPD(err, SetSizeL(iNonScaledSize.iWidth, iNonScaledSize.iHeight));
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDCanvasGraphicsItem::HandleFullscreenModeChange: SetSizeL method leave with %d code", err);
    }
    SetPosition(iNonScaledPosition.iX, iNonScaledPosition.iY);

    if (iUtils)
    {
        // Setting of fullscreen canvas rect.
        iItemPainter->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvasGraphicsItem::HandleResolutionChange
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvasGraphicsItem::HandleResolutionChange()
{
    // Calling all functions which set size and position.
    TRAPD(err, SetSizeL(iNonScaledSize.iWidth, iNonScaledSize.iHeight));
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDCanvasGraphicsItem::HandleFullscreenModeChange: SetSizeL method leave with %d code", err);
    }
    SetPosition(iNonScaledPosition.iX, iNonScaledPosition.iY);
    if (iUtils)
    {
        // Setting of fullscreen canvas rect.
        iItemPainter->SetOnScreenCanvasRect(iUtils->GetOnScreenCanvasRect());
    }
}

TBool CMIDCanvasGraphicsItem::IsScalingOn() const
{
    return iUtils && iComponentContainer && iUtils->IsScalingEnabled() &&
           iComponentContainer->IsFullScreen();
}

// End of file
