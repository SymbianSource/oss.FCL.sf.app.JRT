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
* Description:  Class to provide physics related data in forms.
*
*/


#ifndef CMIDFORMPHYSICS_H
#define CMIDFORMPHYSICS_H


#include <e32base.h>
#include "MMIDLcduiPhysicsInterface.h"

class CMIDForm;

NONSHARABLE_CLASS(CMIDFormPhysics) : public CBase, public MMIDLcduiPhysicsObserver
{
public:
    /**
    * Two-phased constructor.
    */
    static CMIDFormPhysics* NewL(CMIDForm& aParent);

    /**
    * Destructor.
    */
    virtual ~CMIDFormPhysics();

    static TBool FeatureEnabled();

    /**
    * Initializes physics engine.
    *
    * @since S60 v5.0
    * @param aWorldSize Collected size of all form items.
    * @param aViewSize View's size i.e. size of form's rect.
    * @param aViewCenter View center's initial position.
    */
    void InitPhysicsL(const TSize& aWorldSize, const TSize& aViewSize, const TPoint& aViewCenter);

    /**
    * Stops the physics engine.
    *
    * @since S60 v5.0
    */
    void Stop();

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
    * Returns highlight timeout. Tapped item should be activated only after a small delay.
    *
    * @since S60 v5.0
    * @return Highlight delay.
    */
    TInt HighlightDelay() const;


    /**
    * Return drag event's treshold.
    *
    * @since S60 v5.0
    * @return Drag treshold in pixels.
    */
    TInt DragThreshold() const;


    /**
    * Returns current view size.
    *
    * @since S60 v5.0
    * @return View size.
    */
    TSize ViewSize() const;

    /**
    * Registers panning position to physics.
    *
    * @since S60 v5.0
    * @aPosition New panning position
    */
    void SetPanningPosition(const TPoint& aPosition);


public: // from base class MMIDLcduiPhysicsObserver
    /**
     * Physics emulation has moved the view.
     *
     * @since S60 v5.0
     * @param aNewPosition The new position of the view.
     * @param aDrawNow ETrue if client should redraw the screen, otherwise EFalse.
     * @param aFlags Additional flags, not used currently.
     */
    void ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags);

    /**
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

public:
    /**
     * Checks whether the physics engine can be stopped.
     *
     * @since S60 v5.0
     * @return ETrue if the physics engine can be stopped.
     */
    TBool CanBeStopped() const;

    /**
    * Checks whether panning and flicking are allowed
    *
    * @since S60 v5.0
    * @return ETrue if allowed.
    */
    TBool PhysicsAllowed() const;

private:

    /**
    * Constructor.
    */
    CMIDFormPhysics(CMIDForm& aParent);

    /**
    * Second stage constructor.
    */
    void ConstructL();

public: // data
    /**
    * Center of the "view" (the visible window to the whole form i.e. form's
    * virtual rect).
    */
    TPoint iViewCenter;

private: // data

    /**
     * Interface to the physics engine.
     * Own.
     */
    MMIDLcduiPhysicsInterface* iPhysics;

    /**
    * Dialog page whose scrolling is controlled by this class.
    */
    CMIDForm& iParent;

    /**
    * Size of the "world".
    */
    TSize iWorldSize;

    /**
    * View size.
    */
    TSize iViewSize;

    RLibrary   iPluginLoader;
};

#endif // CMIDFORMPHYSICS_H
