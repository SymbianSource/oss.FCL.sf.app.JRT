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
* Description:  Provides static information about event lists and creates
 *                event list adapters.
 *
*/


// INCLUDE FILES
#include "cpimmemoadaptermanager.h"
#include "cpimeventlistadapter.h"
#include "cpimagnmemoadapter.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::CPIMMemoAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CPIMMemoAdapterManager::CPIMMemoAdapterManager()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void CPIMMemoAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMEventAdapterManager::ConstructL(aListName);

    iSupportedFields
    = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedMemoFieldsCount);

    iSupportedFields->AppendL(KPIMSupportedMemoFields,
                              KPIMSupportedMemoFieldsCount);
}

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CPIMMemoAdapterManager* CPIMMemoAdapterManager::NewL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMMemoAdapterManager* self = new(ELeave) CPIMMemoAdapterManager();

    CleanupStack::PushL(self);
    self->ConstructL(aListName);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMMemoAdapterManager::~CPIMMemoAdapterManager()
{
    JELOG2(EPim);
    delete iSupportedFields;
    delete iRepeatRuleFieldArrayMemoEmpty;
}

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::GetAdapterManager
// Returns: A MPIMAdapterManager representation of this object.
// -----------------------------------------------------------------------------
//

MPIMAdapterManager* CPIMMemoAdapterManager::GetAdapterManager()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::GetSupportedRepeatRuleFields
// Provides the supported repeat rule fields for a given frequency.
// Returns: Supported repeat rule fields for \a aFrequency. If no
// fields are supported for the frequency, an empty array is returned.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMField>&
CPIMMemoAdapterManager::GetSupportedRepeatRuleFieldsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    if (aFrequency != EPIMRepeatRuleDaily && aFrequency != EPIMRepeatRuleWeekly
            && aFrequency != EPIMRepeatRuleMonthly && aFrequency
            != EPIMRepeatRuleYearly)
    {
        User::Leave(KErrArgument); // invalid frequency
    }

    // Frequency is valid, return an empty array indicating no supported fields
    if (!iRepeatRuleFieldArrayMemoEmpty)
    {
        iRepeatRuleFieldArrayMemoEmpty = new(ELeave) CArrayFixFlat<TInt> (1);
    }
    return *iRepeatRuleFieldArrayMemoEmpty;
}

// -----------------------------------------------------------------------------
// CPIMMemoAdapterManager::GetSupportedIntervals
// Provides the supported interval values for given frequency.
// Returns: Supported interval values for given frequency. If the
// interval field is not supported for given frequency,
// an empty array is returned.
// -----------------------------------------------------------------------------
//

const CArrayFix<TInt>& CPIMMemoAdapterManager::GetSupportedIntervalsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    // As there are no supported intervals, the GetSupportedRepeatRuleFieldsL
    // method can be used to check the frequency and to return an empty array
    return GetSupportedRepeatRuleFieldsL(aFrequency);
}

//  End of File
