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
* Description:  Valid contact data info.
 *
*/


// INCLUDE FILES
#include  "cpimvalidator.h"
#include  "pimcontact.h"
#include "logger.h"

// CONSTANTS
const TInt KPIMFieldArrayGranularity = 8;

CPIMValidator::CPIMValidator(const TPIMAttribute& aValidAttributes) :
        iValidAttributes(aValidAttributes)
{
    JELOG2(EPim);
}

TInt CPIMValidator::NumElementsL(const TPIMField& /*aStringArrayField*/) const
{
    JELOG2(EPim);
    User::Leave(KErrArgument);
    return 0; // Never executed but keeps the compiler happy
}

void CPIMValidator::ConstructL()
{
    JELOG2(EPim);
    iValidFields
    = new(ELeave) CArrayFixFlat<TPIMFieldAndType> (KPIMFieldArrayGranularity);
}

CPIMValidator::~CPIMValidator()
{
    JELOG2(EPim);
    delete iValidFields;
}

const CArrayFix<TPIMFieldAndType>& CPIMValidator::ValidFields() const
{
    JELOG2(EPim);
    return *iValidFields;
}

TBool CPIMValidator::IsValidField(const TPIMField& aField) const
{
    JELOG2(EPim);
    return (FieldDataType(aField) != EPIMFieldInvalid);
}

TPIMFieldDataType CPIMValidator::FieldDataType(const TPIMField& aField) const
{
    JELOG2(EPim);
    const TInt n = iValidFields->Count();

    for (TInt i = 0; i < n; i++)
    {
        const TPIMFieldAndType& fieldAndType = (*iValidFields)[i];

        if (fieldAndType.iField == aField)
        {
            return (fieldAndType.iType);
        }
    }

    return EPIMFieldInvalid;
}

TBool CPIMValidator::IsValidBooleanValue(const TPIMField& /* aField */,
        const TBool& /* aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TBool CPIMValidator::IsValidDateValue(const TPIMField& /* aField */,
                                      const TPIMDate& /* aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TBool CPIMValidator::IsValidIntegerValue(const TPIMField& /* aField */,
        const TInt& /* aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TBool CPIMValidator::IsValidBinaryValue(const TPIMField& /* aField */,
                                        const CPIMByteArray& /* aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TBool CPIMValidator::IsValidStringValue(const TPIMField& /* aField */,
                                        const HBufC& /* aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TBool CPIMValidator::IsValidStringArrayValue(const TPIMField& /* aField */,
        const CDesCArray& /*aValue */) const
{
    JELOG2(EPim);
    // Default implementation

    return ETrue;
}

TPIMAttribute CPIMValidator::ValidAttributes() const
{
    JELOG2(EPim);
    return iValidAttributes;
}

//  End of File
