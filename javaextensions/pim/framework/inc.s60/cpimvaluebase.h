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
* Description:  Class for holding PIM field values
 *
*/


#ifndef CPIMVALUEBASE_H
#define CPIMVALUEBASE_H

// INTERNAL INCLUDES
#include "pimtypes.h"

// EXTERNAL INCLUDES
#include <e32std.h>
#include <badesca.h>

/**
 * Class for holding PIM field values
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMValueBase) : public CBase
{
public:

    /**
     * Destructor
     */
    virtual ~CPIMValueBase();

public: // New methods

    /**
     * SetValue
     * Sets new integer value for this field
     * value holder. The old value will be overwritten
     *
     * @param aIntegerValue New value
     */
    virtual void SetIntValue(TInt aIntegerValue);

    /**
     * SetValue
     * Sets new boolean value for this field
     * value holder. The old value will be overwritten
     *
     * @param aBooleanValue New value
     */
    virtual void SetBooleanValue(TBool aBooleanValue);

    /**
     * SetValue
     * Sets new date value for this field
     * value holder. The old value will be overwritten
     *
     * @param aDateValue New value
     */
    virtual void SetDateValue(TPIMDate aDateValue);

    /**
     * SetValue
     * Sets new string value for this field
     * value holder. The old value will be overwritten
     * The ownership of the value is transferred to this class
     *
     * @param aStringValue New value
     */
    virtual void SetStringValue(const HBufC* aStringValue);

    /**
     * SetValue
     * Sets new string array value for this field
     * value holder. The old value will be overwritten
     * The ownership of the value is transferred to this class
     *
     * @param aStringArrayValue New value
     */
    virtual void SetStringArrayValue(
        const CDesCArray* aStringArrayValue);

    /**
     * SetValue
     * Sets new binary value for this field
     * value holder. The old value will be overwritten
     * The ownership of the value is transferred to this class
     *
     * @param aBinaryValue New value
     */
    virtual void SetBinaryValue(const CPIMByteArray* aBinaryValue);

    /**
     * Value. Returns an integer value of this value holder
     * @return Integer value
     */
    virtual TInt IntValue() const;

    /**
     * Value. Returns a boolean value of this value holder
     * @return Boolean value
     */
    virtual TBool BooleanValue() const;

    /**
     * Value. Returns a date value of this value holder
     * @return Date value
     */
    virtual TPIMDate DateValue() const;

    /**
     * Value. Returns a string value of this value holder
     * Ownership of the value is NOT transferred to the caller
     * @return String value
     */
    virtual const HBufC* StringValue() const;

    /**
     * Value. Returns a string array value of this value holder
     * Ownership of the value is NOT transferred to the caller
     * @return String array value
     */
    virtual const CDesCArray* StringArrayValue() const;

    /**
     * Value. Returns a binary value of this value holder
     * Ownership of the value is NOT transferred to the caller
     * @return Binary value
     */
    virtual const CPIMByteArray* BinaryValue() const;

    /**
     * Returns the number of elements in this value
     * @return Number of elements in this value
     */
    virtual TInt NumElements() const;

public: // New methods

    /**
     * SetInternalAttributes
     * Sets internal attributes of this value. These are not
     * accessible via PIM API but must not be lost when item
     * is converted from the native database
     *
     * @param aAttributes New internal attributes. Old ones will
     *        be overwriten.
     */
    void SetInternalAttributes(CArrayFix< TUid>* aInternalAttributes);

    /**
     * SetAttributes
     * Sets attributes of of this value. Used with PIM item
     * attribute handling
     *
     * @param aAttributes New attributes
     */
    void SetAttributes(TPIMAttribute aAttributes);

    /**
     * Returns the internal attributes of this value.
     * The returned value can be NULL which indicates that
     * there is no internal attributes in this field. NOTE the
     * ownership is NOT transferred to the caller
     *
     * @return Internal attributes of this value. NULL is returned
     *         if there is no internal attributes
     */
    const CArrayFix< TUid>& InternalAttributes() const;

    /**
     * @return Attributes of this value
     */
    const TPIMAttribute& Attributes() const;

    /**
     * SetLabels
     * Sets value labels. The array holds all labels including
     * string array field labels and normal field labels
     *
     * @param aValueLabels New labels for this value
     */
    void SetLabels(CDesCArray* aValueLabels);

    /**
     * Labels
     * Returns the labels of this value
     *
     * @return Labels of associated with this value
     */
    CDesCArray* Labels() const;

protected:

    /**
     * C++ default constructor
     */
    CPIMValueBase(TPIMAttribute aAttributes);

protected: // Data

    // PIM item attributes
    TPIMAttribute iAttributes;

    // Labels of this value. Owned
    CDesCArray* iLabelArray;

    // Internal attributes. Owned
    CArrayFix< TUid>* iInternalAttributes;
};

#endif // CPIMVALUEBASE_H
// End of file
