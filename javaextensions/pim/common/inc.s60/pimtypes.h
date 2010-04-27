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
* Description:  PIM API common types and constants
 *
*/


#ifndef PIMTYPES_H
#define PIMTYPES_H

/** @file
 * This file contains common definitions and constants for the PIM API
 * implementation.
 */

//  INCLUDE FILES
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>

//  FORWARD DECLARATIONS
class MPIMItem;

/**
 * Item ID type. Item ID identifies a \ref MPIMItemData object uniquely in
 * a native database. The value is interpreted in adapters during item read
 * and write operations. See \ref KPIMNullItemID.
 *
 * @par Notes:
 * @li The size of the ItemID type affects the size of \ref TPIMItemIdDes
 *     type.
 */
typedef TPtrC8 TPIMItemID;

/**
 * Length of the largest Item ID number in the string form. Used when
 * an Item ID needs to be written to a descriptor.
 *
 * @par Notes:
 * @li KPIMItemIdDesSize depends on the size of
 *     \ref TPIMItemID type. A descriptor must be large enough to contain
 *     the largest possible string in the \ref TPIMItemID type.
 * @li The length is referred to calendar where the maximum length of the
 *     descriptor is 32 bytes and this is also MS Windows standard.
 */
const TInt KPIMItemIdDesSize = 32;

/**
 * Null Item ID. Indicates that no Item ID has been set for the
 * \ref MPIMItemData object.
 *
 * @par Notes:
 * @li It is essential that the value of the null ID is zero.
 *     Otherwise blank item initialization (with automatically
 *     zero-initialized members by CBase) will fail in current
 *     implementation.
 *
 */
_LIT8(KPIMNullItemID, "");

/**
 * Field type. Fields are presented as symbolic constants.
 */
typedef TInt TPIMField;

/**
 * Attribute type. Attributes are presented as bit-flags. Several attributes
 * can be combined with bitwise operations.
 */
typedef TInt TPIMAttribute;

/**
 * Array element type. Array elements are presented as index values.
 */
typedef TInt TPIMArrayElement;

/**
 * Byte array type. Used for storing binary values, such as pictures.
 */
typedef CArrayFixFlat<TUint8> CPIMByteArray;

/**
 * Date type. Used for storing time and date values. Converted to
 * java.util.Time format in the Framework.
 */
typedef TTime TPIMDate;

/**
 * Localization data identifier type. Lists provided by adapters are mapped
 * to localization data sets by IDs of this type.
 */
typedef TInt TPIMLocalizationDataID;

/**
 * Expresses unlimited number of values for particular field.
 * [From javax.microedition.pim.PIMList]
 */
const TInt KPIMUnlimitedValues = -1;

/**
 * Expresses unlimited number of categories in particular list.
 * [From javax.microedition.pim.PIMList]
 */
const TInt KPIMUnlimitedCategories = -1;

/**
 * Literal that represents a "null" array element.
 * It is practical to implement the PIM string arrays as CDesCArrays, but we
 * cannot set null elements to such array. Because in a PIM string array
 * a single element can be either NULL or String, and an empty String ("")
 * is valid content for an element, we cannot mark empty array elements
 * with it. Let's make it short and hard to guess.
 *
 * The value of this literal MUST be the same as
 * PIMItemImpl.PIM_NULL_ARRAY_ELEMENT.
 */
_LIT(KPIMNullArrayElement, "T&%3#");

/**
 * Literal that represents a zero value
 * This separator is used in string array labels where it separates
 * label names from each other
 */

_LIT(KPIMNullSeparator, "\0");

/**
 * Pointer to a function which compares two MPIMItemData objects, determining
 * their ordering in an item enumeration.
 *
 * Returns:
 * @li < 0, if first object becomes \b before the second
 * @li 0, if the objects are equal in the ordering
 * @li > 0, if the firs object becomes \b after the second.
 */
typedef TInt(*TPIMItemComparisonFunc)(const MPIMItem&, const MPIMItem&);

/**
 * Macro for checking whether an integer value has one or zero bits set.
 *
 * @param X Integer to be tested.
 *
 * @return True if X has just one bit set or X is 0; false otherwise.
 */
#define IS_SINGLE_BIT_OR_ZERO(X) ((X&(X-1))==0)

/**
 * Macro for checking whether an integer value has one and only one bit set.
 *
 * @param X Integer to be tested.
 *
 * @return True if X has one bit set; false otherwise.
 */
#define IS_SINGLE_BIT(X) (X?((X&(X-1))==0):EFalse)

#endif // PIMTYPES_H
// End of File
