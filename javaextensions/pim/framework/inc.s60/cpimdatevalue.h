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
* Description:  Specialized date value class
 *
*/


#ifndef CPIMDATEVALUE_H
#define CPIMDATEVALUE_H

// INTERNAL INCLUDES
#include "cpimvaluebase.h"

// EXTERNAL INCLUDES
#include <e32std.h>

/**
 * Specialized date value class
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMDateValue) : public CPIMValueBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMDateValue* NewL(TPIMAttribute aAttributes,
    TPIMDate aDateValue);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMDateValue* NewLC(TPIMAttribute aAttributes,
                                TPIMDate aDateValue);

    /**
     * Destructor
     */
    virtual ~CPIMDateValue();

public: // Functions from CPIMValue

    /**
     * SetValue
     * Sets new date value for this field
     * value holder. The old value will be overwritten
     *
     * @param aDateValue New value
     */
    virtual void SetDateValue(TPIMDate aDateValue);

    /**
     * Value
     * Returns a reference to the value of this field
     * value holder. The value cannot be modified
     *
     * @return Date value
     */
    virtual TPIMDate DateValue() const;

private:

    /**
     * C++ constructor
     */
    inline CPIMDateValue(TPIMAttribute aAttributes,
                         TPIMDate aDateValue);

private: // Data

    // Date value
    TPIMDate iDateValue;
};

#endif // CPIMDATEVALUE_H
// End of file
