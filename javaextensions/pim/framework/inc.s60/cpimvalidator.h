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


#ifndef CPIMVALIDATOR_H
#define CPIMVALIDATOR_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "pimcommon.h"

/**
 *  PIM validator abstract base class.
 *  Provides information about valid fields, attributes and values.
 *
 *  @par Notes:
 *  @li The value validator methods do not take any behavioral aspects into
 *      account. Checking is based on predefined values only (if applicable)
 *      and additional dynamic checking might be necessary.
 *
 */
NONSHARABLE_CLASS(CPIMValidator): public CBase
{
public: // Destructor

    /**
     * Destructor.
     */
    virtual ~CPIMValidator();

public: // New functions

    /**
     * Provides an array of valid fields.
     */
    virtual const CArrayFix< TPIMFieldAndType>& ValidFields() const;

    /**
     * Determines whether given field is valid for this list type.
     */
    virtual TBool IsValidField(const TPIMField& aField) const;

    /**
     * Provides data type of given field.
     * Can be also used for determining wheter a field is valid for
     * the derived class.
     *
     * @param aField A field.
     *
     * @return Data type of \a aField, or \ref EPIMFieldInvalid if
     *         \a aField is not valid for the derived class.
     */
    virtual TPIMFieldDataType FieldDataType(
        const TPIMField& aField) const;

    /**
     * Provides valid contact attributes, combined.
     */
    virtual TPIMAttribute ValidAttributes() const;

    /**
     * Provides number of elements for given string array field.
     * Default implementation leaves with \c KErrArgument.
     */
    virtual TInt NumElementsL(
        const TPIMField& /*aStringArrayField*/) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidBooleanValue(
        const TPIMField& aField,
        const TBool& aValue) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidDateValue(
        const TPIMField& aField,
        const TPIMDate& aValue) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidIntegerValue(
        const TPIMField& aField,
        const TInt& aValue) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidBinaryValue(
        const TPIMField& aField,
        const CPIMByteArray& aValue) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidStringValue(
        const TPIMField& aField,
        const HBufC& aValue) const;

    /**
     * Determines wheter given value is valid for given field.
     * @return Default implementation returns always \c ETrue.
     */
    virtual TBool IsValidStringArrayValue(
        const TPIMField& aField,
        const CDesCArray& aValue) const;

protected: // Constructors and destructor

    /**
     * Symbian 2nd phase constructor. Must be called in the
     * ConstructL of derived classes.
     */
    void ConstructL();

    /**
     * C++ constructor.
     * @param aValidAttributes Valid attributes from the derived class,
     *        combined.
     */
    CPIMValidator(const TPIMAttribute& aValidAttributes);

protected: // Data

    /** Valid fields and their types. Owned. */
    CArrayFix< TPIMFieldAndType>* iValidFields;

    /** Valid attributes, combined. */
    TPIMAttribute iValidAttributes;
};

#endif // CPIMVALIDATOR_H
// End of File
