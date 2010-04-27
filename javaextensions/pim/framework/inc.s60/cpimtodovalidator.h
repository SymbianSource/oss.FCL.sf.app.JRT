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
* Description:  Valid to-do data info.
 *
*/


#ifndef CPIMTODOVALIDATOR_H
#define CPIMTODOVALIDATOR_H

//  INCLUDES
#include "cpimvalidator.h"
#include "pimcommon.h"

/**
 *  ToDo data info.
 *
 *  Provides information about valid ToDo fields, attributes and
 *  values.
 */
NONSHARABLE_CLASS(CPIMToDoValidator): public CPIMValidator
{

public: // Constructors and destructor

    //@{
    /**
     * Two-phased constructors.
     */
    static CPIMToDoValidator* NewL();

    static CPIMToDoValidator* NewLC();
    //@}

    /**
     * Destructor.
     */
    virtual ~CPIMToDoValidator();

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
    CPIMToDoValidator();

};

#endif // CPIMTODOVALIDATOR_H
// End of File
