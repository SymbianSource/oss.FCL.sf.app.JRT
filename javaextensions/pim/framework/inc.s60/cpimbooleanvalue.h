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
* Description:  Specialized boolean value class
 *
*/


#ifndef CPIMBOOLEANVALUE_H
#define CPIMBOOLEANVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized boolean value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMBooleanValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMBooleanValue* NewL(TPIMAttribute aAttributes,
    TBool aBooleanValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMBooleanValue* NewLC(TPIMAttribute aAttributes,
                                   TBool aBooleanValue);

    /**
     * Destructor
     */
    virtual ~CPIMBooleanValue();

public: // Functions from CPIMValue

    /**
     * SetValue
     * Sets new boolean value for this field
     * value holder. The old value will be overwritten
     *
     * @param aBooleanValue New value
     */
    virtual void SetBooleanValue(TBool aBooleanValue);

    /**
     * Value
     * Returns the value in this boolean value holder
     * The value cannot be modified
     *
     * @return Boolean value
     */
    virtual TBool BooleanValue() const;

private:

    /**
     * C++ constructor
     */
    inline CPIMBooleanValue(TPIMAttribute aAttributes,
                            TBool aBooleanValue);

private: // Data

    // Boolean value
    TBool iBooleanValue;
};

#endif // CPIMBOOLEANVALUE_H
// End of file
