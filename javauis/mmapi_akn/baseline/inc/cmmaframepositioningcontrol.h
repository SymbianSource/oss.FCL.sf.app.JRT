/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Abstract base class for FramePositioningControl
*
*/


#ifndef CMMAFRAMEPOSITIONINGCONTROL_H
#define CMMAFRAMEPOSITIONINGCONTROL_H

// EXTERNAL INCLUDES
#include "mmf/common/mmfcontroller.h"
#include "mmf/common/mmfstandardcustomcommands.h"

//  INTERNAL INCLUDES
#include "cmmacontrol.h" // base class

// FORWARD DECLARATIONS
class CMMAPlayer;

// CONSTANTS


// Constant for control name. Name is used in Java side to instantiate
// Java Class which uses CMMAFramePositioningControl.
_LIT(KMMAFramePositioningControlName, "FramePositioningControl");

//  CLASS DECLARATION
/**
*   Abstract base class for FramePositioningControls
*/
NONSHARABLE_CLASS(CMMAFramePositioningControl): public CMMAControl
{
public:
    /**
     * Destructor.
     */
    ~CMMAFramePositioningControl();

protected:
    /**
     * Constructor.
     */
    CMMAFramePositioningControl(CMMAPlayer* aPlayer);

public: // From CMMAControl
    const TDesC& ClassName() const;

public: // New methods
    virtual TInt SeekL(TInt aFrameNumber) = 0;
    virtual TInt SkipL(TInt aFramesToSkip) = 0;
    virtual void MapFrameToTimeL(TInt aValue, TInt64* aMediaTime) = 0;
    virtual TInt MapTimeToFrameL(TInt64* aMediaTime) = 0;

protected: // New methods

    /**
     * Clamp media time between 0 and duration of media
     * @param aMediaTime media time value to clamp
     * @return KErrNotFound if duration of media is not known,
     *         otherwise KErrNone.
     */
    virtual TInt ClampMediaTime(TInt64& aMediaTime);

private: // Data

    // To be used by this class and not by children.
    CMMAPlayer* iPlayer;

};

#endif // CMMAFRAMEPOSITIONINGCONTROL_H
