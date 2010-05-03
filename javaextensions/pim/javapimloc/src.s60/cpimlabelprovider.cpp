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
* Description:  Holds PIM API field and attribute labels
*
*/


// CLASS HEADER
#include    "cpimlabelprovider.h"

// INTERNAL INCLUDES
#include    "pimpanics.h"

// EXTERNAL INCLUDES
#include    <barsc.h>
#include    <barsread.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMLabelProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMLabelProvider* CPIMLabelProvider::NewL(TResourceReader& aReader)
{
    CPIMLabelProvider* self = CPIMLabelProvider::NewLC(aReader);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CPIMLabelProvider::NewL
// Two-phased constructor. Created item is left to the cleanup stack
// -----------------------------------------------------------------------------
//
CPIMLabelProvider* CPIMLabelProvider::NewLC(TResourceReader& aReader)
{
    CPIMLabelProvider* self = new(ELeave) CPIMLabelProvider();
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aReader);
    return self;
}

// -----------------------------------------------------------------------------
// CPIMLabelProvider::~CPIMLabelProvider
// Destructor
// -----------------------------------------------------------------------------
//
CPIMLabelProvider::~CPIMLabelProvider()
{
    delete iSubElementLabels;
    delete iSubElementIds;
    delete iLabel;
}

// -----------------------------------------------------------------------------
// CPIMLabelProvider::LabelL
// Returns a label of an attribute or field. Caller takes ownership of the value
// -----------------------------------------------------------------------------
//
HBufC* CPIMLabelProvider::LabelL(const TInt aId) const
{
    if (aId == iId)
    {
        __ASSERT_DEBUG(iLabel,
                       User::Panic(KPIMPanicCategory, EPIMPanicInvalidLabel));
        return iLabel->AllocL();
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// CPIMLabelProvider::LabelL
// Returns a label of an array element. Caller takes ownership of the value
// -----------------------------------------------------------------------------
//
HBufC* CPIMLabelProvider::LabelL(const TInt aId, const TInt aElement) const
{
    // Check that field id matches and this field contains sub-elements
    if (aId == iId && iSubElementIds)
    {
        TInt count = iSubElementIds->Count();
        for (TInt i(0); i < count; i++)
        {
            if (iSubElementIds->At(i) == aElement)
            {
                return iSubElementLabels->MdcaPoint(i).AllocL();
            }
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// Default C++ constructor
// -----------------------------------------------------------------------------
//¨
CPIMLabelProvider::CPIMLabelProvider()
{
}

// -----------------------------------------------------------------------------
// CPIMLabelProvider::ConstructFromResourceL
// -----------------------------------------------------------------------------
//¨
void CPIMLabelProvider::ConstructFromResourceL(TResourceReader& aReader)
{
    // Read provider identifier and label
    iId = aReader.ReadInt32();
    iLabel = aReader.ReadHBufCL();

    TBool hasSubElements(aReader.ReadInt8());

    // Read sub elements
    if (hasSubElements)
    {
        // Create arrays. 7 for granularity
        iSubElementIds = new(ELeave) CArrayFixFlat< TInt >(7);
        iSubElementLabels = new(ELeave) CDesCArrayFlat(7);

        TInt subelemCount(aReader.ReadInt16());
        for (TInt i(0); i < subelemCount; i++)
        {
            iSubElementIds->AppendL(aReader.ReadInt32());
            iSubElementLabels->AppendL(aReader.ReadTPtrC());
        }
    }
}

// End of file
