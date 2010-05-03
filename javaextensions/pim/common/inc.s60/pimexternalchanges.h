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
* Description:  Native-originated external change data.
 *
*/


#ifndef PIMEXTERNALCHANGES_H
#define PIMEXTERNALCHANGES_H

/** @file
 * This file contains definitions and helper classes for passing external
 * changes from the adapters to the Framework.
 */

#include <e32base.h>
#include "pimcommon.h"
#include "pimpanics.h"

/**
 * External change types.
 */
typedef enum
{
    EPIMExternalChangeNew,
    EPIMExternalChangeModified,
    EPIMExternalChangeRemoved
} TPIMExternalChangeType;

/**
 * Simple class for passing external modifications to entries (items) in
 * a native database. The class encapsulates Item ID and type of change for
 * single item. Adapters produce these objects and Framework consumes them.
 */
NONSHARABLE_CLASS(CPIMItemStateChange) : public CBase
{
public: // Constructors and destructor

    /**
     * Constructor.
     * Object data is explicitly set with this constructor. No default
     * constructor is provided.
     * @param aItemID ID of the item this change applies to. The ownership
     *        of the value is transferred to this class
     * @param aChangeType Type of change for the associated item.
     */
    CPIMItemStateChange(
        HBufC8* aItemID,
        TPIMExternalChangeType aChangeType)
            :
            iItemID(aItemID),
            iChangeType(aChangeType)
    {
        __ASSERT_DEBUG(iItemID || *iItemID != KPIMNullItemID,
        User::Panic(KPIMPanicCategory,
        EPIMPanicExternalChangeInit));
    }

    /**
     * Destructor.
     */
    ~CPIMItemStateChange()
    {
        delete iItemID;
    }

    /**
     * Provides ID of the changed item.
     * @return Item ID.
     */
    TPIMItemID ItemID() const
    {
        return *iItemID;
    }

    /**
     * Provides type of change for the associated item.
     * @return Change type.
     */
    TPIMExternalChangeType ChangeType() const
    {
        return iChangeType;
    }

private: // Prohibited operations

    // Prohibit default constructor
    CPIMItemStateChange()
    {}

private: // Data

    HBufC8* iItemID; // Owned
    TPIMExternalChangeType iChangeType;

};

/**
 * Simple class for passing external modifications to categories in
 * a native database. The class encapsulates category name and type of change
 * for single item. Adapters produce these objects and Framework consumes
 * them. \b Note that the object contains heap-allocated data.
 */
NONSHARABLE_CLASS(CPIMCategoryStateChange) : public CBase
{
public:

    //@{
    /**
     * Constructors.
     * Object data is explicitly set with this constructor. No default
     * constructor is provided.
     * @param aCategory Name of the category this change applies to.
     *        The object takes the ownership of the argument. The
     *        argument must not be NULL; otherwise a panic is raised.
     * @param aNewCategory Name of the new category if \a aChangeType
     *        is EPIMExternalChangeModified, in which case it must not
     *        be NULL; otherwise it must be NULL.
     * @param aChangeType Type of change for the associated item.
     */
    CPIMCategoryStateChange(
        HBufC* aCategory,
        TPIMExternalChangeType aChangeType)
            :
            iCategory(aCategory),
            iChangeType(aChangeType)
            // iNewCategoryName is left NULL

    {
        __ASSERT_DEBUG(iCategory,
        User::Panic(KPIMPanicCategory,
        EPIMPanicExternalChangeInit));
    }

    CPIMCategoryStateChange(
        HBufC* aCategory,
        TPIMExternalChangeType aChangeType,
        HBufC* aNewCategoryName)
            :
            iCategory(aCategory),
            iChangeType(aChangeType),
            iNewCategoryName(aNewCategoryName)
    {
        __ASSERT_DEBUG(iCategory,
                       User::Panic(KPIMPanicCategory,
                                   EPIMPanicExternalChangeInit));

#if _DEBUG
        if (aChangeType == EPIMExternalChangeModified)
        {
            __ASSERT_DEBUG(
                iNewCategoryName,
                User::Panic(KPIMPanicCategory,
                            EPIMPanicExternalChangeInit));
        }
        else
        {
            __ASSERT_DEBUG(
                !iNewCategoryName,
                User::Panic(KPIMPanicCategory,
                            EPIMPanicExternalChangeInit));
        }
#endif // _DEBUG
    }
    //@}

    /**
     * Destructor. Deletes the owned category name and new name
     * of that category, if present.
     */
    ~CPIMCategoryStateChange()
    {
        delete iCategory;
        delete iNewCategoryName;
    }

    /**
     * Provides name of the changed category.
     * @return Category name.
     */
    const TDesC& Category() const
    {
        return *iCategory;
    }

    /**
     * Provides the new name of a renamed category.
     *
     * @return New name of a renamed category if change type is
     *         EPIMExternalChangeModified.
     *
     * @par Leaving:
     * @li \c KErrGeneral - Change type is not
     *     EPIMExternalChangeModified.
     */
    const TDesC& NewCategoryNameL() const
    {
        if (!iNewCategoryName)
        {
            User::Leave(KErrGeneral);
        }

        return *iNewCategoryName;
    }

    /**
     * Provides type of change for the associated item.
     * @return Change type.
     */
    TPIMExternalChangeType ChangeType() const
    {
        return iChangeType;
    }

private: // Prohibited operations

    // Prohibit default constructor
    CPIMCategoryStateChange()
    {}

private: // Data
    HBufC* iCategory; // owned
    TPIMExternalChangeType iChangeType;
    HBufC* iNewCategoryName; // owned
};

#endif // PIMEXTERNALCHANGES_H
// End of File
