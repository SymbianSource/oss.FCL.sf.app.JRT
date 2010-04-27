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
* Description:  Adapter manager interface, describes adapter properties.
 *
*/


#ifndef MPIMADAPTERMANAGER_H
#define MPIMADAPTERMANAGER_H

//  INCLUDES
#include <e32def.h>
#include <e32std.h>
#include <bamdesca.h>
#include "pimcommon.h"

//  FORWARD DECLARATIONS
class MPIMItemData;

// CLASS DECLARATION

/**
 *  Defines the operations common to all adapter managers and provides
 *  the characteristics of a single list.
 *
 *  List characteristics include
 *  list name, supported fields and attributes and limits of the
 *  list. The underlying native database cannot be changed through
 *  MPIMAdapterManager class.
 */
class MPIMAdapterManager
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~MPIMAdapterManager()
    {
    }

public:
    // New functions

    /**
     * Provides the name of this list.
     */
    virtual const TDesC& ListNameL() = 0;

    /**
     * Provides number of maximum categories supported by the list adapter.
     * @return Number of maximum supported categories. 0 if not supported,
     *         -1 if unlimited.
     */
    virtual TInt MaxCategories() = 0;

    /**
     * Checks whether a specific field is supported by the list adapter.
     * @param aField The field to be checked.
     * @return ETrue if the given field is supported, EFalse otherwise.
     */
    virtual TBool IsSupportedField(TPIMField aField) = 0;

    /**
     * Provides all fields supported by the list adapter.
     *
     * @return An array containing all supported fields. The order of the
     *         fields must be the same as in the native side application
     *         UI.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the adapter
     * is non-functional.
     */
    virtual const CArrayFix<TPIMField>& GetSupportedFieldsL() = 0;

    /**
     * Checks whether a specific attribute is supported by the list adapter.
     *
     * @param aField The field to which the attribute might be associated.
     * @param aAttribute The attribute to be checked.
     *
     * @return ETrue if the given attribute is supported, EFalse otherwise,
     *         also on unsupported \a aField.
     */
    virtual TBool IsSupportedAttribute(TPIMField aField,
                                       TPIMAttribute aAttribute) = 0;

    /**
     * Provides attributes supported for a field.
     *
     * @param aField The field to which the attributes are associated with.
     *
     * @return An array containing attributes supported for the \a aField.
     *         The order of attributes must reflect the order of field and
     *         attribute combinations in the native application UI.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li Other - Array could not be created.
     */
    virtual const CArrayFix<TPIMAttribute>& GetSupportedAttributesL(
        TPIMField aField) = 0;

    /**
     * Provides attributes supported for a field, combined into single
     * integer value.
     *
     * @param aField The field to which the attributes are associated with.
     *
     * @return An integer holding the combination of all attributes
     *         supported for \a aField.
     *
     * @par Leaving.
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     */
    virtual TPIMAttribute
    GetSupportedAttributesCombinedL(TPIMField aField) = 0;

    /**
     * Provides all attributes supported for all fields, combined into
     * single integer value.
     *
     * @return An integer holding the combination of all attributes
     *         supported for all fields.
     */
    virtual TPIMAttribute GetAllSupportedAttributesCombined() = 0;

    /**
     * Checks whether a specific array element is supported by the
     * list adapter.
     *
     * @param aStringArrayField The field to which the array
     *        element belongs to.
     * @param aArrayElement The array element to be checked.
     *
     * @return ETrue if the given array element is supported,
     *         EFalse otherwise.
     */
    virtual TBool IsSupportedArrayElement(TPIMField aStringArrayField,
                                          TPIMArrayElement aArrayElement) = 0;

    /**
     * Provides supported array elements for a specific field.
     *
     * @param aStringArrayField A string array field.
     *
     * @return An array containing supported array elements for the field.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid.
     * @li Other - Array could not be created.
     */
    virtual const CArrayFix<TPIMArrayElement>& GetSupportedArrayElementsL(
        TPIMField aStringArrayField) = 0;

    /**
     * Provides the number of maximum values supported for a specific field.
     *
     * @param aField The field to be evaluated.
     *
     * @return Number of values supported for the field. -1 for unlimited,
     *         0 if not supported.
     */
    virtual TInt MaxValues(TPIMField aField) = 0;

    /**
     * Provides the array size of a specific string array field.
     * The values in a string array field are arrays themselves.
     * StringArraySize method returns the number of elements in every value
     * of the field.
     *
     * @param aStringArrayField The string array field to be evaluated.
     * @return Number of elements in a single string array value.
     *
     * @par Leaving:
     * The method leaves with \c KErrArgument if \a aStringArrayField
     * is not a string array field or is not supported.
     */
    virtual TInt StringArraySizeL(TPIMField aStringArrayField) = 0;

    /**
     * Provides pointer to a function which implements an algorithm that
     * determines the order of two items.
     *
     * @return Item comparison function.
     */
    virtual TPIMItemComparisonFunc ItemOrder() = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMAdapterManager()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMAdapterManager(const MPIMAdapterManager&)
    {
    }

    // Prohibit assigment operator.
    MPIMAdapterManager& operator=(const MPIMAdapterManager&)
    {
        return *this;
    }

};

#endif // MPIMADAPTERMANAGER_H
// End of File
