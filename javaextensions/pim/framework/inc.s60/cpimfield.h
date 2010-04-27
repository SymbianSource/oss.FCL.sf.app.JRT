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
* Description:  Class for holding PIM field data
 *
*/


#ifndef CPIMFIELD_H
#define CPIMFIELD_H

// INTERNAL INCLUDES
#include "tpimfielddata.h"

// EXTERNAL INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CPIMValueBase;

/**
 * Class for holding PIM field data. This class
 * stores value indexes to an array.
 * @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMField) : public CBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class.
     */
    static CPIMField* NewL(const TPIMFieldData& aFieldData);

    /**
     * Two-phased constructor.
     *
     * @return New instance of this class. The instance
     *         is pushed to the cleanup stack
     */
    static CPIMField* NewLC(const TPIMFieldData& aFieldData);

    /**
     * Destructor
     */
    virtual ~CPIMField();

public: // New methods

    /**
     * Sets PIM field data with certain index
     *
     * This function is used to set all types of PIM
     * data. The TPIMFieldData class encapsulates all
     * possible data about one field so we don't need
     * separate functions for each PIM field type
     *
     * @param aFieldData Reference to data class which
     *        all needed information about the new value
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void SetValueL(const TPIMFieldData& aFieldData);

    /**
     * Adds new PIM field data to certain field
     *
     * This function is used to add all types of PIM
     * data. The TPIMFieldData class encapsulates all
     * possible data about one field so we don't need
     * separate functions for each PIM field type
     *
     * NOTE: The index value in the argument class is ignored
     *
     * @param aFieldData Reference to data class which
     *        all needed information about the new value
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void AddValueL(const TPIMFieldData& aFieldData);

    /**
     * Fetches value from specific value index.
     *
     * @param aIndex Index of the value in this specific field
     * @return A class holding requested information
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    const TPIMFieldData ValueL(const TInt aIndex) const;

    /**
     * Removes a specific value of a specific field.
     *
     * @param aIndex Index of the value that is to be removed.
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void RemoveValueL(const TInt aIndex);

    /**
     * Count values currently in this PIM field
     *
     * @return Number of values in this field
     */
    TInt CountValues() const;

    /**
     * Adds new label to an existing field
     *
     * @param aIndex Value index of the label
     * @param aArrayIndex Array index of the label
     * @param aLabel New label
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void SetLabelL(
        const TInt aIndex,
        const TInt aArrayIndex,
        const TDesC& aLabel);

    /**
     * Retrieves the label from the specific field with specific
     * field and array indexes.
     *
     * @param aIndex Index of the field's value
     * @param aArrayIndex Index of the array index if this is a string
     *        array field
     *
     * @return Label name. NULL if there is no label for this array
     *         index or the labels have not been allocated yet
     */
    const TPtrC Label(
        const TInt aIndex,
        const TInt aArrayIndex) const;

    /**
     * DataType
     * Returns the data type of this PIM field
     *
     * @return Data type of this field
     */
    TPIMFieldDataType DataType() const;

    /**
     * FieldType
     * Returns the type of this field
     *
     * @return The type of this field
     */
    TPIMField FieldType() const;

    /**
     * SetAttributesL
     * Sets new attributes for a specific value
     *
     * @param aIndex Index of the value
     * @param aAttributes New attributes
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void SetAttributesL(const TInt aIndex, TPIMAttribute aAttributes);

    /**
     * AttributesL
     * Returns attributes from the specific value
     *
     * @param aIndex Index of the value
     * @return Attributes of the value
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    TPIMAttribute AttributesL(const TInt aIndex) const;

    /**
     * SetInternalAttributesL
     * Sets new internal attributes for a specific value. NOTE:
     * Internal attributes are not accessible from the Java-side
     * They only store information which must be preserved when item
     * is loaded from the native database and modified before storing
     *
     * @param aIndex Index of the value
     * @param aAttributes New attributes
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    void SetInternalAttributesL(
        const TInt aIndex,
        CArrayFix< TUid>* aInternalAttributes);

    /**
     * InternalAttributesL
     * Returns internal attributes from the specific value. NOTE:
     * Internal attributes are not accessible from the Java-side
     * They only store information which must be preserved when item
     * is loaded from the native database and modified before storing
     *
     * @param aIndex Index of the value
     * @return Attributes of the value
     *
     * @par Leaving:
     * The method leaves on error. Error codes are interpreted as follows:
     * @li \c KErrNotFound - There is not value for given \a aIndex.
     * @li Other - The value could not be removed cleanly. This probably
     *     means that the value array has gone corrupt or OOM situation
     *     has occured
     */
    const CArrayFix< TUid>& InternalAttributesL(const TInt aIndex) const;

private: // New methods

    /**
     * Checks that the given index is in the value
     * range.
     *
     * @param aIndex Index which is to be checked
     * @return ETrue if the index is in range, EFalse if not
     */
    TBool IsInRange(const TInt aIndex) const;

private:

    /**
     * C++ default constructor
     */
    inline CPIMField(const TPIMFieldData& aFieldData);

private: // Data

    /** Type of this field */
    TPIMField iField;

    /** Data type of this field */
    TPIMFieldDataType iFieldDataType;

    /** PIM value array */
    RPointerArray< CPIMValueBase> iValues;

};

#endif // CPIMFIELD_H
// End of file
