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


#ifndef CPIMMEMOADAPTERMANAGER_H
#define CPIMMEMOADAPTERMANAGER_H

// INCLUDES
#include "cpimeventadaptermanager.h"

// CLASS DECLARATION

/**
 * PIM Memo Adapter Manager class
 */
NONSHARABLE_CLASS(CPIMMemoAdapterManager): public CPIMEventAdapterManager
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMMemoAdapterManager* NewL(const TDesC& aListName);

    /**
     * Destructor.
     */
    ~CPIMMemoAdapterManager();

public: // New functions

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
    CPIMMemoAdapterManager();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC& aListName);

private: // Member data

    // Array is owned by CPIMMemoAdapterManager class
    CArrayFixFlat< TPIMField>* iRepeatRuleFieldArrayMemoEmpty;
};

#endif // CPIMMEMOADAPTERMANAGER_H
// End of File

