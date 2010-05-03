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
* Description:  Battery Level sensor implementation
*
*/

#ifndef CPSBATTERYCHARGESENSOR_H
#define CPSBATTERYCHARGESENSOR_H

#include "cpssensorbase.h"

class CPSBatteryChargeSensor : public CPSSensorBase
{
public:
    /**
     * Creates new CPSBatteryChargeSensor
     */
    static CPSBatteryChargeSensor* NewL();

    /**
     * Destructor
     */
    virtual ~CPSBatteryChargeSensor();

public: // From CSensorBase

    virtual    HBufC* CreateDescriptionLC();
    virtual CSensorBase* DuplicateL();

public: // From CPSSensorBase

    virtual TReal InterpretValue(TReal aValue);

private:
    /**
     * Constructor
     */
    CPSBatteryChargeSensor();
};

#endif // CPSBATTERYCHARGESENSOR_H
