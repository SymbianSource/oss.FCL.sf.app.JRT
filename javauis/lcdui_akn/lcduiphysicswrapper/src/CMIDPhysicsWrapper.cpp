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
* Description:  Wrapper class to communicate physics related data.
*
*/


#include <aknphysics.h>
#include "CMIDPhysicsWrapper.h"
#include <w32std.h>

// ---------------------------------------------------------------------------
//  GetInstanceImpl
//----------------------------------------------------------------------------
//
EXPORT_C MMIDLcduiPhysicsInterface* GetInstanceImpl(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver)
{
    MMIDLcduiPhysicsInterface* interface = 0;
    TRAP_IGNORE(interface = CMIDPhysicsWrapper::NewL(aParent, aObserver));
    return interface;
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::CMIDPhysicsWrapper
// ---------------------------------------------------------------------------
//
CMIDPhysicsWrapper::CMIDPhysicsWrapper()
{
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::ConstructL
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::ConstructL(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver)
{
    iParent = aParent;
    iObserver = aObserver;
    iPhysics = CAknPhysics::NewL(*this, iParent);
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::NewL
// ---------------------------------------------------------------------------
//

CMIDPhysicsWrapper* CMIDPhysicsWrapper::NewL(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver)
{
    CMIDPhysicsWrapper* self = new(ELeave) CMIDPhysicsWrapper;
    CleanupStack::PushL(self);
    self->ConstructL(aParent, aObserver);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::~CMIDPhysicsWrapper
// ---------------------------------------------------------------------------
//

CMIDPhysicsWrapper::~CMIDPhysicsWrapper()
{
    delete iPhysics;
}


TBool CMIDPhysicsWrapper::FeatureEnabled()
{
    return CAknPhysics::FeatureEnabled();
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::InitPhysicsL
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::InitPhysicsL(const TSize& aWorldSize,
                                      const TSize& aViewSize, const TBool aDirection)
{
    if (iPhysics)
    {
        iPhysics->InitPhysicsL(aWorldSize, aViewSize, aDirection);
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::StopPhysics
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::StopPhysics()
{
    if (iPhysics)
    {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::ResetFriction
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::ResetFriction()
{
    if (iPhysics)
    {
        iPhysics->ResetFriction();
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::StartFlick
// ---------------------------------------------------------------------------
//
TBool CMIDPhysicsWrapper::StartFlick(const TPoint& aLength, TTime aDuration)
{
    if (iPhysics)
    {
        TPoint drag(aLength);
        iPhysics->StartPhysics(drag, aDuration);
        return ETrue;
    }

    PhysicEmulationEnded();
    return EFalse;
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::HighlightDelay
// ---------------------------------------------------------------------------
//
TInt CMIDPhysicsWrapper::HighlightTimeout() const
{
    if (iPhysics)
    {
        return iPhysics->HighlightTimeout();
    }
    else
    {
        return 0;
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::DragThreshold
// ---------------------------------------------------------------------------
//
TInt CMIDPhysicsWrapper::DragThreshold() const
{
    if (iPhysics)
    {
        return iPhysics->DragThreshold();
    }
    else
    {
        return 0;
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::SetPanningPosition
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::RegisterPanningPosition(const TPoint& aPosition)
{
    if (iPhysics)
    {
        iPhysics->RegisterPanningPosition(aPosition);
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags)
{
    if (!iPhysics)
    {
        return;
    }

    if (iParent && iObserver)
    {
        iObserver->ViewPositionChanged(aNewPosition, aDrawNow, aFlags);
    }
}

void CMIDPhysicsWrapper::ViewPositionChanged(const TPoint& aNewPosition)
{
    if (!iPhysics)
    {
        return;
    }

    if (iParent && iObserver)
    {
        iObserver->ViewPositionChanged(aNewPosition, ETrue, 0);
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//
void CMIDPhysicsWrapper::PhysicEmulationEnded()
{
    if (iParent && iObserver)
    {
        iObserver->PhysicEmulationEnded();
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::ViewPosition
// ---------------------------------------------------------------------------
//
TPoint CMIDPhysicsWrapper::ViewPosition() const
{
    if (iObserver)
    {
        return iObserver->ViewPosition();
    }
    else
    {
        // Must return something
        return TPoint(0,0);
    }
}

// ---------------------------------------------------------------------------
// CMIDPhysicsWrapper::CanBeStopped
// ---------------------------------------------------------------------------
//
TBool CMIDPhysicsWrapper::CanBeStopped() const
{
    if (iPhysics)
    {
        return iPhysics->OngoingPhysicsAction() !=
               CAknPhysics::EAknPhysicsActionBouncing;
    }
    else
    {
        return ETrue;
    }
}
