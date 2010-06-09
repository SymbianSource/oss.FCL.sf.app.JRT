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
* Description:  Class to store physics related data in forms.
*
*/


#include "CMIDForm.h"
#include "CMIDFormPhysics.h"
// #include "MMIDLcduiPhysicsInterface.h"
// used for retrieving CR repository codes
#include <centralrepository.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMIDFormPhysics::CMIDFormPhysics
// ---------------------------------------------------------------------------
//
CMIDFormPhysics::CMIDFormPhysics(CMIDForm& aParent) :
        iParent(aParent)
{
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::ConstructL
// ---------------------------------------------------------------------------
//
//typedef MMIDLcduiPhysicsInterface* (*GetInstance)(CCoeControl*, MMIDLcduiPhysicsObserver*);
void CMIDFormPhysics::ConstructL()
{
    _LIT(KDllName, "lcduiphysicswrap");

    TInt status = iPluginLoader.Load(KDllName);
    if (status == KErrNone)
    {
        GetInstance getInstanceL = (GetInstance)iPluginLoader.Lookup(1);
        iPhysics = getInstanceL(&iParent, this);
    }
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::NewL
// ---------------------------------------------------------------------------
//
CMIDFormPhysics* CMIDFormPhysics::NewL(CMIDForm& aParent)
{
    CMIDFormPhysics* self = new(ELeave) CMIDFormPhysics(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::~CMIDFormPhysics
// ---------------------------------------------------------------------------
//
CMIDFormPhysics::~CMIDFormPhysics()
{
    delete iPhysics;
    iPluginLoader.Close();
}

TBool CMIDFormPhysics::FeatureEnabled()
{
#ifdef RD_JAVA_S60_RELEASE_9_2
    return ETrue;
#else
    // Copied from aknphysicscrkeys.h
    const TUid KCRUidAknPhysicsSettings = { 0x20021192 };
    const TInt KFeatureEnabled(0x00000009);

    CRepository* cenRep = NULL;
    TRAPD(err, cenRep = CRepository::NewL(KCRUidAknPhysicsSettings))
    if (err || !cenRep)
    {
        return EFalse;
    }
    TBool featureEnabled(EFalse);
    TInt retValue = cenRep->Get(KFeatureEnabled, featureEnabled);
    delete cenRep;

    if (retValue == KErrNone)
    {
        return featureEnabled;
    }
    else
    {
        return EFalse;
    }
#endif
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::InitPhysicsL
// ---------------------------------------------------------------------------
//
void CMIDFormPhysics::InitPhysicsL(const TSize& aWorldSize,
                                   const TSize& aViewSize, const TPoint& aViewCenter)
{
    if (iPhysics)
    {
        iPhysics->InitPhysicsL(aWorldSize, aViewSize, EFalse);
        iViewCenter = aViewCenter;
        iWorldSize = aWorldSize;
        iViewSize = aViewSize;
    }
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::Stop
// ---------------------------------------------------------------------------
//
void CMIDFormPhysics::Stop()
{
    if (iPhysics)
    {
        iPhysics->StopPhysics();
        iPhysics->ResetFriction();
    }
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::StartFlick
// ---------------------------------------------------------------------------
//
TBool CMIDFormPhysics::StartFlick(const TPoint& aLength, TTime aDuration)
{
    if (iPhysics)
    {
        TPoint drag(aLength);
        return iPhysics->StartFlick(drag, aDuration);
    }

    PhysicEmulationEnded();
    return EFalse;
}


// ---------------------------------------------------------------------------
// CMIDFormPhysics::HighlightDelay
// ---------------------------------------------------------------------------
//
TInt CMIDFormPhysics::HighlightDelay() const
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
// CMIDFormPhysics::DragThreshold
// ---------------------------------------------------------------------------
//
TInt CMIDFormPhysics::DragThreshold() const
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
// CMIDFormPhysics::ViewdSize
// ---------------------------------------------------------------------------
//
TSize CMIDFormPhysics::ViewSize() const
{
    return iViewSize;
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::SetPanningPosition
// ---------------------------------------------------------------------------
//
void CMIDFormPhysics::SetPanningPosition(const TPoint& aPosition)
{
    if (iPhysics)
    {
        iPhysics->RegisterPanningPosition(aPosition);
    }
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::ViewPositionChanged
// ---------------------------------------------------------------------------
//
void CMIDFormPhysics::ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags)
{
    if (!PhysicsAllowed())
    {
        return;
    }

    TInt delta = iViewCenter.iY - aNewPosition.iY;

    iViewCenter = aNewPosition;

    iParent.HandlePhysicsScroll(delta, aDrawNow, aFlags);
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::PhysicEmulationEnded
// ---------------------------------------------------------------------------
//
void CMIDFormPhysics::PhysicEmulationEnded()
{
    iParent.PhysicsScrollingEnd();
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::ViewPosition
// ---------------------------------------------------------------------------
//
TPoint CMIDFormPhysics::ViewPosition() const
{
    return iViewCenter;
}

// ---------------------------------------------------------------------------
// CMIDFormPhysics::CanBeStopped
// ---------------------------------------------------------------------------
//
TBool CMIDFormPhysics::CanBeStopped() const
{
    if (iPhysics)
    {
        return iPhysics->CanBeStopped();
    }
    else
    {
        return ETrue;
    }
}
// ---------------------------------------------------------------------------
// CMIDFormPhysics::PhysicsAllowed
// ---------------------------------------------------------------------------
//
TBool CMIDFormPhysics::PhysicsAllowed() const
{
    if (iPhysics)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
