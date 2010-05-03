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
* Description:  Specialized string value class
 *
*/


#ifndef CPIMSTRINGVALUE_H
#define CPIMSTRINGVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized string value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMStringValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMStringValue* NewL(TPIMAttribute aAttributes,
    HBufC* aStringValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMStringValue* NewLC(TPIMAttribute aAttributes,
                                  HBufC* aStringValue);

    /**
     * Destructor
     */
    virtual ~CPIMStringValue();

public: // Functions from CPIMValue

    /**
     * SetValue
     * Sets new integer value for this field value holder
     * The old value will be overwritten and the ownership
     * is transferred to this class
     *
     * @param aIntegerValue New value
     */
    virtual void SetStringValue(const HBufC* aStringValue);

    /**
     * Value. Returns a string value of this value holder
     * NOTE: The ownership is NOT transferred to the caller
     * @return String value
     */
    virtual const HBufC* StringValue() const;

private:

    /**
     * C++ constructor
     */
    inline CPIMStringValue(TPIMAttribute aAttributes,
                           HBufC* aStringValue);

private: // Data

    // String value. Owned
    const HBufC* iStringValue;
};

#endif // CPIMSTRINGVALUE_H
