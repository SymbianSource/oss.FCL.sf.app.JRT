/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaAttribute implementation
*
*/


#include <s32mem.h>
#include "javaattributes.h"

using namespace Java::Manager::Registry;

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaAttribute::NewL
// Constructs a CJavaAttributes object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttributes* CJavaAttributes::NewL()
{
    CJavaAttributes* self = CJavaAttributes::NewLC();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttributes::NewLC
// Constructs a CJavaAttributes object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttributes* CJavaAttributes::NewLC()
{
    CJavaAttributes* self = new(ELeave) CJavaAttributes();
    CleanupStack::PushL(self);
    self->ConstructL(NULL);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::NewL
// Constructs a CJavaAttributes object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttributes* CJavaAttributes::NewL(TDesC8* aBuffer)
{
    CJavaAttributes* self = CJavaAttributes::NewLC(aBuffer);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::NewLC
// Constructs a CJavaAttribute object.
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttributes* CJavaAttributes::NewLC(TDesC8* aBuffer)
{
    CJavaAttributes* self = new(ELeave) CJavaAttributes();
    CleanupStack::PushL(self);
    self->ConstructL(aBuffer);
    return self;
}

// ---------------------------------------------------------------------------
// CJavaAttribute::ConstructL
// ---------------------------------------------------------------------------
void CJavaAttributes::ConstructL(TDesC8* aBuffer)
{
    if (aBuffer)
    {
        RDesReadStream stream(*aBuffer);
        CleanupClosePushL(stream);
        stream >> *this;
        CleanupStack::PopAndDestroy(&stream);
    }
    else
    {
        iAttributes.Reset();
    }
}

// ---------------------------------------------------------------------------
// CJavaAttribute::ConstructL
// ---------------------------------------------------------------------------
EXPORT_C CJavaAttributes::~CJavaAttributes()
{
    DeleteAttributes();
}

// ---------------------------------------------------------------------------
// CJavaAttributes::Count
// Returns TInt
// ---------------------------------------------------------------------------
EXPORT_C TInt CJavaAttributes::Count() const
{
    return iAttributes.Count();
}

// ---------------------------------------------------------------------------
// CJavaAttributes::GetAttribute
// Returns MJavaAttribute&
// ---------------------------------------------------------------------------
EXPORT_C const MJavaAttribute* CJavaAttributes::
Attribute(const TDesC& aName) const
{
    TInt i = 0;
    while (i < iAttributes.Count())
    {
        if (iAttributes[i]->Name() == aName)
        {
            return iAttributes[i];
        }
        else
        {
            i = i + 1;
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CJavaAttributes::GetAttributes
// Returns RPointerArray&
// ---------------------------------------------------------------------------
EXPORT_C const RPointerArray< MJavaAttribute >&
CJavaAttributes::Attributes() const
{
    return iAttributes;
}

// ---------------------------------------------------------------------------
// CJavaAttributes::AddAttributeL
// Returns TInt
// ---------------------------------------------------------------------------
EXPORT_C void CJavaAttributes::AddAttributeL(MJavaAttribute* aAttribute)
{
    TInt i = 0;
    MJavaAttribute* attr = NULL;
    // Check if attribute with the same name exist, if yes delete it.
    while (i < iAttributes.Count())
    {
        if (iAttributes[i]->Name() == aAttribute->Name())
        {
            attr = iAttributes[i];
            break;
        }
        i = i + 1;
    }
    if (attr)
    {
        delete attr;
        iAttributes.Remove(i);
    }
    TInt err = iAttributes.Append(aAttribute);
    if (err)
    {
        delete aAttribute;     //delete not added attribute
        User::Leave(err);
    }
}

// ---------------------------------------------------------------------------
// CJavaAttributes::AddAttributeL
// Returns TInt
// ---------------------------------------------------------------------------
EXPORT_C void CJavaAttributes::
AddAttributeL(const TDesC& aName, const TDesC& aValue, TBool aTrusted)
{
    CJavaAttribute* attribute = CJavaAttribute::NewL(aName, aValue, aTrusted);
    AddAttributeL(attribute);
}

// ---------------------------------------------------------------------------
// CJavaAttributes::DeleteAttributes
// ---------------------------------------------------------------------------
EXPORT_C void CJavaAttributes::DeleteAttributes()
{
    TInt count = iAttributes.Count();
    for (TInt i = 0; i < count; i++)
    {
        delete iAttributes[i];
        iAttributes[i] = NULL;
    }
    iAttributes.ResetAndDestroy();
}

// ---------------------------------------------------------------------------
// CJavaAttributes::ExternalizeL
// ---------------------------------------------------------------------------
void CJavaAttributes::ExternalizeL(RWriteStream& aStream) const
{
    TInt32 attr_count = iAttributes.Count();
    aStream << attr_count;
    for (TInt i = 0; i < attr_count; i++)
    {
        MJavaAttribute* attr = iAttributes[i];
        TInt32 val = attr->Name().Length();
        aStream << val << attr->Name();
        val = attr->Value().Length();
        aStream << val << attr->Value();
        val = attr->Trusted();
        aStream << val;
    }
}

// ---------------------------------------------------------------------------
// CJavaAttributes::InternalizeL
// ---------------------------------------------------------------------------
void CJavaAttributes::InternalizeL(RReadStream& aStream)
{
    if (iAttributes.Count())    // if exist some attributes delete it
    {
        DeleteAttributes();
    }
    TInt32 count;
    aStream >> count;      // read number of attributes
    for (TInt i = 0; i < count; i++)
    {
        HBufC* name;
        HBufC* value;
        TInt32 trusted;
        TInt32 len;
        aStream >> len;
        name = HBufC::NewLC(aStream, len);
        aStream >> len;
        value = HBufC::NewLC(aStream, len);
        aStream >> trusted;
        MJavaAttribute* attr = CJavaAttribute::
                               NewL(*name, *value, STATIC_CAST(TBool, trusted));
        CleanupStack::PopAndDestroy(value);          //value
        CleanupStack::PopAndDestroy(name);       // name
        TInt err = iAttributes.Append(attr);
        if (err)
        {
            DeleteAttributes();    // delete appended properities
            delete attr;           // delete not appended property
            User::Leave(err);
        }
    }

}

// ---------------------------------------------------------------------------
// CJavaAttributes::Size
// Returns TInt;
// ---------------------------------------------------------------------------
EXPORT_C TInt CJavaAttributes::Size() const
{

    TInt size = sizeof(TInt32);                          // size of count
    TInt count = iAttributes.Count();
    TInt boolsize = sizeof(TBool);
    TInt int32size =  sizeof(TInt32);
    // size of all value "IsTrusted" and size of stored size
    size += count * (boolsize + (int32size * 2));
    // add sizes of name and value of all attributes
    for (TInt i = 0; i < count; i++)
    {
        MJavaAttribute* attr = iAttributes[i];
        size += attr->Name().Size();
        size += attr->Value().Size();
    }
    return size;
}


// ---------------------------------------------------------------------------
// CJavaAttributes::SerializedAttributesL
// Returns HBufC8* serialized CJavaAttributes object;
// ---------------------------------------------------------------------------
EXPORT_C HBufC8* CJavaAttributes::SerializedAttributesL() const
{
    TInt32 size = Size();
    HBufC8* buffer = HBufC8::NewLC(size);
    TPtr8 des8 = buffer->Des();
    RDesWriteStream stream(des8);
    CleanupClosePushL(stream);
    stream << *this;
    stream.CommitL();
    CleanupStack::PopAndDestroy(&stream);        // stream
    CleanupStack::Pop(buffer);                  // buffer
    return buffer;
}

// End of File
