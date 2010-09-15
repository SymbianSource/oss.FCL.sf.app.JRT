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
* Description:  Converts vCalendars PIM <-> Versit
 *
*/


// CLASS HEADER
#include "cpimcalendarconverter.h"

// INTERNAL INCLUDES
#include "pimcommon.h" // tpimfield
#include "pimpanics.h"
#include "pimversit.h"
#include "cpimitem.h"
#include "cpimeventitem.h"
#include "cpimtodoitem.h"
#include "cpimtodopropertyconverter.h"
#include "cpimeventpropertyconverter.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "cleanupresetanddestroy.h"

// EXTERNAL INCLUDES
#include <vcal.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMCalendarConverter* CPIMCalendarConverter::NewL(
    const CPIMEventValidator& aEventValidator,
    const CPIMToDoValidator& aToDoValidator,
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMCalendarConverter* self = new(ELeave) CPIMCalendarConverter(
        aEventValidator, aToDoValidator, aFuncServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMCalendarConverter::~CPIMCalendarConverter()
{
    JELOG2(EPim);
    delete iToDoConverter;
    delete iEventConverter;
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::ItemToStreamL
// Converts Event/ToDo Items to vCards.
// -----------------------------------------------------------------------------
//
void CPIMCalendarConverter::ItemToStreamL(RWriteStream& aStream, // RWriteStream to write the vCard to.
        const CPIMItem& aItem, // CPIMItem which is to be converted.
        const Versit::TVersitCharSet aCharset) // charSet used in versit objects
{
    JELOG2(EPim);
    CParserVCal* calParser = CParserVCal::NewL();
    CleanupStack::PushL(calParser);
    calParser->SetDefaultCharSet(aCharset);

    CParserVCalEntity* parser = CParserVCalEntity::NewL();
    // NOTE: parser is not put into the cleanup stack since AddEntityL
    // pushes it right from the start. So it MUST not be pushed here
    // to avoid duplicate cleanup stack pointers
    calParser->AddEntityL(parser);
    // No need to pop here

    CArrayFix<TPIMField>* fields = aItem.GetFieldsL();
    CleanupStack::PushL(fields);
    TInt fieldCount = fields->Count();
    switch (aItem.ItemType())
    {
    case EPIMEventList:
    {
        TPtrC vEvent(KVersitVarTokenVEVENT);
        parser->SetEntityNameL(vEvent);
        for (TInt i = 0; i < fieldCount; i++)
        {
            TPIMEventField eventField =
                static_cast<TPIMEventField>(fields->At(i));
            CallMethodL(iEventConverter,
                        &CPIMEventPropertyConverter::ConvertEventFieldL, aItem,
                        *parser, eventField, iFuncServer);
        }
        // Priority is a mandatory property for vCals so add it here...
        // Since event items do not have priority field, use unspecified
        // priority value which is zero
        CParserPropertyValue* value = new(ELeave) CParserPropertyValueInt(0);
        // value is pushed to the cleanup stack in AddPropertyToParserL
        // and parser takes the ownership of the added value
        iEventConverter->AddPropertyToParserL(value, KVersitTokenPRIORITY(),
                                              *parser);
        // Add repeat rule vCal property if the item is repeating
        const CPIMEventItem& eventItem =
            static_cast<const CPIMEventItem&>(aItem);
        if (eventItem.IsRepeating())
        {
            iEventConverter->ConvertRepeatRuleL(eventItem.GetRepeat(), *parser);
        }
        break;
    }
    case EPIMToDoList:
    {
        // Priority field neesds to be added if not present in the item
        if (aItem.CountValuesL(EPIMToDoCompleted) == 0)
        {
            CParserPropertyValue* propertyValue =
                CParserPropertyValueHBufC::NewL(KPIMStatusUncompleted);
            // value is pushed to the cleanup stack in AddPropertyToParserL
            // and parser takes the ownership of the added value
            iToDoConverter->AddPropertyToParserL(propertyValue,
                                                 KVersitTokenSTATUS(), *parser);
        }
        // Convert other fields
        TPtrC vToDo(KVersitVarTokenVTODO);
        parser->SetEntityNameL(vToDo);
        for (TInt i = 0; i < fieldCount; i++)
        {
            TPIMToDoField todoField =
                static_cast<TPIMToDoField>(fields->At(i));
            CallMethodL(iToDoConverter,
                        &CPIMToDoPropertyConverter::ConvertToDoFieldL, aItem, *parser,
                        todoField, iFuncServer);
        }
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    calParser->ExternalizeL(aStream);
    aStream.CommitL();
    CleanupStack::PopAndDestroy(2, calParser);  // fields, parser
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::StreamToItemL
// Converts vCards to Event Items
// -----------------------------------------------------------------------------
//
void CPIMCalendarConverter::StreamToItemL(RReadStream& aStream, // RReadStream to be converted.
        RPointerArray<CPIMItem>& aItemArray, Versit::TVersitCharSet aCharset)
{
    JELOG2(EPim);
    CParserVCal* parser = CParserVCal::NewL();
    CleanupStack::PushL(parser);
    parser->SetDefaultCharSet(aCharset);
    parser->InternalizeL(aStream);

    CArrayPtr<CVersitParser>* eventArray = parser->EntityL(
                                               KVersitVarTokenVEVENT, EFalse);
    CleanupStack::PushL(eventArray);
    CArrayPtr<CVersitParser>* todoArray = parser->EntityL(KVersitVarTokenVTODO,
                                          EFalse);
    CleanupStack::PushL(todoArray);

    if (eventArray)
    {
        TInt eventCount = eventArray->Count();
        for (TInt i = 0; i < eventCount; i++)
        {
            CParserVCalEntity* entity =
                static_cast<CParserVCalEntity*>(eventArray->At(i));
            ParserToEventL(*entity, aItemArray);
        }
    }

    if (todoArray)
    {
        TInt todoCount = todoArray->Count();
        for (TInt i = 0; i < todoCount; i++)
        {
            CParserVCalEntity* entity =
                static_cast<CParserVCalEntity*>(todoArray->At(i));
            ParserToToDoL(*entity, aItemArray);
        }
    }
    CleanupStack::PopAndDestroy(3, parser); // parser, eventArray, todoArray
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::CPIMCalendarConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMCalendarConverter::CPIMCalendarConverter(
    const CPIMEventValidator& aEventValidator,
    const CPIMToDoValidator& aToDoValidator,
    java::util::FunctionServer* aFuncServer) :
        iEventValidator(aEventValidator), iToDoValidator(aToDoValidator),
        iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMCalendarConverter::ConstructL()
{
    JELOG2(EPim);
    iToDoConverter = CPIMToDoPropertyConverter::NewL();
    iEventConverter = CPIMEventPropertyConverter::NewL();
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::ParserToEventL
// Converts vEvents to Event Items
// -----------------------------------------------------------------------------
//
void CPIMCalendarConverter::ParserToEventL(CParserVCalEntity& aParser,
        RPointerArray<CPIMItem>& aItemArray)
{
    JELOG2(EPim);
    CleanupClosePushL(aItemArray);
    CPIMEventItem* item = CPIMEventItem::NewLC(iEventValidator);
    TPIMDate alarm(TInt64(0));
    // We don't take the ownership of the propertyArray, so the properties
    // are deleted when the parser is
    CArrayPtr<CParserProperty>* propertyArray = aParser.ArrayOfProperties(
                EFalse);
    if (!propertyArray)
    {
        User::Leave(KErrCorrupt);
    }
    TInt propertyCount = propertyArray->Count();
    for (TInt i = 0; i < propertyCount; i++)
    {
        iEventConverter->ConvertPropertyL(*(propertyArray->At(i)), *item, alarm);
    }
    if (alarm.Int64() != TInt64(0))
    {
        // Count the alarm value
        if (item->CountValuesL(EPIMEventStart) > 0)
        {
            const TPIMDate startDate = item->GetDateL(EPIMEventStart, 0);
            TTimeIntervalSeconds interval(0);
            User::LeaveIfError(startDate.SecondsFrom(alarm, interval));
            item->addInt(EPIMEventAlarm, KPIMAttrNone, interval.Int());
        }
    }
    User::LeaveIfError(aItemArray.Append(item));
    CleanupStack::Pop(item); // item
    CleanupStack::Pop(&aItemArray);
}

// -----------------------------------------------------------------------------
// CPIMCalendarConverter::ParserToToDoL
// Converts vTodos to Todo Items
// -----------------------------------------------------------------------------
//
void CPIMCalendarConverter::ParserToToDoL(CParserVCalEntity& aParser,
        RPointerArray<CPIMItem>& aItemArray)
{
    JELOG2(EPim);
    CleanupClosePushL(aItemArray);
    CPIMToDoItem* item = CPIMToDoItem::NewLC(iToDoValidator);
    TPIMDate alarm(TInt64(0));
    // We don't take the ownership of the propertyArray, so the properties
    // are deleted when the parser is
    CArrayPtr<CParserProperty>* propertyArray = aParser.ArrayOfProperties(
                EFalse);
    if (!propertyArray)
    {
        User::Leave(KErrCorrupt);
    }
    TInt propertyCount = propertyArray->Count();
    for (TInt i = 0; i < propertyCount; i++)
    {
        iToDoConverter->ConvertPropertyL(*(propertyArray->At(i)), *item, alarm);
    }
    if (alarm.Int64() != TInt64(0))
    {
        // Count alarm value if possible
        if (item->CountValuesL(EPIMToDoDue) > 0)
        {
            const TPIMDate dueDate = item->GetDateL(EPIMToDoDue, 0);
            TTimeIntervalSeconds alarmOffset(0);
            User::LeaveIfError(dueDate.SecondsFrom(alarm, alarmOffset));
            item->AddIntL(EPIMToDoExtAlarm, KPIMAttrNone, alarmOffset.Int());
        }
    }
    if (item->CountValuesL(EPIMToDoCompletionDate) == 0)
    {
        item->AddBooleanL(EPIMToDoCompleted, KPIMAttrNone, EFalse);
    }
    else
    {
        item->AddBooleanL(EPIMToDoCompleted, KPIMAttrNone, ETrue);
    }
    User::LeaveIfError(aItemArray.Append(item));
    CleanupStack::Pop(item); // item
    CleanupStack::Pop(&aItemArray);
}

//  End of File
