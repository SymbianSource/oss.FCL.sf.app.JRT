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
* Description:  Converts properties vTodo <-> PIM API*
*/


// CLASS HEADER
#include "cpimtodopropertyconverter.h"

// INTERNAL INCLUDES
#include "pimcommon.h" // tpimfield
#include "pimpanics.h"
#include "pimversit.h"
#include "cpimitem.h"
#include "cpimtodoitem.h"
#include "cpimparserproperty.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <vcal.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMToDoPropertyConverter* CPIMToDoPropertyConverter::NewL()
{
    JELOG2(EPim);
    CPIMToDoPropertyConverter* self = new(ELeave) CPIMToDoPropertyConverter();
    return self;
}

// Destructor
CPIMToDoPropertyConverter::~CPIMToDoPropertyConverter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertPropertyL
// Inserts a proprety from a vTodo to a PIM ToDo Item as a field
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertPropertyL(
    const CParserProperty& aProperty, // property to convert
    CPIMToDoItem& aItem, // item to insert the field to
    TPIMDate& aAlarm)
{
    JELOG2(EPim);
    TUid valueTypeUid = aProperty.Uid();
    TInt valueType = valueTypeUid.iUid;

    // The following, rather ugly, cast makes it possible for us to access
    // the protected iArrayOfParams field.
    const CPIMParserProperty& property =
        static_cast<const CPIMParserProperty&>(aProperty);

    if (aProperty.Name().CompareF(KVersitTokenCLASS) == 0)
    {
        ConvertClassPropertyL(property, aItem);
    }
    else
    {
        switch (valueType)
        {
        case KVersitPropertyDateTimeUid:
        {
            // due, completed
            ConvertDatePropertyL(property, aItem);
            break;
        }
        case KVersitPropertyHBufCUid:
        {
            // summary
            ConvertStringPropertyL(property, aItem);
            break;
        }
        case KVersitPropertyIntUid:
        {
            // priority
            ConvertPriorityPropertyL(property, aItem);
            break;
        }
        case KVCalPropertyAlarmUid:
        {
            ConvertAlarmPropertyL(property, aAlarm);
            break;
        }
        default:
        {
            // don't support, don't care
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertToDoFieldL
// Inserts a Field from a PIM ToDo Item to CParserVCard as a property.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertToDoFieldL(const CPIMItem& aItem, // item to read the field from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMToDoField aField) // field to convert
{
    JELOG2(EPim);
    TInt valueCount = aItem.CountValuesL(aField);
    for (TInt i = 0; i < valueCount; i++)
    {
        switch (aField)
        {
        case EPIMToDoDue:
        case EPIMToDoCompletionDate:
        case EPIMToDoRevision:
        {
            ConvertDateFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMToDoSummary:
        case EPIMToDoNote:
            // UID is required by VFX. Do not fix even though this is wrong
        case EPIMToDoUid:
        {
            ConvertStringFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMToDoPriority:
        {
            ConvertIntFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMToDoCompleted:
        {
            ConvertCompletedFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMToDoClass:
        {
            ConvertClassFieldL(aItem, aParser, aField, i);
            break;
        }
        case EPIMToDoExtAlarm:
        {
            ConvertAlarmFieldL(aItem, aParser, aField, i);
            break;
        }
        default:
        {
            __ASSERT_DEBUG(EFalse, User::Panic(KPIMPanicCategory,
                                               EPIMPanicUnsupportedField));
            return;
        }
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::CPIMCalendarConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMToDoPropertyConverter::CPIMToDoPropertyConverter()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertDatePropertyL
// Converts a date field from a vCard to a PIM ToDo Item
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertDatePropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMToDoItem& aItem) // item to convert to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchToDoField();
    // Do not convert unrecognized fields
    if (field == -1)
    {
        return;
    }

    CParserPropertyValueDate* propertyValue =
        static_cast<CParserPropertyValueDate*>(aProperty.Value());
    const TVersitDateTime* value = propertyValue->Value();

    TPIMDate date(value->iDateTime);
    if (aItem.CountValuesL(field) > 0)
    {
        aItem.SetDateL(field, 0, KPIMAttrNone, date);
    }
    else
    {
        aItem.AddDateL(field, KPIMAttrNone, date);
    }
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertStringPropertyL
// Converts a string property from a vTodo to a PIM ToDo Item
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertStringPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMToDoItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchToDoField();
    // Do not convert unrecognized fields
    if (field == -1)
    {
        return;
    }

    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());
    // ToDo completed is a boolean field
    if (field == EPIMToDoCompleted)
    {
        TPtrC completed = propertyValue->Value();
        if (completed.CompareF(KPIMStatusCompleted) == 0)
        {
            aItem.AddBooleanL(EPIMToDoCompleted, KPIMAttrNone, ETrue);
        }
        else if (completed.CompareF(KPIMStatusUncompleted) == 0)
        {
            aItem.AddBooleanL(EPIMToDoCompleted, KPIMAttrNone, EFalse);
        }
        return;
    }
    HBufC* value = propertyValue->TakeValueOwnership();
    CleanupStack::PushL(value);
    aItem.AddStringL(field, KPIMAttrNone, value);
    CleanupStack::Pop(value); // owned by aItem
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertPriorityPropertyL
// Converts a priority property from a vTodo to a PIM ToDo Item
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertPriorityPropertyL(
    const CPIMParserProperty& aProperty, // property to convert
    CPIMToDoItem& aItem) // item to insert the field to
{
    JELOG2(EPim);
    TPIMField field = aProperty.MatchToDoField();
    // Do not convert unrecognized fields
    if (field != EPIMToDoPriority)
    {
        return;
    }

    CParserPropertyValueInt* propertyValue =
        static_cast<CParserPropertyValueInt*>(aProperty.Value());
    TInt value = propertyValue->Value();
    TPIMAttribute attributes = KPIMAttrNone;
    if (aItem.CountValuesL(field) > 0)
    {
        aItem.SetIntL(field, 0, attributes, value);
    }
    else
    {
        aItem.addInt(field, attributes, value);
    }
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertDateFieldL
// Converts a date field from a PIM ToDo Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertDateFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMToDoField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    // Panic on debug builds if the field is incorrect
    __ASSERT_DEBUG(aField == EPIMToDoDue || aField == EPIMToDoCompletionDate
                   || aField == EPIMToDoRevision, User::Invariant());

    // Generate date
    const TPIMDate date = aItem.GetDateL(aField, aIndex);
    TDateTime dateTime = date.DateTime();
    TVersitDateTime* versitDateTime =
        new(ELeave) TVersitDateTime(dateTime, TVersitDateTime::EIsUTC);
    CleanupDeletePushL(versitDateTime);

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueDateTime(versitDateTime);
    CleanupStack::Pop(versitDateTime); // versitDateTime is now owned by propertyValue
    TPtrC8 propertyName;

    if (aField == EPIMToDoDue)
    {
        propertyName.Set(KVersitTokenDUE);
    }
    else if (aField == EPIMToDoCompletionDate)
    {
        propertyName.Set(KVersitTokenCOMPLETED);
    }
    else if (aField == EPIMToDoRevision)
    {
        propertyName.Set(KVersitTokenLASTMODIFIED);
    }
    AddPropertyToParserL(propertyValue, propertyName, aParser);
    // Needed cleanup stack items are pushed and popped within the function
}
// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertStringFieldL
// Converts a string field from a PIM ToDo Item to vCard
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertStringFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to write to
        TPIMToDoField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    // Panic on debug builds if the field is incorrect
    __ASSERT_DEBUG(aField == EPIMToDoSummary || aField == EPIMToDoNote
                   || aField == EPIMToDoUid, User::Invariant());
    const TDesC& string = aItem.GetStringL(aField, aIndex);
    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              string);

    TPtrC8 propertyName;
    if (aField == EPIMToDoSummary)
    {
        propertyName.Set(KVersitTokenSUMMARY);
    }
    else if (aField == EPIMToDoNote)
    {
        propertyName.Set(KVersitTokenDESCRIPTION);
    }
    else if (aField == EPIMToDoUid)
    {
        propertyName.Set(KVersitTokenUID);
    }
    AddPropertyToParserL(propertyValue, propertyName, aParser);
    // Needed cleanup stack items are pushed and popped within the function
}
// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertIntFieldL
// Converts a string field from a PIM ToDo Item to vCard
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertIntFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to write to
        TPIMToDoField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    __ASSERT_DEBUG(EPIMToDoPriority == aField, User::Panic(KPIMPanicCategory,
                   EPIMPanicUnsupportedIntField));
    TInt priority = aItem.getInt(aField, aIndex);
    // Requirement specification 4.1.7, table 8 defines the following
    // mapping
    if (priority == EPIMToDoPriorityMinValue) // 0
    {
        return;
    }
    else if ((EPIMToDoPriorityHigh <= priority) && (priority
             < EPIMToDoPriorityMedium))
    {
        priority = EPIMToDoPriorityHigh;
    }
    else if ((EPIMToDoPriorityMedium <= priority) && (priority
             < EPIMToDoPriorityLow))
    {
        priority = EPIMToDoPriorityMedium;
    }
    else if ((EPIMToDoPriorityLow <= priority) && (priority
             <= EPIMToDoPriorityMaxValue))
    {
        priority = EPIMToDoPriorityLow;
    }
    else
    {
        User::Leave(KErrTooBig);
    }

    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueInt(priority);
    AddPropertyToParserL(propertyValue, KVersitTokenPRIORITY(), aParser);
    // Needed cleanup stack items are pushed and popped within the function
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertCompletedFieldL
// Converts a completed field from a PIM ToDo Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertCompletedFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMToDoField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aField == EPIMToDoCompleted, User::Panic(KPIMPanicCategory,
                   EPIMPanicUnsupportedField));
    TBool completed = aItem.GetBooleanL(aField, aIndex);
    TPtrC statusString(KPIMStatusUncompleted);
    // Completed should be converted as STATUS field in the vCalendar
    // false = STATUS:NEEDS ACTION, true = STATUS:COMPLETED. see vCalendar
    // standard specification
    if (completed)
    {
        statusString.Set(KPIMStatusCompleted);
    }

    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              statusString);
    AddPropertyToParserL(propertyValue, KVersitTokenSTATUS(), aParser);
    // Needed cleanup stack items are pushed and popped within the function
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertClassPropertyL
// Converts CLASS property to a PIM ToDo item.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertClassPropertyL(
    const CPIMParserProperty& aProperty, CPIMToDoItem& aItem)
{
    JELOG2(EPim);
    CParserPropertyValueHBufC* propertyValue =
        static_cast<CParserPropertyValueHBufC*>(aProperty.Value());

    const TPtrC classStringPtr = propertyValue->Value();
    TInt classInt = KErrCorrupt;

    if (classStringPtr.CompareF(KPIMClassStringPublic) == 0)
    {
        classInt = EPIMToDoClassPublic;
    }
    else if (classStringPtr.CompareF(KPIMClassStringPrivate) == 0)
    {
        classInt = EPIMToDoClassPrivate;
    }
    else if (classStringPtr.CompareF(KPIMClassStringConfidential) == 0)
    {
        classInt = EPIMToDoClassConfidential;
    }
    // else the class value in the originating vCalendar is flawed - ignore

    if (classInt != KErrCorrupt)
    {
        aItem.AddIntL(EPIMToDoClass, KPIMAttrNone, classInt);
    }
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertClassFieldL
// Converts ToDo.CLASS field to a vCalendar.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertClassFieldL(const CPIMItem& aItem,
        CParserVCalEntity& aParser, TPIMToDoField aField, TInt aIndex)
{
    JELOG2(EPim);
    const TInt classVal = aItem.getInt(aField, aIndex);
    TPtrC classString(KPIMClassStringConfidential);

    switch (classVal)
    {
    case EPIMToDoClassConfidential:
    {
        classString.Set(KPIMClassStringConfidential);
        break;
    }
    case EPIMToDoClassPrivate:
    {
        classString.Set(KPIMClassStringPrivate);
        break;
    }
    case EPIMToDoClassPublic:
    {
        classString.Set(KPIMClassStringPublic);
        break;
    }
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        break;
    }
    }

    CParserPropertyValue* propertyValue = CParserPropertyValueHBufC::NewL(
                                              classString);
    AddPropertyToParserL(propertyValue, KVersitTokenCLASS(), aParser);
    // Needed cleanup stack items are pushed and popped within the function
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::ConvertAlarmFieldL
// Converts an alarm field from a PIM ToDo Item to a CParserVCard.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertAlarmFieldL(const CPIMItem& aItem, // item to convert from
        CParserVCalEntity& aParser, // parser to insert the property to.
        TPIMToDoField aField, // field to convert
        TInt aIndex) // index to the field
{
    JELOG2(EPim);
    // Calculate the alarm date and time
    const TInt alarmOffset = aItem.getInt(aField, aIndex);

    // If ToDo.DUE is not present there is no way to calculate alarm
    // so it has to be ignored
    if (aItem.CountValuesL(EPIMToDoDue) == 0)
    {
        return;
    }
    // There is ToDo.DUE present, so conversion can be made
    const TPIMDate dueDate = aItem.GetDateL(EPIMToDoDue, aIndex);
    TTime alarmTime(dueDate.Int64());
    // Count alarm time
    alarmTime -= TTimeIntervalSeconds(alarmOffset);

    TVersitDateTime
    * versitDateTime =
        new(ELeave) TVersitDateTime(alarmTime.DateTime(), TVersitDateTime::EIsUTC);
    CleanupDeletePushL(versitDateTime);

    CVersitAlarm* versitAlarm = CVersitAlarm::NewL(versitDateTime, NULL, 0,
                                KNullDesC, KNullDesC);
    // versitDateTime is now owned by versitAlarm
    CleanupStack::Pop(versitDateTime);
    CleanupStack::PushL(versitAlarm);
    CParserPropertyValue* propertyValue =
        new(ELeave) CParserPropertyValueAlarm(versitAlarm);
    // versitAlarm is now owned by propertyValue
    CleanupStack::Pop(versitAlarm);
    AddPropertyToParserL(propertyValue, KVersitTokenDALARM(), aParser);
    // Needed cleanup stack items are pushed and popped within the function
}

// -----------------------------------------------------------------------------
// CPIMEventPropertyConverter::ConvertAlarmPropertyL
// Converts an alarm property to a PIM ToDo item.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::ConvertAlarmPropertyL(
    const CPIMParserProperty& aProperty, TPIMDate& aAlarm)
{
    JELOG2(EPim);
    // Do not convert unrecognized fields
    if (aProperty.MatchToDoField() != EPIMToDoExtAlarm)
    {
        return;
    }

    CParserPropertyValueAlarm* propertyValue =
        static_cast<CParserPropertyValueAlarm*>(aProperty.Value());
    const CVersitAlarm* value = propertyValue->Value();
    const TVersitDateTime* versitDT = value->iRunTime;

    TPIMDate date(versitDT->iDateTime);
    aAlarm = date;
}

// -----------------------------------------------------------------------------
// CPIMToDoPropertyConverter::AddPropertyToParserL
// adds a new property value to the parser. NOTE that the property value can be
// of any type and it is taken to ownership within this function.
// -----------------------------------------------------------------------------
//
void CPIMToDoPropertyConverter::AddPropertyToParserL(
    CParserPropertyValue* aPropertyValue, const TDesC8& aPropertyName,
    CParserVCalEntity& aParser)
{
    JELOG2(EPim);
    // Add property value to the cleanup stack. It should not be added
    // before this call. This function is an exception and reduces much code
    CleanupStack::PushL(aPropertyValue);
    // Create a new parser property from the property value, its name and
    // array of parameters. If there are no parameters, the aArrayOfParams
    // should be NULL and the parser property will be generated without params
    CParserProperty* property = CParserProperty::NewL(*aPropertyValue,
                                aPropertyName, NULL);
    // property takes ownership of the property value
    CleanupStack::Pop(aPropertyValue);
    // NOTE: property MUST not be pushed to the cleanup stack since the
    // AddPropertyL pushes it right away to the cleanup stack. So, we must not
    // push it here to avoid duplicate stack pointers
    aParser.AddPropertyL(property);
    // Property is now owned by the parser but we do not have to pop it
}

// End of File
