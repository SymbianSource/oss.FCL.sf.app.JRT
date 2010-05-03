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


#ifndef CPIMANNIVADAPTERMANAGER_H
#define CPIMANNIVADAPTERMANAGER_H

// INCLUDES
#include "cpimeventadaptermanager.h"

// CONSTANTS

// Supported repeat fields
const TPIMRepeatRuleFrequency KPIMSupportedRepeatRuleFrequenciesAnniv[] =
    { EPIMRepeatRuleYearly };

const TInt KPIMSupportedRepeatRuleFrequenciesAnnivCount =
    sizeof(KPIMSupportedRepeatRuleFrequenciesAnniv)
    / sizeof(TPIMRepeatRuleFrequency);

// CLASS DECLARATION

/**
 * PIM Anniv Adapter Manager class
 */
NONSHARABLE_CLASS(CPIMAnnivAdapterManager): public CPIMEventAdapterManager
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMAnnivAdapterManager* NewL(const TDesC& aListName);

    /**
     * Destructor.
     */
    ~CPIMAnnivAdapterManager();

public: // MPIMEventAdapterManager

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    MPIMAdapterManager* GetAdapterManager();

    /**
     * Provides the supported repeat rule fields for a given frequency.
     *
     * @param aFrequency A frequency.
     *
     * @return Supported repeat rule fields for \a aFrequency. If no
     *         fields are supported for the frequency, an empty array
     *         is returned.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    const CArrayFix< TPIMField>& GetSupportedRepeatRuleFieldsL(
        const TPIMRepeatRuleFrequency& aFrequency);

    /**
     * Provides the supported interval values for given frequency.
     *
     * @param aFrequency A frequency.
     *
     * @return Supported interval values for given frequency. If the
     *         interval field is not supported for given frequency,
     *         an empty array is returned.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    const CArrayFix< TInt>& GetSupportedIntervalsL(
        const TPIMRepeatRuleFrequency& aFrequency);

private:

    /**
     * C++ default constructor.
     */
    CPIMAnnivAdapterManager();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC& aListName);

private: // Member data

    // All the following arrays are owned by CPIMAnnivAdapterManager class
    CArrayFixFlat< TPIMField>* iRepeatRuleFieldArrayAnniv;
    CArrayFixFlat< TPIMField>* iRepeatRuleFieldArrayAnnivEmpty;
    CArrayFixFlat< TInt>* iRepeatRuleIntervalArrayAnnivEmpty;
};

#endif // CPIMANNIVADAPTERMANAGER_H
// End of File

