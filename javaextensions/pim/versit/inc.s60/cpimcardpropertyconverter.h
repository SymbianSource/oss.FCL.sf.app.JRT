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
* Description:  Converts properties vCard <-> PIM API
 *
*/


#ifndef CPIMCARDPROPERTYCONVERTER_H
#define CPIMCARDPROPERTYCONVERTER_H

//  INCLUDES
#include <e32base.h> // CBase
#include <badesca.h> // CDesCArray
#include "pimcontact.h" // TPIMContactField
#include "pimcommon.h" // TPIMAttribute
// CLASS DECLARATION
class CPIMItem;
class CPIMContactItem;
class CParserVCard;
class CParserParam;
class CPIMParserProperty;
class CParserProperty;
class CPIMContactValidator;
class CPIMVCardParserParamArray;
class CParserPropertyValue;

/**
 *  Converts Properties from versit objects to fields in PIM Contact items.
 */
NONSHARABLE_CLASS(CPIMCardPropertyConverter): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMCardPropertyConverter* NewL(
        const CPIMContactValidator& aValidator);

    /**
     * Destructor
     */
    virtual ~CPIMCardPropertyConverter();

public: // New functions

    /**
     * Inserts a Field from a PIM Contact Item to CParserVCard as
     * a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     *
     */
    void ConvertFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField);

    /**
     * Inserts a property from a vCard to a PIM Contact Item as a field.
     * If the property contains another vCard, that vCard is returned.
     * Otherwise NULL is returned.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     * @param aItemArray Array where agent field items are stored
     *
     * @return contained vCard, or NULL.
     */
    CParserVCard* ConvertPropertyL(
        const CParserProperty& aProperty,
        CPIMContactItem& aItem);

private:
    /**
     * C++ default constructor.
     */
    CPIMCardPropertyConverter(const CPIMContactValidator& aValidator);

    /**
     * Converts an agent property. The contained vCard is returned.
     *
     * @param aProperty property to convert
     *
     * @return contained vCard.
     */
    CParserVCard* ConvertAgentPropertyL(
        const CPIMParserProperty& aProperty);

    /**
     * Adds parameters "HOME" and "WORK" to an array of property
     * parameters if needed. Ownership of the new parameter objects
     * is transferred to the caller.
     *
     * @param aAttributes Checked, if contains EPIMContactAttrHome or
     *        EPIMContactAttrWork, the parameter is added.
     *
     * @param aParamArray Property parameter array to add the parameters
     *        to.
     */
    void AddParamHomeAndWorkL(TPIMAttribute aAttributes,
                              CPIMVCardParserParamArray& aParamArray);

    /**
     * Converts an address field from a PIM Contact Item to CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertAddressFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a name field from a PIM Contact Item to CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertNameFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts name and address arrays from a vCard to a PIM Contact Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertCDesCArrayPropertyL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Converts a date field from a PIM Contact Item to a CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertDateFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a birthday field from a vCard to a PIM Contact Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertDatePropertyL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Adds new parser parameter to the parameter array
     *
     * @param aArrayOfParams Parameter array
     * @param aParamName The name of the parameter which will be added
     */
    void AddParserParameterL(
        CPIMVCardParserParamArray& aArrayOfParams,
        const TPtrC8 aParamName);

    /**
     * Converts a string field from a PIM Contact Item to a CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertStringFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts a string field from a vCard to a PIM Contact Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertStringPropertyL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Converts a photo field from a PIM Contact Item to a CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertPhotoFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Inserts a PHOTO property from a vCard to a PIM Contact Item as a
     * binary field
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertPhotoPropertyL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Converts a class field from a PIM Contact Item to a CParserVCard.
     * The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertClassFieldL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Inserts a CLASS property from a vCard to a PIM Contact Item as a
     * integer field
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertClassPropertyL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Converts organisational fields from a PIM Contact Item to a
     * CParserVCard. The field is stored in the parser as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertOrganisationalFieldsL(
        const CPIMItem& aItem,
        CParserVCard& aParser,
        TPIMContactField aField,
        TInt aIndex);

    /**
     * Converts vCard name readings to a PIM item name array elements
     *
     * @param aProperty vCard property value
     * @param aItem Contact List item
     * @param aNameArrayElementIndex Index of this element
     */
    void ConvertNameReadingFieldL(
        const CPIMParserProperty& aProperty,
        CPIMContactItem& aItem);

    /**
     * Adds a new property to the vCard parser
     * Note that property value and array of params are popped
     * from the cleanup stack inside this function
     *
     * @param aPropertyValue Property value for the vCard property
     *        This needs not to be pushed to the cleanup stack before
     *        this function call since it is done straight from the start
     *        of this function
     * @param aPropertyName Name of the property
     * @param aArrayOfParams Parameter array. This can be NULL and must be
     *        pushed to the cleanup stack before this function call
     * @param aParser VCard parser into which the new property is pushed
     */
    void AddPropertyToParserL(
        CParserPropertyValue* aPropertyValue,
        const TDesC8& aPropertyName,
        CPIMVCardParserParamArray* aArrayOfParams,
        CParserVCard& aParser);

private: // data

    // Validates contacts
    const CPIMContactValidator& iContactValidator;

};

#endif // CPIMCARDPROPERTYCONVERTER_H

// End of file
