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
* Description:  Controls the volume of a CMMAVolumeControl.
*
*/


#ifndef CAMMSVOLUMECONTROL_H
#define CAMMSVOLUMECONTROL_H

//  INCLUDES
#include <e32base.h>
#include "cammscontrol.h"

// FORWARD DECLARATIONS
class CMMAVolumeControl;

// CONSTANTS
const TInt KAMMSMAXVolume = 100;

// CLASS DECLARATION

/**
*
*  Controls the volume of a CMMAVolumeControl.
*  This class delegates SetVolumeL method calls to CMMAVolumeControl
*  given in constructor.
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSVolumeControl): public CAMMSControl
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aControlName Class name for this control.
    * @param aVolumeControl Delegated control.
    * @param aPlayer Player that has this control.
    */
    static CAMMSVolumeControl* NewLC(const TDesC& aControlName,
    CMMAVolumeControl* aVolumeControl,
    CMMAPlayer* aPlayer);

    /**
    * Destructor.
    */
    ~CAMMSVolumeControl();

public: // New functions
    /**
    * Sets the volume..
    *
    * @param aVolume The new volume to be set.
    */
    void SetVolumeL(TInt aVolume);

public: // Functions from base classes
    const TDesC& ClassName() const;

private:
    /**
    * C++ constructor.
    * @param aControlName Class name for this control.
    * @param aVolumeControl Delegated control.
    * @param aPlayer Player that has this control.
    */
    CAMMSVolumeControl(const TDesC& aControlName,
                       CMMAVolumeControl* aVolumeControl,
                       CMMAPlayer* aPlayer);

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private:  // Data
    const TDesC& iClassName;

    // Not owned mma volume control.
    CMMAVolumeControl* iVolumeControl;

    // Index in iVolumeControl
    TInt iControlLevelIndex;
};

#endif // CAMMSVOLUMECONTROL_H


