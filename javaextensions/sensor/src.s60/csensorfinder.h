/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Factory for Sensors
*
*/


#ifndef CSENSORFINDER_H
#define CSENSORFINDER_H

// INCLUDES
#include "sensorfinder.h"
#include "csensorbase.h"
#include <e32std.h>
#include <e32base.h>
#include <e32cmn.h>


// FORWARD DECLARATIONS
class Sensor;

// CLASS DECLARATION

/**
 *  S60 implementation class for SensorFinder
 *  This class is used for finding sensors and
 *  creating them
 *
 */
class CSensorFinder : public SensorFinder, public CBase
{
public:
    // Constructors and destructor
    /**
     * Constructor
     */
    CSensorFinder();

    /**
     * Destructor.
     */
    ~CSensorFinder();

public: // From SensorFinder

    unsigned short** FindSensors(int* aSensorCount);
    Sensor* CreateSensor(int aIndex);

protected: // New methods
    /**
     * Adds sensors which are not coming from SensorFW
     */
    virtual void AddCustomSensorsL();

private: // New methods
    /**
     * Adds custom sensors and
     * finds sensors from SensorFW
     */
    void FindSensorsL();

private: // Data
    // Array of sensors
    RPointerArray<CSensorBase> iSensors;
};

#endif // CSENSORFINDER_H

// End of file
