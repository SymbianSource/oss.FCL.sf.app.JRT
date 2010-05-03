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
* Description:  Valid contact data info.
 *
*/


#ifndef CPIMCONTACTVALIDATOR_H
#define CPIMCONTACTVALIDATOR_H

//  INCLUDES
#include "cpimvalidator.h"
#include "pimcommon.h"

/**
 *  Contact data info.
 *
 *  Provides information about valid Contact fields, attributes and
 *  values.
 */
NONSHARABLE_CLASS(CPIMContactValidator): public CPIMValidator
{

public: // Constructors and destructor

    //@{
    /**
     * Two-phased constructors.
     */
    static CPIMContactValidator* NewL();

    static CPIMContactValidator* NewLC();
    //@}

    /**
     * Destructor.
     */
    virtual ~CPIMContactValidator();

public: // Functions from CPIMValidator

    TInt NumElementsL(
        const TPIMField& aStringArrayField) const;

    TBool IsValidIntegerValue(
        const TPIMField& aField,
        const TInt& aValue) const;

    TBool IsValidStringArrayValue(
        const TPIMField& aField,
        const CDesCArray& aValue) const;

protected: // Constructor and destructor

    /**
     * Sybian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * C++ default constructor.
     */
    CPIMContactValidator();

};

#endif // CPIMCONTACTVALIDATOR_H
// End of File
