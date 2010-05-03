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
* Description:  Specialized string array value class
 *
*/


#ifndef CPIMSTRINGARRAYVALUE_H
#define CPIMSTRINGARRAYVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized string array value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMStringArrayValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMStringArrayValue* NewL(TPIMAttribute aAttributes,
    CDesCArray* aStringArrayValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMStringArrayValue* NewLC(TPIMAttribute aAttributes,
                                       CDesCArray* aStringArrayValue);

    /**
     * Destructor
     */
    virtual ~CPIMStringArrayValue();

public: // Functions from CPIMValueBase

    /**
     * SetValue
     * Sets new string array value for this field
     * value holder. The old value will be overwritten
     *
     * @param aStringArrayValue New value
     */
    virtual void SetStringArrayValue(
        const CDesCArray* aStringArrayValue);

    /**
     * Value
     * Returns a reference to the value of this field
     * value holder. The value cannot be modified
     *
     * @return String array value
     */
    virtual const CDesCArray* StringArrayValue() const;

    /**
     * Returns the number of elements in this value
     * @return Number of elements in this value
     */
    virtual TInt NumElements() const;

private:

    /**
     * Default C++ constructor
     */
    inline CPIMStringArrayValue(TPIMAttribute aAttributes,
                                CDesCArray* aStringArrayValue);

private: // Data

    // String array value. Owned
    const CDesCArray* iStringArrayValue;
};

#endif // CPIMSTRINGARRAYVALUE_H
// End of file
