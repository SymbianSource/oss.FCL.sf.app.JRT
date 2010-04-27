/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javapropertyarray implementation
*
*/


#include <s32mem.h>
#include "javaregproperty.h"
#include "javapropertyarray.h"
#include "logger.h"

// using namespace Java::Utilities;
using namespace Java::Manager::Registry;

// ---------------------------------------------------------------------------
// Constructs a java property array object.
// ---------------------------------------------------------------------------

CJavaPropertyArray* CJavaPropertyArray::NewL()
{
    CJavaPropertyArray* self = NewLC();
    CleanupStack::Pop();
    return self;
}


// ---------------------------------------------------------------------------
// Constructs a java property array object.
// ---------------------------------------------------------------------------
CJavaPropertyArray* CJavaPropertyArray::NewLC()
{
    CJavaPropertyArray* self = new(ELeave) CJavaPropertyArray();
    CleanupStack::PushL(self);
    self->ConstructL(0);
    return self;
}


// ---------------------------------------------------------------------------
// Constructs a java property array object from a buffer.
// ---------------------------------------------------------------------------
CJavaPropertyArray* CJavaPropertyArray::NewL(TDesC8*  aBuffer)
{
    CJavaPropertyArray* self = NewLC(aBuffer);
    CleanupStack::Pop();
    return self;
}


// ---------------------------------------------------------------------------
// Constructs a java property array object from a buffer.
// ---------------------------------------------------------------------------
CJavaPropertyArray* CJavaPropertyArray::NewLC(TDesC8*  aBuffer)
{
    CJavaPropertyArray* self = new(ELeave) CJavaPropertyArray();
    CleanupStack::PushL(self);
    self->ConstructL(aBuffer);
    return self;
}


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CJavaPropertyArray::ConstructL(TDesC8*  aBuffer)
{
    if (aBuffer)
    {
        RDesReadStream  stream(*aBuffer);
        CleanupClosePushL(stream);
        stream >> *this;
        CleanupStack::PopAndDestroy();  // stream
    }
}


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
CJavaPropertyArray::~CJavaPropertyArray()
{
    if (iPropertiesOwned)
    {
        DeleteProperties();
    }
    iProperties.Close();
}


// ---------------------------------------------------------------------------
// Appends a new property object reference.
// ---------------------------------------------------------------------------
TInt CJavaPropertyArray::Append(CJavaProperty*  aProperty)
{
    return iProperties.Append(aProperty);
}


// ---------------------------------------------------------------------------
// Returns the total number of contained property object references.
// ---------------------------------------------------------------------------
TInt CJavaPropertyArray::Count()
{
    return iProperties.Count();
}


// ---------------------------------------------------------------------------
// Returns the property object reference located at a specified position.
// ---------------------------------------------------------------------------
CJavaProperty* CJavaPropertyArray::At(TInt aIndex)
{
    return iProperties[aIndex];
}


// ---------------------------------------------------------------------------
// Internalizes the property objects.
// ---------------------------------------------------------------------------
void CJavaPropertyArray::InternalizeL(RReadStream&  aStream)
{
    // properties are created internally
    iPropertiesOwned = ETrue;

    // if there are already contained properties delete them and reset array
    if (Count())
    {
        LOG(EJavaStorage, EInfo,
            "PropertyArray already contained properties, delete them.");
        DeleteProperties();
    }

    // get the number of properties
    TInt32 count;
    aStream >> count;

    // internalize poperties
    for (TInt propIndex = 0; propIndex < count; propIndex++)
    {
        TInt32 id;
        HBufC* value;
        TInt32 valueLength;

        aStream >> id >> valueLength;
        value = HBufC::NewLC(aStream, valueLength);

        CJavaProperty* prop = CJavaProperty::NewL(id, *value);
        CleanupStack::PopAndDestroy();  // value
        TInt err = Append(prop);

        // in case of error delete property objects and leave
        if (err)
        {
            ELOG1(EJavaStorage,
                  "Error on appending property, delete properties. Error: %d.", err);
            // delete not appended property
            delete prop;
            // delete appended properties
            DeleteProperties();
            User::Leave(err);
        }
    }
}


// ---------------------------------------------------------------------------
// Externalizes the property objects.
// ---------------------------------------------------------------------------
void CJavaPropertyArray::ExternalizeL(RWriteStream&  aStream) const
{
    // first externalize the number of properties
    TInt32 count = iProperties.Count();
    aStream << count;

    // externalize poperties
    for (TInt propIndex = 0; propIndex < count; propIndex++)
    {
        CJavaProperty* prop = iProperties[propIndex];
        TInt32 valueLength = prop->Value().Length();
        aStream << prop->Id() << valueLength << prop->Value();
    }
}


// ---------------------------------------------------------------------------
// Calculates the number of bytes needed for serialization of
// the property objects.
// ---------------------------------------------------------------------------
TInt CJavaPropertyArray::Size() const
{
    // size of the integer containing the total number of properties
    TInt size = sizeof(TInt32);

    // increment size with size of properties
    for (TInt propIndex = 0; propIndex < iProperties.Count(); propIndex++)
    {
        CJavaProperty* prop = iProperties[propIndex];

        // property id size + size of integer containing the value length
        // + size of property value
        size += sizeof(prop->Id()) +
                sizeof(TInt32) +
                prop->Value().Size();
    }

    return size;
}


// ---------------------------------------------------------------------------
// Contains
// ---------------------------------------------------------------------------
TBool CJavaPropertyArray::Contains(CJavaProperty& aProperty) const
{
    for (TInt propIndex = 0; propIndex < iProperties.Count(); propIndex++)
    {
        CJavaProperty* prop = iProperties[propIndex];

        if (*prop == aProperty)
        {
            return ETrue;
        }
    }

    return EFalse;
}


// ---------------------------------------------------------------------------
// CJavaPropertyArray::Find
// ---------------------------------------------------------------------------
TInt CJavaPropertyArray::Find(TInt32 aPropertyId)
{
    for (TInt propIndex = 0; propIndex < iProperties.Count(); propIndex++)
    {
        CJavaProperty* prop = iProperties[propIndex];

        if (prop->Id() == aPropertyId)
        {
            return propIndex;
        }
    }

    return KErrNotFound;
}


// ---------------------------------------------------------------------------
// Serializes the property objects in a buffer.
// ---------------------------------------------------------------------------
HBufC8* CJavaPropertyArray::SerializedPropertiesL() const
{
    HBufC8* buffer = HBufC8::NewLC(Size());

    TPtr8 des8 = buffer->Des();
    RDesWriteStream stream(des8);
    CleanupClosePushL(stream);

    stream << *this;
    stream.CommitL();
    CleanupStack::PopAndDestroy();  // stream
    CleanupStack::Pop();            // buffer

    return buffer;
}


// ---------------------------------------------------------------------------
// Delete and remove property objects.
// ---------------------------------------------------------------------------
void CJavaPropertyArray::DeleteProperties()
{
    for (TInt i = 0; i < Count(); i++)
    {
        delete iProperties[i];
        iProperties[i] = NULL;
    }
    iProperties.Reset();
}
