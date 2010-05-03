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
* Description:  Event adapter interface, provides access to an Event list.
 *
*/


#ifndef MPIMEVENTADAPTERMANAGER_H
#define MPIMEVENTADAPTERMANAGER_H

//  INCLUDES
#include "mpimadaptermanager.h"
#include "pimrepeatrule.h"

//  FORWARD DECLARATIONS
class MPIMEventListAdapter;

// CLASS DECLARATION

/**
 *  Provides access to a single todo list. See also \ref MPIMAdapterManager
 *  class description.
 */
class MPIMEventAdapterManager
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMEventAdapterManager()
    {
    }

public:
    // New functions

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    virtual MPIMAdapterManager* GetAdapterManager() = 0;

    /**
     * Provides the supported repeat rule fields for a given frequency.
     *
     * @param aFrequency A frequency.
     *
     * @return Supported repeat rule fields for \a aFrequency.
     *         If \a aFrequency is not supported, an empty array
     *         is returned.
     *         If \a aFrequency is supported but no fields are
     *         supported for it, an array with one element, a zero,
     *         is returned.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aFrequency is not a valid frequency.
     */
    virtual const CArrayFix<TPIMField>& GetSupportedRepeatRuleFieldsL(
        const TPIMRepeatRuleFrequency& aFrequency) = 0;

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
    virtual const CArrayFix<TInt>& GetSupportedIntervalsL(
        const TPIMRepeatRuleFrequency& aFrequency) = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMEventAdapterManager()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMEventAdapterManager(const MPIMEventAdapterManager&)
    {
    }

    // Prohibit assigment operator.
    MPIMEventAdapterManager& operator=(const MPIMEventAdapterManager&)
    {
        return *this;
    }

};

#endif // MPIMEVENTADAPTERMANAGER_H
// End of File

