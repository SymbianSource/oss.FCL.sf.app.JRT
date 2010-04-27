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


#ifndef SWTPHYSICS_H
#define SWTPHYSICS_H


#include <e32base.h>
#include <aknphysicsobserveriface.h>
#include "mswtphysics.h"


class CAknPhysics;


NONSHARABLE_CLASS(CSwtPhysics)
        : public CBase
        , public MSwtPhysics
        , public MAknPhysicsObserver
{
// Own methods
public:
    static CSwtPhysics* NewL(MSwtPhysicsObserver& aObserver, CCoeControl& aControl);
    virtual ~CSwtPhysics();

// From MSwtPhysics
public:
    void InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize,
                      TBool aLandscape);
    TBool StartPhysics(TPoint& aDrag, const TTime& aStartTime);
    void StopPhysics();
    TInt OngoingPhysicsAction() const;
    void RegisterPanningPosition(const TPoint& aDelta);
    TInt HighlightTimeout() const;
    TInt DragThreshold() const;
    TBool FeatureEnabled();

// From MAknPhysicsObserver
public:
    void ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow,
                             TUint aFlags);
    void ViewPositionChanged(const TPoint& aNewPosition);
    void PhysicEmulationEnded();
    TPoint ViewPosition() const;

// Own methods
private:
    CSwtPhysics(MSwtPhysicsObserver& aObserver, CCoeControl& aControl);
    void ConstructL();

// Data
private:
    CAknPhysics* iPhysics; // Own
    CCoeControl& iControl;
    MSwtPhysicsObserver& iObserver;
};

/**
 * Factory
 */
IMPORT_C MSwtPhysics* SwtPhysicsLibEntryL(MSwtPhysicsObserver&, CCoeControl&);


#endif // SWTPHYSICS_H
