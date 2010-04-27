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
* Description:  Specialized integer value class
 *
*/


#ifndef CPIMINTVALUE_H
#define CPIMINTVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized integer value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMIntValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMIntValue* NewL(TPIMAttribute aAttributes,
    TInt aIntegerValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMIntValue* NewLC(TPIMAttribute aAttributes,
                               TInt aIntegerValue);

    /**
     * Destructor
     */
    virtual ~CPIMIntValue();

public: // Functions from CPIMValue

    /**
     * SetValue
     * Sets new integer value for this field
     * value holder. The old value will be overwritten
     *
     * @param aIntegerValue New value
     */
    virtual void SetIntValue(TInt aIntegerValue);

    /**
     * Value
     * Returns the integer value of this integer
     * value holder. The value cannot be modified
     *
     * @return Integer value
     */
    virtual TInt IntValue() const;

private:

    /**
     * Default C++ constructor
     */
    CPIMIntValue(TPIMAttribute aAttributes,
                 TInt aIntegerValue);

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // Integer
    TInt iIntegerValue;
};

#endif // CPIMINTVALUE_H
// End of file
