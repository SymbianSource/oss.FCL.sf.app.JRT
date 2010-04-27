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
* Description:  Specialized binary value class
 *
*/


#ifndef CPIMBINARYVALUE_H
#define CPIMBINARYVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized binary value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMBinaryValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMBinaryValue* NewL(TPIMAttribute aAttributes,
    CPIMByteArray* aBinaryValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMBinaryValue* NewLC(TPIMAttribute aAttributes,
                                  CPIMByteArray* aBinaryValue);

    /**
     * Destructor
     */
    virtual ~CPIMBinaryValue();

public: // Functions from CPIMValue

    /**
     * SetValue
     * Sets new binary value for this field
     * value holder. The old value will be overwritten
     *
     * @param aBinaryValue New value
     */
    virtual void SetBinaryValue(const CPIMByteArray* aBinaryValue);

    /**
     * Value
     * Returns a reference to the value of this field
     * value holder. The value cannot be modified
     *
     * @return Binary value
     */
    virtual const CPIMByteArray* BinaryValue() const;

private:

    /**
     * C++ constructor
     */
    inline CPIMBinaryValue(TPIMAttribute aAttributes,
                           CPIMByteArray* aBinaryValue);

private: // Data

    // Binary value. Owned
    const CPIMByteArray* iBinaryValue;
};

#endif // CPIMBINARYVALUE_H
// End of file
