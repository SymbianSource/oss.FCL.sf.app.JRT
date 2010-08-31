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
* Description:  Base class for AMMS controls.
*
*/


#ifndef CAMMSCONTROL_H
#define CAMMSCONTROL_H
//  INCLUDES
#include <e32base.h>
#include <cmmacontrol.h>
#include "ammsconstants.h"
// RD_JAVA_HTTP_EMC_ENABLED
#ifdef RD_JAVA_HTTP_EMC_ENABLED
#include "StreamControl.h"
#include "EffectControl.h"
#include "MMControlFactory.h"
using namespace multimedia;
using multimedia ::MStreamControl;
using multimedia ::MEffectControl;
using multimedia ::CMultimediaFactory;
#endif

// FORWARD DECLARATIONS
class CCustomCommandUtility;
class CMMAPlayer;

// CLASS DECLARATION

/**
*
*  Base class for AMMS controls.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSControl): public CMMAControl
{
public:
    /**
    * Destructor.
    */
    virtual ~CAMMSControl();

public: // New functions
    /**
    * Creates the custom command utility.
    * @since 3.0
    */
    CCustomCommandUtility* CreateCustomCommandUtilityL();

    /**
    * Prepares the Control by creating the Effect API Control.
    */
    virtual void PrepareControlL();

    /**
    * Deallocates the Control by deleting the Effect API Control.
    */
    virtual void DeallocateControl();

public:  // From CMMAControl

    const TDesC& PublicClassName() const;

protected:

    /**
    * C++ default constructor.
    * @param aPlayer Player that has this control.
    */
    CAMMSControl(CMMAPlayer* aPlayer);

public:  // Data

    /** Type of the Control */
    TAMMSControlTypes iControlType;

protected:   // Data

    CMMAPlayer* iPlayer;  // Not owned.
};

#endif // CAMMSCONTROL_H


