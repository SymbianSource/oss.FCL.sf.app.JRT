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


#ifndef CPIMSTRINGMATCHER_H
#define CPIMSTRINGMATCHER_H

//  INCLUDES
#include <e32base.h>
#include "pimcommon.h"

// FORWARD DECLARATIONS
class CPIMItem;
class MPIMAdapterManager;
class CPIMValidator;

// CLASS DECLARATION
/**
 * Class for matching an item against multiple items for a match
 * defined in the PIM API PIMList.items( aMatchingItem ) method.
 * See MatchL() method.
 */
NONSHARABLE_CLASS(CPIMStringMatcher): public CBase
{
public: // Constructors and destructor

    /**
     * C++ constructor.
     *
     * @param aValidator The validator by which the string fields
     *        are distinguished from other fields.
     */
    CPIMStringMatcher(const CPIMValidator& aValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMStringMatcher();

public: // New functions

    /**
     * Matches given item against given string value.
     *
     * @return \c ETrue if \a aMatchingValue is a substring in
     *         some of the string field values of \a aTestedItem,
     *         case-insensitive; \c EFalse otherwise.
     */
    TBool MatchL(
        const TDesC& aMatchingValue,
        const CPIMItem& aTestedItem) const;

private: // Data

    /** Validator. Not owned. */
    const CPIMValidator& iValidator;

};

#endif // CPIMSTRINGMATCHER_H
// End of File
