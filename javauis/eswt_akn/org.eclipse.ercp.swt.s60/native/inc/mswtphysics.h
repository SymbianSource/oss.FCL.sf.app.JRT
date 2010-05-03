/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef MSWTPHYSICS_H
#define MSWTPHYSICS_H


#include <coecntrl.h>


/**
 *  Wrapper to MAknPhysicsObserver.
 *
 *  @lib swtphysics
 *  @since S60 v5.0
 */
class MSwtPhysicsObserver
{

public:
    virtual void ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow,
                                     TUint aFlags) = 0;
    virtual void PhysicEmulationEnded() = 0;
    virtual TPoint ViewPosition() const = 0;
};


/**
 *  Wrapper to CAknPhysics.
 *
 *  @lib swtphysics
 *  @since S60 v5.0
 */
class MSwtPhysics
{
public:
    enum TSwtPhysicsAction
    {
        ESwtPhysicsActionNone = 1,
        ESwtPhysicsActionFlicking,
        ESwtPhysicsActionBouncing
    };
public:
    virtual void InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize,
                              TBool aLandscape) = 0;
    virtual TBool StartPhysics(TPoint& aDrag, const TTime& aStartTime) = 0;
    virtual void StopPhysics() = 0;
    virtual TInt OngoingPhysicsAction() const = 0;
    virtual void RegisterPanningPosition(const TPoint& aDelta) = 0;
    virtual TInt HighlightTimeout() const = 0;
    virtual TInt DragThreshold() const = 0;
    virtual TBool FeatureEnabled() = 0;
    virtual ~MSwtPhysics() {};
};


/**
 * Library entry point
 */
typedef MSwtPhysics*(*TSwtPhysicsLibEntry)(MSwtPhysicsObserver&, CCoeControl&);


#endif // MSWTPHYSICS_H
