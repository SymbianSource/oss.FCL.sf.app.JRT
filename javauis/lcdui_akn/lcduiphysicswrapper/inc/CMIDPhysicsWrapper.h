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


#ifndef CMIDPHYSICSWRAPPER_H
#define CMIDPHYSICSWRAPPER_H

#include <e32base.h>
#include <aknphysicsobserveriface.h>

#include "MMIDLcduiPhysicsInterface.h"
#include "MMIDLcduiPhysicsObserver.h"

class CAknPhysics;

NONSHARABLE_CLASS(CMIDPhysicsWrapper) : public CBase,
        public MMIDLcduiPhysicsInterface,
        public MAknPhysicsObserver
{
public:

    /**
     * Two-phased constructor.
     */
    static CMIDPhysicsWrapper* NewL(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver);

    /**
     * Destructor.
     */
    virtual ~CMIDPhysicsWrapper();


    // From base class MMIDLcduiPhysicsInterface

    /**
     * checks whether physics is enabled
     *
     * @return ETrue if physics is enabled
     */
    TBool FeatureEnabled();

    /**
     * Initializes physics engine.
     *
     * @since S60 v5.0
     * @param aWorldSize Collected size of all form items.
     * @param aViewSize View's size i.e. size of form's rect.
     * @param aDirection scrolling direction EFalse = vertical, ETrue = horizontal.
     */
    void InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize, const TBool aDirection);

    /**
     * Registers panning position to physics.
     *
     * @since S60 v5.0
     * @aPosition New panning position
     */
    void RegisterPanningPosition(const TPoint& aPosition);

    /**
     * Starts the physics engine.
     *
     * @since S60 v5.0
     * @param aLength Length of the flick on display (in pixels).
     * @param aDuration Flick's duration.
     * @return ETrue if flicking was started.
     */
    TBool StartFlick(const TPoint& aLength, TTime aDuration);

    /**
     * Checks whether the physics engine can be stopped.
     *
     * @since S60 v5.0
     * @return ETrue if the physics engine can be stopped.
     */
    TBool CanBeStopped() const;

    /**
     * Stops the physics engine.
     *
     * @since S60 v5.0
     */
    void StopPhysics();


    void ResetFriction();
    /**
    * Returns highlight timeout. Tapped item should be activated only after a small delay.
    *
    * @since S60 v5.0
    * @return Highlight timeout.
    */
    TInt HighlightTimeout() const;


    /**
    * Return drag event's treshold.
    *
    * @since S60 v5.0
    * @return Drag treshold in pixels.
    */
    TInt DragThreshold() const;

    // from base class MAknPhysicsObserver

    /**
     * From MAknPhysicsObserver
     * Physics emulation has moved the view.
     *
     * @since S60 v5.0
     * @param aNewPosition The new position of the view.
     * @param aDrawNow ETrue if client should redraw the screen, otherwise EFalse.
     * @param aFlags Additional flags, not used currently.
     */
    void ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags);


    void ViewPositionChanged(const TPoint& aNewPosition);

    /**
     * From MAknPhysicsObserver
     * Physics emulation has stopped moving the view.
     *
     * @since S60 v5.0
     */
    void PhysicEmulationEnded();

    /**
     * Returns the observer view position.
     *
     * @since S60 v5.0
     * @return Physics observer view position.
     */
    TPoint ViewPosition() const;

private:

    /**
    * Constructor.
    */
    CMIDPhysicsWrapper();

    /**
    * Second stage constructor.
    */
    void ConstructL(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver);

    /**
    * Checks whether panning and flicking are allowed with current world and
    * view sizes.
    *
    * @since S60 v5.0
    * @return ETrue if allowed.
    */
    TBool PhysicsAllowed() const;

private: // data

    /**
     * Interface to the physics engine.
     * Own.
     */
    CAknPhysics* iPhysics;

    /**
    * Form whose scrolling is controlled by this class.
    */
    CCoeControl* iParent;

    MMIDLcduiPhysicsObserver* iObserver;
};

/**
 * Factory function
 *
 * @since S60 v5.0
 * @return  pointer to MMIDLcduiPhysicsInterface object
 */
IMPORT_C MMIDLcduiPhysicsInterface* GetInstanceImpl(CCoeControl* aParent, MMIDLcduiPhysicsObserver* aObserver);

#endif // CMIDPHYSICSWRAPPER_H
