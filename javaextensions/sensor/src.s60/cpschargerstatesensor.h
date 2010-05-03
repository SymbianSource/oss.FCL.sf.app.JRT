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
* Description:  Charge state sensor implementation
*
*/

#ifndef CPSCHARGERSTATESENSOR_H
#define CPSCHARGERSTATESENSOR_H

#include "cpssensorbase.h"

class CPSChargerStateSensor : public CPSSensorBase
{
public:
    /**
     * Creates new CPSChargerStateSensor
     */
    static CPSChargerStateSensor* NewL();

    /**
     * Destructor
     */
    virtual ~CPSChargerStateSensor();

public: // From CSensorBase

    virtual    HBufC* CreateDescriptionLC();
    virtual CSensorBase* DuplicateL();

public: // From CPSSensorBase

    virtual TReal InterpretValue(TReal aValue);

private:
    /**
     * Constructor
     */
    CPSChargerStateSensor();
};

#endif // CPSCHARGERSTATESENSOR_H
