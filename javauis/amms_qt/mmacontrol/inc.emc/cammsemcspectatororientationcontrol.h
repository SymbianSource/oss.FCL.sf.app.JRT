/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Manipulates the virtual orientation of the spectator.
*
*/


#ifndef CAMMSEMCSPECTATORORIENTATIONCONTROL_H
#define CAMMSEMCSPECTATORORIENTATIONCONTROL_H

//  INCLUDES
#include <e32base.h>
#include<ListenerOrientationControl.h>
#include <OrientationData.h>
#include "cammsorientationcontrol.h"
#include <cmmaemcaudioplayer.h>

using multimedia :: MListenerOrientationControl;
using multimedia :: KListenerOrientationEffectControl;

// CONSTANTS
_LIT(KAMMSEMCSpectatorOrientationControl, "OrientationControl");

// CLASS DECLARATION

/**
*
*  Controls for the Orientation Control effect for the Spectator.
*  This class delegates Orientation Control effect method calls to
*  MListenerOrientationControl.
*
*
*  @since 5.1
*/
NONSHARABLE_CLASS(CAMMSEMCSpectatorOrientationControl): public CAMMSOrientationControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlayer Player that has this control.
    */
    static CAMMSEMCSpectatorOrientationControl* NewLC(CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSEMCSpectatorOrientationControl();

public: // Functions from base classes
    const TDesC& ClassName() const;

    /**
    * Prepares the Control.
    */
    void PrepareControlL();

    /**
    * Deallocates the Control.
    */
    void DeallocateControl();

private:
    /**
    * C++ constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSEMCSpectatorOrientationControl(CMMAPlayer* aPlayer);
public:
    //base class function overidden
    virtual void SetOrientationL(TInt aHeading, TInt aPitch, TInt aRoll);
    virtual void SetOrientationL(TInt aFrontX,
                                 TInt aFrontY,
                                 TInt aFrontZ,
                                 TInt aAboveX,
                                 TInt aAboveY,
                                 TInt aAboveZ);

    virtual void OrientationVectors(TInt& aFrontX,
                                    TInt& aFrontY,
                                    TInt& aFrontZ,
                                    TInt& aAboveX,
                                    TInt& aAboveY,
                                    TInt& aAboveZ);


private:

    CMMAPlayer *iMMAPlayer;
    /**
    * EMC Listener Orientation Control
    */

    CMultimediaFactory* iFactory;
    MStreamControl* iStreamControl;
    MListenerOrientationControl *iMListenerOrientationControl;

};

#endif // CAMMSEMCSPECTATORORIENTATIONCONTROL_H


