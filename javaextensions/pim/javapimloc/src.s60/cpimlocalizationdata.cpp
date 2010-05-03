/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for all localization data objects.
*
*/


// CLASS HEADER
#include "cpimlocalizationdata.h"

// INTERNAL INCLUDES
#include "cpimlabelprovider.h"
#include "pimpanics.h"

// EXTERNAL INCLUDES
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Number of resource links in the PIM_LIST resource structure
const TInt KPIMNumListResourceLinks = 3;
}

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPIMLocalizationData::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMLocalizationData* CPIMLocalizationData::NewL(
    RResourceFile& aResourceFile,
    TResourceReader& aReader,
    TInt aSubType)
{
    CPIMLocalizationData* self =
        CPIMLocalizationData::NewLC(aResourceFile, aReader, aSubType);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::NewL
// Two-phased constructor. Created item is left to the cleanup stack
// -----------------------------------------------------------------------------
//
CPIMLocalizationData* CPIMLocalizationData::NewLC(
    RResourceFile& aResourceFile,
    TResourceReader& aReader,
    TInt aSubType)
{
    CPIMLocalizationData* self = new(ELeave) CPIMLocalizationData(aSubType);
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aResourceFile, aReader);
    return self;
}

// Destructor
CPIMLocalizationData::~CPIMLocalizationData()
{
    iFields.ResetAndDestroy();
    iAttributes.ResetAndDestroy();
    delete iListName;
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::GetFieldLabelL
// Provides a string label associated with the given field. The caller
// takes ownership of the returned object.
// Returns: String label for the field. The label is locale specific.
// -----------------------------------------------------------------------------
//
HBufC* CPIMLocalizationData::GetFieldLabelL(TPIMField aField)
{
    TInt count(iFields.Count());
    for (TInt i(0); i < count; i++)
    {
        // Search through each label provider and ask field label. Null means
        // that the provider does not have label for this id. Try next
        HBufC* label = iFields[ i ]->LabelL(aField);
        if (label)
        {
            // Found label for this id, return it
            return label;
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::GetAttributeLabelL
// Provides a string label associated with the given attribute. The caller
// takes ownership of the returned object.
// Returns: String label for the attribute. The label is locale specific.
// -----------------------------------------------------------------------------
//
HBufC* CPIMLocalizationData::GetAttributeLabelL(TPIMAttribute aAttribute)
{
    TInt count(iAttributes.Count());
    for (TInt i(0); i < count; i++)
    {
        // Search through each label provider and ask attribute. Null means
        // that the provider does not have label for this id. Try next
        HBufC* label = iAttributes[ i ]->LabelL(aAttribute);
        if (label)
        {
            // Found label for this id, return it
            return label;
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::GetArrayElementLabelL
// Provides a string label associated with the given array element. The caller
// takes ownership of the returned object.
// Returns: String label for the array element. The label is locale specific.
// -----------------------------------------------------------------------------
//
HBufC* CPIMLocalizationData::GetArrayElementLabelL(
    TPIMField aStringArrayField,
    TPIMArrayElement aArrayElement)
{
    TInt count(iFields.Count());
    for (TInt i(0); i < count; i++)
    {
        // Search through each label provider and ask attribute label. Null means
        // that the provider does not have label for this id. Try next
        HBufC* label = iFields[ i ]->LabelL(aStringArrayField, aArrayElement);
        if (label)
        {
            // Found label for this id, return it
            return label;
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::GetListNameL
// Provides the name of the list. The caller
// takes ownership of the returned object.
// Returns: the list name
// -----------------------------------------------------------------------------
//
HBufC* CPIMLocalizationData::GetListNameL()
{
    __ASSERT_DEBUG(iListName,
                   User::Panic(KPIMPanicCategory, EPIMPanicGeneral));
    return iListName->AllocL();
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::CPIMLocalizationData
// -----------------------------------------------------------------------------
//
CPIMLocalizationData::CPIMLocalizationData(TInt aSubType)
        : iSubType(aSubType)
{
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::ConstructFromResourceL
// -----------------------------------------------------------------------------
//
void CPIMLocalizationData::ConstructFromResourceL(
    RResourceFile& aResourceFile,
    TResourceReader& aReader)
{
    TInt listCount(aReader.ReadInt16());

    __ASSERT_DEBUG(listCount > iSubType,
                   User::Panic(KPIMPanicCategory, EPIMPanicGeneral));

    // Find the correct resource structure for the requested sub-type. If
    // the list type is incorrect the reader skips the incorrect resource
    // structure and tries the next one until no lists can be processed.
    for (TInt i(1); i <= listCount; i++)
    {
        TInt listType(aReader.ReadInt8());
        if (listType == iSubType)
        {
            ReadListInfoFromResourceL(aResourceFile, aReader);
            break;
        }
        else if (i < listCount)
        {
            // Advance in resource file since this wasn't the list which
            // was requested. Currently there are three LLINK:s to skip
            aReader.Advance(sizeof(TInt32) * KPIMNumListResourceLinks);
        }
    }
}

// -----------------------------------------------------------------------------
// CPIMLocalizationData::ReadListInfoFromResourceL
// -----------------------------------------------------------------------------
//
void CPIMLocalizationData::ReadListInfoFromResourceL(
    RResourceFile& aResourceFile,
    TResourceReader& aReader)
{
    TInt32 listNameLabelId(aReader.ReadInt32());
    TInt32 fieldsId(aReader.ReadInt32());
    TInt32 attrsId(aReader.ReadInt32());

    // Read label from resource
    aReader.SetBuffer(aResourceFile.AllocReadLC(listNameLabelId));
    iListName = aReader.ReadHBufCL();
    // Resource buffer is not needed anymore
    CleanupStack::PopAndDestroy(); // AllocReadLC()

    // Read fields from resource. Fields must be specified in the resource
    // file since empty lists are not allowed.
    aReader.SetBuffer(aResourceFile.AllocReadLC(fieldsId));

    TInt count(aReader.ReadInt16());
    for (TInt i(0); i < count; i++)
    {
        CPIMLabelProvider* provider = CPIMLabelProvider::NewLC(aReader);
        iFields.AppendL(provider);
        CleanupStack::Pop(provider);
    }
    // Resource buffer is not needed anymore
    CleanupStack::PopAndDestroy(); // AllocReadLC()

    // Read attributes from resource. Do nothing if attributes not specified
    if (attrsId != 0)
    {
        // Create buffer to the attributes lists
        aReader.SetBuffer(aResourceFile.AllocReadLC(attrsId));

        count = aReader.ReadInt16();
        for (TInt i(0); i < count; i++)
        {
            CPIMLabelProvider* provider = CPIMLabelProvider::NewLC(aReader);
            iAttributes.AppendL(provider);
            CleanupStack::Pop(provider);
        }
        // Resource buffer is not needed anymore
        CleanupStack::PopAndDestroy(); // AllocReadLC()
    }
}

// End of file
