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
* Description:  Interface for Location API landmark store category handling
 *
*/


#ifndef MLAPICATEGORYMANAGER_H
#define MLAPICATEGORYMANAGER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32def.h>
#include    <badesca.h>

// FORWARD DECLARATIONS
class MLAPILandmark;
class MLAPILandmarkStore;

/**
 * Interface to Location API landmark store category handling
 *
 * The interface provides functionalities to perform category operations
 * such as adding, deleting or updating. The inferface also provides a
 * functionality to list current available categories
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(MLAPICategoryManager)
{
public: // Destructor

    /**
     * Destructor. Allows destruction through this interface
     */
    virtual ~MLAPICategoryManager()
    {}

public: // New functions

    /**
     * Lists the current available categories in the store
     *
     * @return List of categories. An empty array is returned if there
     *         are no categories in the native category store
     */
    virtual const CDesCArray& ListCategoriesL() = 0;

    /**
     * Adds a new category to the store. The operation leaves
     * with KErrAlreadyExists if a category with the same name
     * has already been added to the store
     *
     * @param aCategoryName The name of the new category
     */
    virtual void AddCategoryL(const TDesC& aCategoryName) = 0;

    /**
     * Removes a existing category from the store. The operation
     * leaves with KErrNotFound if the requested category does not
     * exist in the store
     *
     * @param aCategoryName The name of the removed category
     */
    virtual void RemoveCategoryL(const TDesC& aCategoryName) = 0;

    /**
     * Adds a landmark to the specified category. If the requested
     * category does not exist, this operation leaves with
     * KErrArgument.
     *
     * @param aLandmark The landmark which will be added to the category
     * @param aCategoryName The name of the category to which the landmark
     *        will be added
     */
    virtual void AddLandmarkToCategoryL(MLAPILandmark& aLandmark,
                                        const TDesC& aCategoryName) = 0;

    /**
     * Removes a landmark item from a category. The call is silently ignored
     * if the specified category does not exist in the landmark database
     *
     * @param aLandmark The landmark which will be removed from the category
     * @param aCategoryName The name of the category from which the landmark
     *        will be removed
     */
    virtual void RemoveLandmarkFromCategoryL(MLAPILandmark& aLandmark,
            const TDesC& aCategoryName) = 0;

}
;

#endif // MLAPICATEGORYMANAGER_H
// End of file
