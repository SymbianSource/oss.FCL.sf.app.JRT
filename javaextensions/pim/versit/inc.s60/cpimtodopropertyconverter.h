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
* Description:  Converts properties vTodo <-> PIM API
 *
*/


#ifndef CPIMTODOPROPERTYCONVERTER_H
#define CPIMTODOPROPERTYCONVERTER_H

// INTERNAL INCLUDES
#include "pimtodo.h"
#include "pimcommon.h"

// EXTERNAL INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CParserProperty;
class CPIMParserProperty;
class CPIMItem;
class CPIMToDoItem;
class CParserVCalEntity;
class CParserPropertyValue;

/**
 *  Converts Properties from versit objects to fields in PIM ToDo items.
 */
NONSHARABLE_CLASS(CPIMToDoPropertyConverter): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMToDoPropertyConverter* NewL();

    /**
     * Destructor
     */
    virtual ~CPIMToDoPropertyConverter();

public: // New functions

    /**
     * Inserts a property from a vToDo to a PIM ToDo.
     *
     * @param aProperty property to insert
     * @param aItem item to insert into
     * @param aAlarm value of DALARM property is stored here
     */
    void ConvertPropertyL(const CParserProperty& aProperty,
                          CPIMToDoItem& aItem,
                          TPIMDate& aAlarm);

    /**
     * Inserts a Field from a PIM ToDo Item to CParserVCalEntity.
     * The field is insterted as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     *
     */
    void ConvertToDoFieldL(const CPIMItem& aItem,
                           CParserVCalEntity& aParser,
                           TPIMToDoField aField);

    /**
     * Adds a new property to the vCalendar parser
     * Note that property value and array of params are popped
     * from the cleanup stack inside this function
     *
     * @param aPropertyValue Property value for the vCalendar property
     *        This needs not to be pushed to the cleanup stack before
     *        this function call since it is done straight from the start
     *        of this function
     * @param aPropertyName Name of the property
     * @param aParser vCalendar parser into which the new property is pushed
     */
    void AddPropertyToParserL(
        CParserPropertyValue* aPropertyValue,
        const TDesC8& aPropertyName,
        CParserVCalEntity& aParser);

private:
    /**
     * C++ default constructor.
     */
    CPIMToDoPropertyConverter();

    /**
     * Converts a date property from a vToDo to a PIM ToDo Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertDatePropertyL(const CPIMParserProperty& aProperty,
                              CPIMToDoItem& aItem);

    /**
     * Converts a string field from a vTodo to a PIM ToDo Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertStringPropertyL(const CPIMParserProperty& aProperty,
                                CPIMToDoItem& aItem);

    /**
     * Converts a priority property from a vTodo to a PIM ToDo Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     */
    void ConvertPriorityPropertyL(const CPIMParserProperty& aProperty,
                                  CPIMToDoItem& aItem);

    /**
     * Converts a date field from a PIM ToDo Item to
     * a CParserVCalEntity.
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
    void ConvertDateFieldL(const CPIMItem& aItem,
                           CParserVCalEntity& aParser,
                           TPIMToDoField aField,
                           TInt aIndex);

    /**
     * Converts a string field from a PIM ToDo Item to
     * a CParserVCalEntity.
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
    void ConvertStringFieldL(const CPIMItem& aItem,
                             CParserVCalEntity& aParser,
                             TPIMToDoField aField,
                             TInt aIndex);

    /**
     * Converts an int field from a PIM ToDo Item to
     * a CParserVCalEntity.
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
    void ConvertIntFieldL(const CPIMItem& aItem,
                          CParserVCalEntity& aParser,
                          TPIMToDoField aField,
                          TInt aIndex);

    /**
     * Converts the completed field from a PIM ToDo Item to a
     * CParserVCalEntity. The field is stored in the parser as a
     * COMPLETED (completion date) property. Current date is used
     * as a date value.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     * @param aIndex index to the field to convert
     *
     * @par Leaving:
     * This method leaves on an error with system wide error codes.
     */
    void ConvertCompletedFieldL(const CPIMItem& aItem,
                                CParserVCalEntity& aParser,
                                TPIMToDoField aField,
                                TInt aIndex);

    /**
     * Coverts the ToDo.CLASS field from a PIM Event item to
     * a CParserVCalEntity.
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
        CParserVCalEntity& aParser,
        TPIMToDoField aField,
        TInt aIndex);

    /**
     * Convert CLASS property from a vCalendar to a PIM ToDo Item.
     * (This is not done among other string/HBufC properties, because
     * the resulting data is not string, but int.)
     *
     * @param aProperty property to convert
     * @param aItem item to write the dates to
     */
    void ConvertClassPropertyL(
        const CPIMParserProperty& aProperty,
        CPIMToDoItem& aItem);

    /**
     * Converts an alarm field from a PIM ToDo Item to
     * a CParserVCalEntity.
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
    void ConvertAlarmFieldL(
        const CPIMItem& aItem,
        CParserVCalEntity& aParser,
        TPIMToDoField aField,
        TInt aIndex);

    /**
     * Convert Alarm property from a vCalendar to a PIM ToDo Item.
     *
     * @param aProperty property to convert
     * @param aAlarm value of DALARM property is stored here
     */
    void ConvertAlarmPropertyL(
        const CPIMParserProperty& aProperty,
        TPIMDate& aAlarm);

};

#endif // CPIMTODOPROPERTYCONVERTER_H

// End of file
