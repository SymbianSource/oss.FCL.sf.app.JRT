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
* Description:  Class for holding PIM field data
 *
*/


#ifndef TPIMFIELDDATA_H
#define TPIMFIELDDATA_H

// INTERNAL INCLUDES
#include "pimtypes.h"
#include "pimcommon.h"

// EXTERNAL INCLUDES
#include <e32std.h>
#include <badesca.h>

/**
 * Class for holding PIM field values;
 */
NONSHARABLE_CLASS(TPIMFieldData)
{
public:

    // -- Constructors --

    /**
     * Default C++ constructor. Not applicable for this class
     * but still needed to represent
     */
    inline TPIMFieldData() :
            iDateValue(Time::NullTTime()),
            iStringValue(NULL),
            iStringArrayValue(NULL),
            iBinaryValue(NULL)
    {}

    /**
     * Constructor for integer and boolean types field data.
     * This should be used only with integer and boolean fields.
     * Getters panic if wrong data type is used with this object.
     * NOTE: This constructor is exception and takes field data
     * type as a paramater because integer and boolean values
     * cannot be overloaded. See TBool and TInt
     *
     * @param aField        PIM field type
     * @param aDataType     Data type of this field
     * @param aAttributes   Attributes of this field
     * @param aValue        Value of this field
     */
    inline TPIMFieldData(TPIMField aField, TPIMFieldDataType aDataType,
                         TPIMAttribute aAttributes, TInt aValue) :
            iField(aField), iDataType(aDataType), iAttributes(aAttributes), iIndex(
                0), iIntegerBooleanValue(aValue)
    {
    }

    /**
     * Constructor for integer type field data. This should be
     * used only with date fields. Getters panic if wrong
     * data type is used with this object
     *
     * @param aField        PIM field type
     * @param aAttributes   Attributes of this field
     * @param aValue        Value of this field
     */
    inline TPIMFieldData(TPIMField aField, TPIMAttribute aAttributes,
                         TPIMDate aValue) :
            iField(aField), iDataType(EPIMFieldDate), iAttributes(aAttributes),
            iIndex(0), iDateValue(aValue)
    {
    }

    /**
     * Constructor for integer type field data. This should be
     * used only with string fields. Getters panic if wrong
     * data type is used with this object
     *
     * @param aField        PIM field type
     * @param aAttributes   Attributes of this field
     * @param aValue        Value of this field
     */
    inline TPIMFieldData(TPIMField aField, TPIMAttribute aAttributes,
                         const HBufC* aValue) :
            iField(aField), iDataType(EPIMFieldString), iAttributes(aAttributes),
            iIndex(0), iStringValue(aValue)
    {
    }

    /**
     * Constructor for integer type field data. This should be
     * used only with string array fields. Getters panic if wrong
     * data type is used with this object
     *
     * @param aField        PIM field type
     * @param aAttributes   Attributes of this field
     * @param aValue        Value of this field
     */
    inline TPIMFieldData(TPIMField aField, TPIMAttribute aAttributes,
                         const CDesCArray* aValue) :
            iField(aField), iDataType(EPIMFieldStringArray), iAttributes(
                aAttributes), iIndex(0), iStringArrayValue(aValue)
    {
    }

    /**
     * Constructor for integer type field data. This should be
     * used only with binary fields. Getters panic if wrong
     * data type is used with this object
     *
     * @param aField        PIM field type
     * @param aAttributes   Attributes of this field
     * @param aValue        Value of this field
     */
    inline TPIMFieldData(TPIMField aField, TPIMAttribute aAttributes,
                         const CPIMByteArray* aValue) :
            iField(aField), iDataType(EPIMFieldBinary), iAttributes(aAttributes),
            iIndex(0), iBinaryValue(aValue)
    {
    }
    ;

public: // New functions

    /**
     * @return Field
     */
    TPIMField Field() const;

    /**
     * @return Field's data type
     */
    TPIMFieldDataType FieldDataType() const;

    /**
     * @return Index of this field
     */
    TInt Index() const;
    void SetIndex(TInt aIndex);

    /**
     * @return Attributes of this field
     */
    TPIMAttribute Attributes() const;

    // -- Specific getters --

    /**
     * @return Interger value if this field is an integer field.
     *         If not, a panic is raised
     */
    TInt IntegerValue() const;

    /**
     * @return Boolean value if this field is a boolean field.
     *         If not, a panic is raised
     */
    TBool BooleanValue() const;

    /**
     * @return Date value if this field is a date field.
     *         If not, a panic is raised
     */
    TPIMDate DateValue() const;

    /**
     * @return String value if this field is string field.
     *         If not, a panic is raised
     */
    const TDesC& StringValue() const;

    /**
     * @return String array value if this field is a string array
     *         field. If not, a panic is raised
     */
    const CDesCArray& StringArrayValue() const;

    /**
     * @return Binary value if this field is a binary field.
     *         If not, a panic is raised
     */
    const CPIMByteArray& BinaryValue() const;

private: // Data

    // Holds field info
    TPIMField iField;
    TPIMFieldDataType iDataType;

    // Holds attributes
    TPIMAttribute iAttributes;

    // Holds different types of values. Pointer are not owned
    TInt iIndex;
    TInt iIntegerBooleanValue;
    TPIMDate iDateValue;

    // Pointer values
    const HBufC* iStringValue;
    const CDesCArray* iStringArrayValue;
    const CPIMByteArray* iBinaryValue;

private: // Friend classes

    /**
     * Friend classes are not encouraged to use but in this
     * case it is needed, because CPIMField class uses this
     * class to take and store the pointer values and this
     * helps the implementation. Value accessors can only return
     * references to the actual value.
     */
    friend class CPIMField;
}
;

#endif // TPIMFIELDDATA_H
// End of file
