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
* Description:  Converts event properties PIM API <-> versit
 *
*/


#ifndef CPIMEVENTPROPERTYCONVERTER_H
#define CPIMEVENTPROPERTYCONVERTER_H

// INTERANL INCLUDES
#include "pimevent.h"
#include "pimcommon.h"

// EXTERNAL INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class CParserProperty;
class CPIMParserProperty;
class CPIMItem;
class CPIMEventItem;
class CParserVCalEntity;
class MPIMRepeatRuleData;
class TVersitDateTime;
class CVersitRecurrence;
class CParserPropertyValue;

// CLASS DECLARATION

/**
 *  Converts Properties from versit objects to fields in PIM Event items.
 */
NONSHARABLE_CLASS(CPIMEventPropertyConverter): public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMEventPropertyConverter* NewL();

    /**
     * Destructor
     */
    virtual ~CPIMEventPropertyConverter();

public: // New functions

    /**
     * Inserts a property from a vEvent to a PIM Event.
     * If the property is DALARM, the date is not stored in the item,
     * but in the aAlarm reference.
     *
     * @param aProperty property to insert
     * @param aItem item to insert into
     * @param aAlarm value of DALARM property is stored here
     */
    void ConvertPropertyL(const CParserProperty& aProperty,
                          CPIMEventItem& aItem,
                          TPIMDate& aAlarm);

    /**
     * Inserts a Field from a PIM Event Item to CParserVCalEntity.
     * The field is insterted as a property.
     *
     * @param aItem item to read the field from
     * @param aParser parser to insert the property to.
     * @param aField field to convert
     *
     */
    void ConvertEventFieldL(const CPIMItem& aItem,
                            CParserVCalEntity& aParser,
                            TPIMEventField aField);

    /**
     * Inserts a repeat rule to a parser.
     *
     * @param aRepeat Repeat rule to insert
     * @param aParser Parser to insert into
     */
    void ConvertRepeatRuleL(const MPIMRepeatRuleData* aRepeat,
                            CParserVCalEntity& aParser);

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
    CPIMEventPropertyConverter();

    /**
     * Converts a date property from a vEvent to a PIM Event Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertDatePropertyL(const CPIMParserProperty& aProperty,
                              CPIMEventItem& aItem);

    /**
     * Converts an alarm property from a vEvent to a PIM Event Item.
     * The date is not stored in the item,
     * but in the aAlarm reference.
     *
     * @param aProperty property to convert
     * @param aAlarm the value is stored here
     *
     */
    void ConvertAlarmPropertyL(const CPIMParserProperty& aProperty,
                               TPIMDate& aAlarm);

    /**
     * Converts a string field from a vTodo to a PIM Event Item.
     *
     * @param aProperty property to convert
     * @param aItem item to write the field to
     *
     */
    void ConvertStringPropertyL(const CPIMParserProperty& aProperty,
                                CPIMEventItem& aItem);

    /**
     * Convert RRULE property from a vCalendar to a PIM Event Item.
     * The repeat rule is stored in the repeat rule of the event item.
     * @param aProperty property to convert
     * @param aItem item to write the dates to
     */
    void ConvertRepeatRulePropertyL(const CPIMParserProperty& aProperty,
                                    CPIMEventItem& aItem);

    /**
     * Convert EXDATE property from a vCalendar to a PIM Event Item.
     * The exception dates are stored in the item as repeat rule
     * exception dates.
     * @param aProperty property to convert
     * @param aItem item to write the dates to
     *
     */
    void ConvertExDatePropertyL(const CPIMParserProperty& aProperty,
                                CPIMEventItem& aItem);

    /**
     * Convert CLASS property from a vCalendar to a PIM Event Item.
     * (This is not done among other string/HBufC properties, because
     * the resulting data is not string, but int.)
     *
     * @param aProperty property to convert
     * @param aItem item to write the dates to
     */
    void ConvertClassPropertyL(const CPIMParserProperty& aProperty,
                               CPIMEventItem& aItem);

    /**
     * Inserts exception dates to a parser.
     *
     * @param aDates Exception dates to add
     * @param aParser Parser to insert into
     */
    void ConvertExceptionDatesL(const CArrayFix<TPIMDate>& aDates,
                                CParserVCalEntity& aParser);

    /**
     * Converts a date field from a PIM Event Item to
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
                           TPIMEventField aField,
                           TInt aIndex);

    /**
     * Converts an alarm field from a PIM Event Item to
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
    void ConvertAlarmFieldL(const CPIMItem& aItem,
                            CParserVCalEntity& aParser,
                            TPIMEventField aField,
                            TInt aIndex);

    /**
     * Converts a string field from a PIM Event Item to
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
                             TPIMEventField aField,
                             TInt aIndex);

    /**
     * Coverts the Event.CLASS field from a PIM Event item to
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
        TPIMEventField aField,
        TInt aIndex);

    /**
     * Gets the fields from a repeat rule. The fields are stored in the
     * given pointers.
     *
     * @param aRepeat Fields are read from this repeat rule.
     * @param aInterval interval is stored here
     * @param aFrequency frequency is stored here
     * @param aVersitEndDate end date is stored here
     */
    void GetRepeatRuleFieldsL(const MPIMRepeatRuleData* aRepeat,
                              TInt* aInterval,
                              TInt* aFrequency,
                              TVersitDateTime** aVersitEndDate);

    /**
     * Creates a versit recurrence according to given parameters.
     * If aVersitEndDate is given, the created recurrence takes its
     * ownership.
     *
     * @param aFrequency frequency of the recurrence
     * @param aInterval interval of the recurrence
     * @param aVersitEndDate end date (may be NULL)
     *
     * @return Versit recurrence
     */
    CVersitRecurrence* CreateRecurrenceL(
        TInt aFrequency,
        TInt aInterval,
        TVersitDateTime* aVersitEndDate);

};

#endif // CPIMEVENTPROPERTYCONVERTER_H
// End of file
