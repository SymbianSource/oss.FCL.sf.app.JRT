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
* Description:  Provides static information about anniversary lists and creates
 *                anniversary list adapters.
 *
*/


// INCLUDE FILES
#include "cpimannivadaptermanager.h"
#include "cpimeventlistadapter.h"
#include "cpimagnannivadapter.h"
#include "logger.h"

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::CPIMAnnivAdapterManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMAnnivAdapterManager::CPIMAnnivAdapterManager()
{
    JELOG2(EPim);
}

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMAnnivAdapterManager::ConstructL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMEventAdapterManager::ConstructL(aListName);

    iSupportedFields
    = new(ELeave) CArrayFixFlat<TPIMField> (KPIMSupportedAnnivFieldsCount);

    iSupportedFields->AppendL(KPIMSupportedAnnivFields,
                              KPIMSupportedAnnivFieldsCount);
}

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMAnnivAdapterManager* CPIMAnnivAdapterManager::NewL(const TDesC& aListName)
{
    JELOG2(EPim);
    CPIMAnnivAdapterManager* self = new(ELeave) CPIMAnnivAdapterManager();

    CleanupStack::PushL(self);
    self->ConstructL(aListName);
    CleanupStack::Pop(self);

    return self;
}

// Destructor
CPIMAnnivAdapterManager::~CPIMAnnivAdapterManager()
{
    JELOG2(EPim);
    delete iSupportedFields;
    delete iRepeatRuleFieldArrayAnniv;
    delete iRepeatRuleFieldArrayAnnivEmpty;
    delete iRepeatRuleIntervalArrayAnnivEmpty;
}

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::GetAdapterManager
// Returns: A MPIMAdapterManager representation of this object.
// -----------------------------------------------------------------------------
//
MPIMAdapterManager* CPIMAnnivAdapterManager::GetAdapterManager()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::GetSupportedRepeatRuleFields
// Provides the supported repeat rule fields for a given frequency.
// Returns: Supported repeat rule fields for \a aFrequency. If no
// fields are supported for the frequency, an empty array is returned.
// -----------------------------------------------------------------------------
//

const CArrayFix<TPIMField>&
CPIMAnnivAdapterManager::GetSupportedRepeatRuleFieldsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    const CArrayFix<TPIMField>* retVal = NULL;

    switch (aFrequency)
    {
    case EPIMRepeatRuleDaily:
    case EPIMRepeatRuleWeekly:
    case EPIMRepeatRuleMonthly:
    {
        // Return an empty array indicating no supported fields
        if (!iRepeatRuleFieldArrayAnnivEmpty)
        {
            iRepeatRuleFieldArrayAnnivEmpty = new(ELeave) CArrayFixFlat<
            TPIMField> (1);
        }
        retVal = iRepeatRuleFieldArrayAnnivEmpty;
        break;
    }
    case EPIMRepeatRuleYearly:
    {
        // Return an array with one item, the frequency
        if (!iRepeatRuleFieldArrayAnniv)
        {
            iRepeatRuleFieldArrayAnniv
            = new(ELeave) CArrayFixFlat<TPIMField> (1);
            iRepeatRuleFieldArrayAnniv->AppendL(EPIMRepeatRuleFrequency);
        }
        retVal = iRepeatRuleFieldArrayAnniv;
        break;
    }
    default:
    {
        User::Leave(KErrArgument); // Field was not a valid field
    }
    }

    return *retVal;
}

// -----------------------------------------------------------------------------
// CPIMAnnivAdapterManager::GetSupportedIntervals
// Provides the supported interval values for given frequency.
// Returns: Supported interval values for given frequency. If the
// interval field is not supported for given frequency,
// an empty array is returned.
// -----------------------------------------------------------------------------
//

const CArrayFix<TInt>& CPIMAnnivAdapterManager::GetSupportedIntervalsL(
    const TPIMRepeatRuleFrequency& aFrequency)
{
    JELOG2(EPim);
    if (aFrequency != EPIMRepeatRuleDaily && aFrequency != EPIMRepeatRuleWeekly
            && aFrequency != EPIMRepeatRuleMonthly && aFrequency
            != EPIMRepeatRuleYearly)
    {
        User::Leave(KErrArgument); // codescanner::leave
    }

    // Frequency is valid, return an empty array indicating no supported interv.
    if (!iRepeatRuleIntervalArrayAnnivEmpty)
    {
        iRepeatRuleIntervalArrayAnnivEmpty
        = new(ELeave) CArrayFixFlat<TInt> (1);  // codescanner::leave
    }
    return *iRepeatRuleIntervalArrayAnnivEmpty;
}

//  End of File
