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
* Description:  Valid event data info.
 *
*/


#ifndef CPIMEVENTVALIDATOR_H
#define CPIMEVENTVALIDATOR_H

//  INCLUDES
#include "cpimvalidator.h"
#include "pimcommon.h"

/**
 *  Event data info.
 *
 *  Provides information about valid Event fields, attributes and
 *  values.
 */
NONSHARABLE_CLASS(CPIMEventValidator): public CPIMValidator
{

public: // Constructors and destructor

    //@{
    /**
     * Two-phased constructors.
     */
    static CPIMEventValidator* NewL();

    static CPIMEventValidator* NewLC();
    //@}

    /**
     * Destructor.
     */
    virtual ~CPIMEventValidator();

public: // Functions from CPIMValidator

    TBool IsValidIntegerValue(
        const TPIMField& aField,
        const TInt& aValue) const;

protected: // Constructor and destructor

    /**
     * Sybian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CPIMEventValidator();

};

#endif // CPIMEVENTVALIDATOR_H
// End of File
