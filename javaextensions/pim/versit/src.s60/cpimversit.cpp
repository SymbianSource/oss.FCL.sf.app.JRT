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
* Description:  Handles conversions between PIM Item <-> versit format
 *
*/


// INCLUDE FILES
#include "cpimmanager.h"
#include "cpimversit.h"
#include "cpimcardconverter.h"
#include "cpimcalendarconverter.h"
#include "cpimitem.h"
#include "cleanupresetanddestroy.h"
#include "logger.h"
#include <s32mem.h> // RBufWriteStream
#include <vtoken.h>
#include <vcard.h>
#include <vcal.h>

// CONSTANTS
_LIT(KPIMSlash, "/");
const TInt KPIMBufferGranularity = 40;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMVersit::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMVersit* CPIMVersit::NewL(CPIMManager* aManager)
{
    JELOG2(EPim);
    CPIMVersit* self = new(ELeave) CPIMVersit(aManager);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMVersit::~CPIMVersit()
{
    JELOG2(EPim);
    delete iCardConverter;
    delete iCalendarConverter;
    delete iVCardFormats;
    delete iVCalendarFormats;
}

// -----------------------------------------------------------------------------
// CPIMVersit::ItemToStringL
// Converts a PIM Item to a Versit object.
// -----------------------------------------------------------------------------
//
HBufC8* CPIMVersit::ItemToStringL(const CPIMItem& aItem,
                                  Versit::TVersitCharSet aCharset)
{
    JELOG2(EPim);
    CBufSeg* buffer = CBufSeg::NewL(KPIMBufferGranularity);
    CleanupStack::PushL(buffer);
    RBufWriteStream stream(*buffer, 0);
    stream.PushL();
    switch (aItem.ItemType())
    {
    case EPIMContactList:
    {
        iCardConverter->ItemToStreamL(stream, aItem, aCharset);
        break;
    }
    case EPIMToDoList:
    case EPIMEventList:
    {
        iCalendarConverter->ItemToStreamL(stream, aItem, aCharset);
        break;
    }
    default:
    {
        User::Leave(KErrGeneral);
    }
    }
    TInt bufferSize = buffer->Size();
    HBufC8* retVal = HBufC8::NewL(bufferSize);
    TPtr8 retDes(retVal->Des());
    buffer->Read(0, retDes, bufferSize);
    stream.Pop();
    CleanupStack::PopAndDestroy(buffer);
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMVersit::StringToItemL
// Converts a Versit object to a PIM Item.
// -----------------------------------------------------------------------------
//
RPointerArray<CPIMItem>* CPIMVersit::StringToItemL(const TDesC8& aString,
        Versit::TVersitCharSet aCharset)
{
    JELOG2(EPim);
    RPointerArray<CPIMItem>* itemArray =
        new(ELeave) RPointerArray<CPIMItem> (1);
    CleanupStack::PushL(itemArray);
    CleanupResetAndDestroyPushL(*itemArray);

    // figure out the type of the item
    // is it a vCard
    HBufC8* vCardBegin = Combine3DesC8L(KVersitTokenBEGIN(),
                                        KVersitTokenColon(), KVersitTokenVCARD());

    TInt isVCard = aString.Find(*vCardBegin);
    delete vCardBegin;
    if (isVCard == 0)
    {
        // this is a vCard
        StringToContactL(aString, *itemArray, aCharset);
        CleanupStack::Pop(2); // itemArray reset and destroy, itemArray object
        return itemArray;
    }

    HBufC8* vCalendarBegin = Combine3DesC8L(KVersitTokenBEGIN(),
                                            KVersitTokenColon(), KVersitTokenVCALENDAR());

    TInt isVCalendar = aString.Find(*vCalendarBegin);
    delete vCalendarBegin;
    if (isVCalendar == 0)
    {
        // this is a vCalendar

        RDesReadStream stream(aString);

        iCalendarConverter->StreamToItemL(stream, *itemArray, aCharset);
        CleanupStack::Pop(2); // itemArray reset and restroy, itemArray object
        return itemArray;
    }
    User::Leave(KErrNotSupported);
    return itemArray; // not really, just to avoid warning
}

// -----------------------------------------------------------------------------
// CPIMVersit::SupportedSerialFormatsL
// Returns an array of supported formats for a specific pim list type.
// -----------------------------------------------------------------------------
//
const CDesCArray& CPIMVersit::SupportedSerialFormatsL(TPIMListType aPimListType) const
{
    JELOG2(EPim);
    CDesCArray* retVal = NULL;
    switch (aPimListType)
    {
    case EPIMContactList:
    {
        retVal = iVCardFormats;
        break;
    }
    case EPIMEventList:
    case EPIMToDoList:
    {
        retVal = iVCalendarFormats;
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }
    return *retVal;
}

// -----------------------------------------------------------------------------
// CPIMVersit::CPIMVersit
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
//CPIMVersit::CPIMVersit(
//    const CPIMContactValidator& aContactValidator, // validates contacts
//    const CPIMEventValidator& aEventValidator, // validates events
//    const CPIMToDoValidator& aToDoValidator,  // validates todos
//    java::util::FunctionServer* aFuncServer )
//    : iContactValidator( aContactValidator),
//      iEventValidator( aEventValidator ),
//      iToDoValidator( aToDoValidator ),
//      iFuncServer(aFuncServer)
//    {
//    }
// -----------------------------------------------------------------------------
// CPIMVersit::CPIMVersit
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMVersit::CPIMVersit(CPIMManager* aManager) :
        iContactValidator(aManager->ContactValidator()), iEventValidator(
            aManager->EventValidator()), iToDoValidator(aManager->ToDoValidator()),
        iFuncServer(aManager)
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMVersit::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMVersit::ConstructL()
{
    JELOG2(EPim);
    iVCardFormats = new(ELeave) CDesC16ArrayFlat(1);
    HBufC* vCardFormat = Combine3DesCLC(KVersitVarTokenVCARD(), KPIMSlash(),
                                        KVersitTokenVCardVersionNo());
    iVCardFormats->AppendL(*vCardFormat);
    CleanupStack::PopAndDestroy(vCardFormat); // vCardFormat

    iVCalendarFormats = new(ELeave) CDesC16ArrayFlat(1);
    HBufC* vCalendarFormat = Combine3DesCLC(KVersitVarTokenVCALENDAR(),
                                            KPIMSlash(), KVersitTokenVCalVersionNo());
    iVCalendarFormats->AppendL(*vCalendarFormat);
    CleanupStack::PopAndDestroy(vCalendarFormat); // vCalendarFormat

    iCardConverter = CPIMCardConverter::NewL(iContactValidator, iFuncServer);
    iCalendarConverter = CPIMCalendarConverter::NewL(iEventValidator,
                         iToDoValidator, iFuncServer);
}

// -----------------------------------------------------------------------------
// CPIMVersit::StringToContact
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMVersit::StringToContactL(const TDesC8& aString, // contains a vCard
                                  RPointerArray<CPIMItem>& aItemArray, // items are stored here
                                  Versit::TVersitCharSet aCharset) // defines the used character set
{
    JELOG2(EPim);
    // vCard specification says that END:VCARD is optional.
    // However, Symbian Versit parser chokes without it. So,
    // we have to check if one exists
    HBufC8* vCardEnd = HBufC8::NewL(KVersitTokenCRLF().Length()
                                    + KVersitTokenEND().Length() + KVersitTokenVCARD().Length()
                                    + KVersitTokenColon().Length());
    CleanupStack::PushL(vCardEnd);
    TPtr8 vCardEndDes = vCardEnd->Des();
    vCardEndDes = KVersitTokenCRLF();
    vCardEndDes.Append(KVersitTokenEND());
    vCardEndDes.Append(KVersitTokenColon());
    vCardEndDes.Append(KVersitTokenVCARD());
    TInt endExist = aString.Find(*vCardEnd);
    if (endExist != KErrNotFound)
    {
        RDesReadStream stream(aString);
        iCardConverter->StreamToItemL(stream, aItemArray, aCharset);
    }
    else
    {
        HBufC8* vCard =
            Combine3DesC8L(aString, vCardEndDes, KVersitLineBreak());
        CleanupStack::PushL(vCard);
        RDesReadStream stream(*vCard);
        iCardConverter->StreamToItemL(stream, aItemArray, aCharset);
        CleanupStack::PopAndDestroy(vCard); // vCard
    }
    CleanupStack::PopAndDestroy(vCardEnd); // vCardEnd

}

// -----------------------------------------------------------------------------
// CPIMVersit::Combine3DesC8L
// Combines 3 8-bit descriptors into one
// -----------------------------------------------------------------------------
//
HBufC8* CPIMVersit::Combine3DesC8L(const TDesC8& aDesC1, const TDesC8& aDesC2,
                                   const TDesC8& aDesC3)
{
    JELOG2(EPim);
    HBufC8* retVal = HBufC8::NewL(aDesC1.Length() + aDesC2.Length()
                                  + aDesC3.Length());
    TPtr8 retValDes = retVal->Des();
    retValDes = aDesC1;
    retValDes.Append(aDesC2);
    retValDes.Append(aDesC3);
    return retVal;
}

// -----------------------------------------------------------------------------
// CPIMVersit::Combine3DesCLC
// Combines 3 16-bit descriptors into one, which is left in cleanup stack
// -----------------------------------------------------------------------------
//
HBufC* CPIMVersit::Combine3DesCLC(const TDesC& aDesC1, const TDesC& aDesC2,
                                  const TDesC& aDesC3)
{
    JELOG2(EPim);
    HBufC* retVal = HBufC::NewL(aDesC1.Length() + aDesC2.Length()
                                + aDesC3.Length());
    TPtr retValDes = retVal->Des();
    retValDes = aDesC1;
    retValDes.Append(aDesC2);
    retValDes.Append(aDesC3);
    CleanupStack::PushL(retVal);
    return retVal;
}

//  End of File
