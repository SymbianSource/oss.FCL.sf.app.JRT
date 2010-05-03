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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/



#include <aknphysics.h>
#include "swtphysics.h"
#include <w32std.h>


// ---------------------------------------------------------------------------
// SwtPhysicsInstanceImpl
// Implementing SwtPhysicsInstance
//----------------------------------------------------------------------------
//
EXPORT_C MSwtPhysics* SwtPhysicsLibEntryL(MSwtPhysicsObserver& aObserver, CCoeControl& aControl)
{
    return CSwtPhysics::NewL(aObserver, aControl);
}


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtPhysics::NewL
// ---------------------------------------------------------------------------
//

CSwtPhysics* CSwtPhysics::NewL(MSwtPhysicsObserver& aObserver, CCoeControl& aControl)
{
    CSwtPhysics* self = new(ELeave) CSwtPhysics(aObserver, aControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtPhysics::~CSwtPhysics
// ---------------------------------------------------------------------------
//
CSwtPhysics::~CSwtPhysics()
{
    delete iPhysics;
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
void CSwtPhysics::InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize,
                               TBool aLandscape)
{
    iPhysics->InitPhysicsL(aWorldSize, aViewSize, aLandscape);
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
TBool CSwtPhysics::StartPhysics(TPoint& aDrag, const TTime& aStartTime)
{
    return iPhysics->StartPhysics(aDrag, aStartTime);
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
void CSwtPhysics::StopPhysics()
{
    iPhysics->StopPhysics();
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
TInt CSwtPhysics::OngoingPhysicsAction() const
{
    return iPhysics->OngoingPhysicsAction();
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
void CSwtPhysics::RegisterPanningPosition(const TPoint& aDelta)
{
    iPhysics->RegisterPanningPosition(aDelta);
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
TInt CSwtPhysics::HighlightTimeout() const
{
    return iPhysics->HighlightTimeout();
}

// ---------------------------------------------------------------------------
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
TInt CSwtPhysics::DragThreshold() const
{
    return iPhysics->DragThreshold();
}

// ---------------------------------------------------------------------------
// CSwtPhysics::FeatureEnabled
// From MSwtPhysics
// ---------------------------------------------------------------------------
//
TBool CSwtPhysics::FeatureEnabled()
{
    return CAknPhysics::FeatureEnabled();
}

// ---------------------------------------------------------------------------
// CSwtPhysics::ViewPositionChanged
// From MAknPhysicsObserver
// ---------------------------------------------------------------------------
//
void CSwtPhysics::ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow,
                                      TUint aFlags)
{
    iObserver.ViewPositionChanged(aNewPosition, aDrawNow, aFlags);
}

// ---------------------------------------------------------------------------
// CSwtPhysics::ViewPositionChanged
// From MAknPhysicsObserver
// ---------------------------------------------------------------------------
//
void CSwtPhysics::ViewPositionChanged(const TPoint& aNewPosition)
{
    iObserver.ViewPositionChanged(aNewPosition, ETrue, 0);
}

// ---------------------------------------------------------------------------
// CSwtPhysics::PhysicEmulationEnded
// From MAknPhysicsObserver
// ---------------------------------------------------------------------------
//
void CSwtPhysics::PhysicEmulationEnded()
{
    iObserver.PhysicEmulationEnded();
}

// ---------------------------------------------------------------------------
// CSwtPhysics::ViewPosition
// From MAknPhysicsObserver
// ---------------------------------------------------------------------------
//
TPoint CSwtPhysics::ViewPosition() const
{
    return iObserver.ViewPosition();
}

// ---------------------------------------------------------------------------
// CSwtPhysics::CSwtPhysics
// ---------------------------------------------------------------------------
//
CSwtPhysics::CSwtPhysics(MSwtPhysicsObserver& aObserver, CCoeControl& aControl)
        : CBase()
        , iControl(aControl)
        , iObserver(aObserver)
{
}

// ---------------------------------------------------------------------------
// CSwtPhysics::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtPhysics::ConstructL()
{
    iPhysics = CAknPhysics::NewL(*this, &iControl);
}
