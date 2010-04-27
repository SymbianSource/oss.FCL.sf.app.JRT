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
* Description:  An item string value matcher.
 *
*/


// INCLUDE FILES
#include  "cpimstringmatcher.h"
#include  "mpimadaptermanager.h"
#include  "cpimvalidator.h"
#include  "cpimitem.h"
#include  "pimpanics.h"
#include "logger.h"

CPIMStringMatcher::CPIMStringMatcher(const CPIMValidator& aValidator) :
        iValidator(aValidator)
{
    JELOG2(EPim);
}

CPIMStringMatcher::~CPIMStringMatcher()
{
    JELOG2(EPim);
}

TBool CPIMStringMatcher::MatchL(const TDesC& aMatchingValue,
                                const CPIMItem& aTestedItem) const
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    CArrayFix<TPIMField>* fields = aTestedItem.GetFieldsL();
    CleanupStack::PushL(fields);
    const TInt numFields = fields->Count();

    for (TInt fieldIndex = 0; !retVal && (fieldIndex < numFields); fieldIndex++)
    {
        const TPIMField field = fields->At(fieldIndex);
        TPIMFieldDataType fieldDataType = iValidator.FieldDataType(field);

        // Take only string and string array fields into account
        if (fieldDataType == EPIMFieldString)
        {
            const TInt numValues = aTestedItem.CountValuesL(field);

            for (TInt valueIndex = 0; !retVal && (valueIndex < numValues); valueIndex++)
            {
                const TDesC& stringValue = aTestedItem.GetStringL(field,
                                           valueIndex);

                // Find substring, collated (case-insensitive and
                // discarding punctuation)
                if (stringValue.FindC(aMatchingValue) >= 0)
                {
                    retVal = ETrue;
                }
            }
        }
        else if (fieldDataType == EPIMFieldStringArray)
        {
            const TInt numValues = aTestedItem.CountValuesL(field);

            for (TInt valueIndex = 0; !retVal && (valueIndex < numValues); valueIndex++)
            {
                const CDesCArray& stringArrayValue =
                    aTestedItem.GetStringArrayL(field, valueIndex);

                const TInt numElems = stringArrayValue.Count();
                for (TInt elemIndex = 0; !retVal && (elemIndex < numElems); elemIndex++)
                {
                    // Find substring from a single element, collated
                    // (case-insensitive and discarding punctuation)
                    TPtrC element = stringArrayValue[elemIndex];
                    if (element.FindC(aMatchingValue) >= 0)
                    {
                        retVal = ETrue;
                    }
                }
            }
        }
    }

    CleanupStack::PopAndDestroy(fields);
    return retVal;
}

//  End of File
