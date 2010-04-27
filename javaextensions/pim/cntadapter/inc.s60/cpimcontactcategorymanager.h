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
* Description:  Handles Contacts Model Group <-> PIM API Categories
 *                -conversions and manages group information
 *
*/


#ifndef CPIMCONTACTCATEGORYMANAGER_H
#define CPIMCONTACTCATEGORYMANAGER_H

// INCLUDE FILES
#include <e32def.h>
#include <e32base.h>
#include <badesca.h>
#include <cntdef.h>
#include "functionserver.h"

// FORWARD DECLARATIONS
class CContactDatabase;

// CLASS DECLARATION

/**
 *  This class handles mappings of contacts to categories.
 *  It is used by both list and item adapters to handle category changes.
 */

NONSHARABLE_CLASS(CPIMContactCategoryManager): public CBase
{
public: // Constructors and a destructor

    /**
     * Two-phased constructor.
     */
    static CPIMContactCategoryManager* NewL(CContactDatabase& aDatabase,
    java::util::FunctionServer* aFuncServer);

    /**
     * destructor
     */
    ~CPIMContactCategoryManager();

public: // New functions

    /**
     * Adds a contact to a group.
     *
     * @param aId Id of the contact to add
     * @param aGroup Id of the group to add to
     *
     * @par Leaving:
     *  - KErrNotFound: Either the group or the item does not exist
     *  - KErrNotSupported: The group is not a group.
     */
    void AddToGroupL(TUint32 aId, TUint32 aGroup);

    /**
     * Adds a contact to a group.
     * If no such group exists, nothing is done.
     *
     * @param aId Id of the contact to add
     * @param aGroup name of the group to add to
     *
     * @par Leaving:
     *  - KErrNotFound: There is something wrong with the groups
     *                  in the database
     *  - Other system wide error codes are also possible
     */
    void AddToGroupL(TUint32 aId, const TDesC& aGroup);

    /**
     * Fetches the name of a group.
     *
     * @param aGroupId Id of the group to fetch
     *
     * @return Name of the group. Caller takes the ownership of the object.
     *
     * @par Leaving:
     *  - KErrNotFound: The specified group does not exist in the database,
     *                  is not a group, or does not have a label.
     *  - Other system wide error codes are also possible.
     */
    HBufC* GroupLabelL(TContactItemId aGroupId);

    /**
     * Removes an item from a group
     *
     * @param aId Id of the item to remove
     * @param aGroup Id of the group to remove from
     *
     * @par Leaving:
     * This method leaves on error with system wide error codes
     * @par NOTE!
     * This method panics if the item does not belong to the group, or
     * if the group is not a group.
     */
    void RemoveFromGroupL(TUint32 aId, TUint32 aGroup);

    /**
     * This method should be called whenever there has been a change
     * in the category information in the database, so the
     * category manager knows to throw away any cached information.
     * The cache is flushed at the beginning of the next call to
     * GetCategories()
     */
    void FlushCache();

    const CDesCArray& CallGetCategoriesL();

    /**
     * Provides all categories currently existing in the native database.
     *
     * @return Array of categories. The contents of the array are not
     *         meant to be modified. The contents of the array may change
     *         when any method of the category manager is called.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the
     * category manager is non-functional
     */
    const CDesCArray& GetCategoriesL();

    /**
     * Adds a new category to the native database. If the category already
     * exists, nothing is done and the method returns successfully.
     *
     * @param aNewCategory The name of the category to be added.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li Other - The list adapter is non-functional.
     */
    void AddCategoryL(const TDesC& aNewCategory);

    /**
     * Deletes an existing category. If there is no such category, nothing
     * is done and the method returns successfully.
     *
     * @param aCategory The category to be deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrNotFound - \a aCategory is invalid.
     * @li Other - The list adapter is non-functional.
     */
    void DeleteCategoryL(const TDesC& aCategory);

    /**
     * Renames an existing category. Entries in the old category are moved
     * to the new category.
     *
     * @param aOldCategory The old category name.
     * @param aNewCategory The new category name.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotFound - \a aOldCategory does not exist.
     * @li Other - The list adapter is non-functional.
     */
    void RenameCategoryL(const TDesC& aOldCategory,
                         const TDesC& aNewCategory);

    /**
     * Fetches a category name from the cache. This method does not
     * reread the cache even if it has been flushed. In effect, this method
     * returns the /b old name for the category, if the category name
     * has been changed.
     *
     * @param aCategoryID ID of the category which name is to be fetched
     *
     * @return name of the category, or null if it can not
     *         be found. Caller takes the ownership.
     */
    HBufC* LabelFromCacheL(TContactItemId aCategoryID);

private:

    /**
     * C++ default constructor.
     */
    inline CPIMContactCategoryManager(CContactDatabase& aDatabase,
                                      java::util::FunctionServer* aFuncServer);

    /**
     * Symbian 2nd phase construcor.
     */
    inline void ConstructL();

    /**
     * Checks whether the given category exists
     * @return ETrue it the category exists, EFalse otherwise
     * @par Leaving:
     * The method might leave if the system has run out of memory.
     */
    TBool IsCategoryL(const TDesC& aCategory);

    void RefreshCategoriesL();

private: // data
    // contacts model database
    CContactDatabase& iDatabase;
    // array of category names, owned
    CDesCArrayFlat* iCategories;
    // array of category ids, in the same order, owned.
    CContactIdArray* iIDs;
    // If the cache should be reread before using
    TBool iCacheFlushed;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMCONTACTCATEGORYMANAGER_H

// End of File
