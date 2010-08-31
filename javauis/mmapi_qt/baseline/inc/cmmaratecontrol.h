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
* Description:  Base class for RateControl implementations.
*
*/


#ifndef CMMARATECONTROL_H
#define CMMARATECONTROL_H

// EXTERNAL INCLUDES

//  INTERNAL INCLUDES
#include "cmmacontrol.h" // base class

// CONSTANTS

static const TInt KMMADefaultRate = 100000;
static const TInt KMMAMinRate = 0;

// Constant for control name. Name is used in Java side to instantiate
// Java Class which uses CMMARateControl.
_LIT(KMMARateControlName, "RateControl");


//  CLASS DECLARATION
/**
*   This class is the base class for RateControl implementations.
*
*
*/
NONSHARABLE_CLASS(CMMARateControl): public CMMAControl
{
public:
    /**
     * Destructor.
     */
    ~CMMARateControl();

protected:
    /**
     * Constructor.
     */
    CMMARateControl();

public: // From CMMAControl
    virtual const TDesC& ClassName() const;

public: // New methods
    /**
     * @param aRate Rate to set in "milli-percentage"..
     * @return Actual rate set.
     */
    virtual TInt SetRateL(TInt aRate) = 0;

    /**
     * @return The current playback rate in "milli-percentage".
     */
    virtual TInt RateL() = 0;

    /**
     * @return The maximum rate supported.
     */
    virtual TInt MaxRateL();

    /**
     * @return The minimum rate supported.
     */
    virtual TInt MinRateL();

};


#endif // CMMARATECONTROL_H
