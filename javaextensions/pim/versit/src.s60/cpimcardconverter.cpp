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
* Description:  Converts vCards PIM <-> Versit
 *
*/


// INCLUDE FILES
#include "cpimcardconverter.h"
#include "pimcommon.h" // tpimfield
#include "pimpanics.h"
#include "cpimitem.h"
#include "cpimcontactitem.h"
#include "cpimcontactvalidator.h"
#include "cpimcardpropertyconverter.h"
#include "fs_methodcall.h"
#include "logger.h"
#include "javasymbianoslayer.h"
#include <vcard.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMCardConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMCardConverter* CPIMCardConverter::NewL(
    const CPIMContactValidator& aContactValidator,
    java::util::FunctionServer* aFuncServer) // validator used for contacts
{
    JELOG2(EPim);
    CPIMCardConverter* self =
        new(ELeave) CPIMCardConverter(aContactValidator, aFuncServer);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMCardConverter::~CPIMCardConverter()
{
    JELOG2(EPim);
    delete iPropertyConverter;
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::ItemToStreamL
// Converts Contact Items to vCards.
// -----------------------------------------------------------------------------
//
void CPIMCardConverter::ItemToStreamL(RWriteStream& aStream, // RWriteStream to write the vCard to.
                                      const CPIMItem& aItem, // CPIMItem which is to be converted.
                                      Versit::TVersitCharSet aCharset) // defines charset used in vcards
{
    JELOG2(EPim);
    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);
    parser->SetDefaultCharSet(aCharset);
    CArrayFix<TPIMField>* fields = aItem.GetFieldsL();
    CleanupStack::PushL(fields);
    TInt fieldCount = fields->Count();

    for (TInt i = 0; i < fieldCount; i++)
    {
        TPIMContactField contactField = (TPIMContactField)(fields->At(i));
        CallMethodL(iPropertyConverter,
                    &CPIMCardPropertyConverter::ConvertFieldL, aItem, *parser,
                    contactField, iFuncServer);
    }
    parser->ExternalizeL(aStream);
    aStream.CommitL();
    CleanupStack::PopAndDestroy(2, parser); // fields, parser
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::StreamToItemL
// Converts vCards to Contact Items
// -----------------------------------------------------------------------------
//
void CPIMCardConverter::StreamToItemL(RReadStream& aStream, // RReadStream to be converted.
                                      RPointerArray<CPIMItem>& aItemArray, Versit::TVersitCharSet aCharset)
{
    JELOG2(EPim);
    CParserVCard* parser = CParserVCard::NewL();
    CleanupStack::PushL(parser);
    parser->SetDefaultCharSet(aCharset);
    parser->InternalizeL(aStream);

    CArrayPtrFlat<CParserVCard>* parserArray = new(ELeave) CArrayPtrFlat<
    CParserVCard> (1);
    CleanupStack::PushL(parserArray);
    parserArray->AppendL(parser);

    // parserArray may grow inside the loop, therefore the Count() -method
    // is always called.
    for (TInt i = 0; i < parserArray->Count(); i++)
    {
        ParserToItemL(*(parserArray->At(i)), aItemArray, *parserArray);
    }

    if (aItemArray.Count() == 0)
    {
        User::Leave(KErrCorrupt);
    }

    CleanupStack::PopAndDestroy(2, parser); // parserArray, parser
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::ParserToItemL
// Converts vCards to Contact Items
// -----------------------------------------------------------------------------
//
void CPIMCardConverter::ParserToItemL(CParserVCard& aParser, // parser to be converted
                                      RPointerArray<CPIMItem>& aItemArray, // items are added here
                                      CArrayPtrFlat<CParserVCard>& aParserArray) // contained vCards here
{
    JELOG2(EPim);
    CleanupClosePushL(aItemArray);
    CPIMContactItem* item = CPIMContactItem::NewLC(iContactValidator);
    // We don't take the ownership of the propertyArray, so the properties
    // are deleted when the parser is
    CArrayPtr<CParserProperty>* propertyArray = aParser.ArrayOfProperties(
                EFalse);
    if (propertyArray)
    {
        TInt propertyCount = propertyArray->Count();
        for (TInt i = 0; i < propertyCount; i++)
        {
            CParserVCard* containedCard = iPropertyConverter->ConvertPropertyL(
                                              *(propertyArray->At(i)), *item);
            if (containedCard)
            {
                aParserArray.AppendL(containedCard);
            }
        }
        User::LeaveIfError(aItemArray.Append(item));
        CleanupStack::Pop(item); // item
    }
    else
    {
        // no properties, no item
        CleanupStack::PopAndDestroy(item);
    }
    // Let's see if there are any subentities to be converted as well
    CArrayPtr<CVersitParser>* entityArray = aParser.ArrayOfEntities(EFalse);
    if (entityArray)
    {
        TInt entityCount = entityArray->Count();
        for (TInt i = 0; i < entityCount; i++)
        {
            CParserVCard* subParser =
                static_cast<CParserVCard*>(entityArray->At(i));
            aParserArray.AppendL(subParser);
        }
    }
    CleanupStack::Pop(&aItemArray);
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::CPIMCardConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMCardConverter::CPIMCardConverter(
    const CPIMContactValidator& aContactValidator,
    java::util::FunctionServer* aFuncServer) :
        iContactValidator(aContactValidator), iFuncServer(aFuncServer)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMCardConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMCardConverter::ConstructL()
{
    JELOG2(EPim);
    iPropertyConverter = CPIMCardPropertyConverter::NewL(iContactValidator);
}

//  End of File
