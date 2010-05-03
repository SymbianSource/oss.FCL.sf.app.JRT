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
* Description:  Holds Location API address information
 *
*/


#ifndef CLAPIADDRESSINFO_H
#define CLAPIADDRESSINFO_H

// EXTERNAL INCLUDES
#include    <e32base.h>

/**
 * Holds Location API address information
 *
 * Basically this class is a value and identifier holder which maps
 * each value to a specific field index. The values can be obtained
 * using these indexes
 *
 * Values can be null or non-null
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPIAddressInfo) : public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @return New instance from this class
     */
    static CLAPIAddressInfo* NewL();

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     *
     * @return New instance from this class
     */
    static CLAPIAddressInfo* NewLC();

    /**
     * Destructor.
     */
    virtual ~CLAPIAddressInfo();

public: // New methods

    /**
     * Returns the value count of this address information
     *
     * The count includes both, null and non-null elements
     *
     * @return The number of values in this address information object
     */
    TInt ValueCount() const;

    /**
     * Sets an address information value to a specific field id
     *
     * The existing value will be overwritten or a new value will
     * be added if the field identifier does not exist
     *
     * @param aFieldId The identifier of the value
     * @param aValue The value which maps to the identifier. The
     *        ownership of the value is NOT tranferred to this class
     */
    void SetValueL(TInt aFieldId, const TDesC* aValue);

    /**
     * Returns the value of the given field identifier
     *
     * A null value is returned if there is no value which maps
     * to the given identifier. The ownership of the value is NOT
     * transferred to the caller
     *
     * @param aFieldId The field id which value should be returned
     * @return The value which maps to the given indentifier. Null
     *         is returned if there is no value which maps to the
     *         given identifier
     */
    const TDesC* Value(TInt aFieldId) const;

private: // Constructors

    /**
     * Constructor
     */
    CLAPIAddressInfo();

private: // Data

    // Values. Owned
    RPointerArray< HBufC> iValues;
    // Ids. Owned
    RArray< TInt> iIds;

};

#endif // CLAPIADDRESSINFO_H
// End of file
