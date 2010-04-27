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
* Description:  Handles landmark database category operations.
 *
*/


#ifndef CLAPICATEGORYMANAGER_H
#define CLAPICATEGORYMANAGER_H

// INTERNAL INCLUDES
#include    "mlapicategorymanager.h"
#include    "mlapilandmark.h"
#include    "mlapilmdatabaseobserver.h"

// EXTERNAL INCLUDES
#include    <e32def.h>
#include    <e32base.h>
#include    <badesca.h>
#include    <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLmCategoryManager;
class CPosLandmarkDatabase;
class MLAPILandmarkStore;
class MLAPILmDatabaseEventNotifier;

/**
 * Handles landmark database category operations.
 *
 * Categories can be added, removed or updated using this class. A landmark
 * can be added to a category and also removed.
 *
 * The class constantly monitors changes in the native database if a
 * category is deleted or added. The changes are updated immediately
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPICategoryManager) : public CBase,
        public MLAPICategoryManager,
        public MLAPILmDatabaseObserver
{
public: // Type definitions

    // Construction parameters of this class
    class TCtorParams
    {
    public:
        CPosLandmarkDatabase* iLandmarkDatabase; // Ref
        MLAPILmDatabaseEventNotifier* iEventNotifier; // Uses
    };

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aParams Construction parameters
     * @return New instance from this class
     */
    static CLAPICategoryManager* NewL(const TCtorParams& aParams);

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     *
     * @param aParams Construction parameters
     * @return New instance from this class
     */
    static CLAPICategoryManager* NewLC(const TCtorParams& aParams);

    /**
     * Destructor.
     */
    virtual ~CLAPICategoryManager();

public: // From MLAPICategoryManager

    /**
     * Lists the current available categories in the store
     *
     * @return List of categories. An empty array is returned if there
     *         are no categories in the native category store
     */
    const CDesCArray& ListCategoriesL();

    /**
     * Adds a new category to the store. The operation leaves
     * with KErrAlreadyExists if a category with the same name
     * has already been added to the store
     *
     * @param aCategoryName The name of the new category
     */
    void AddCategoryL(const TDesC& aCategoryName);

    /**
     * Removes a existing category from the store. The operation
     * is silently ignored if the specified category does not
     * exist in the landmark database
     *
     * @param aCategoryName The name of the removed category
     */
    void RemoveCategoryL(const TDesC& aCategoryName);

    /**
     * Adds a landmark to the specified category. If the requested
     * category does not exist, this operation leaves with
     * KErrArgument.
     *
     * @param aLandmark The landmark which will be added to the category
     * @param aCategoryName The name of the category to which the landmark
     *        will be added
     */
    void AddLandmarkToCategoryL(
        MLAPILandmark& aLandmark,
        const TDesC& aCategoryName);

    /**
     * Removes a landmark item from a category. The call is silently ignored
     * if the specified category does not exist in the landmark database
     *
     * @param aLandmark The landmark which will be removed from the category
     * @param aCategoryName The name of the category from which the landmark
     *        will be removed
     */
    void RemoveLandmarkFromCategoryL(
        MLAPILandmark& aLandmark,
        const TDesC& aCategoryName);

    void CompactIfNeededL();

public: // From MLAPILmDatabaseObserver

    /**
     * This function is called if a change occurs in the native database
     * Currently, the notified events are only related to categories and
     * landmarks
     *
     * @param aEventType The type of the database event
     * @param aItemId The item to which the event is related to
     */
    void NotifyEvent(
        TPosLmEventType& aEventType,
        TPosLmItemId& aItemId);

protected: // Construtors

    /**
     * C++ constructor.
     * @param aParams Construction parameters
     */
    CLAPICategoryManager(const TCtorParams& aParams);

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // New functions

    /**
     * Refreshes the category list from the database. Note that this operation
     * may be slow if there is a huge amount of categories in the database
     */
    void RefreshCategoryListL();

    /**
     * Reads a category with the specified item identifier. The category
     * is added to the category list
     *
     * @param aItemId The item identifier of the category
     */
    void DoReadCategoryL(TPosLmItemId& aItemId);

    /**
     * Finds the index of the specified category name
     * @param aName The name of the searched category
     * @return The index of the requested category information. KErrNotFound
     *         is returned if there is no such category
     */
    TInt FindCategory(const TDesC& aName) const;

    /**
     * Handles category changes received through database event notifier
     * @param aEventType Type of the event
     * @param aItemId The category id which the event is related to
     */
    void HandleCategoryChangeL(
        TPosLmEventType& aEventType,
        TPosLmItemId& aItemId);

private: // Data

    // Category manager. Owned
    CPosLmCategoryManager* iCategoryManager;

    // Landmark database. Used
    CPosLandmarkDatabase& iLandmarkDatabase;

    // Landmark database.
    CPosLandmarkDatabase* iLandmarkDatabasePtr;

    // Event notifier for database events. Used
    MLAPILmDatabaseEventNotifier* iEventNotifier;

    // Array of category identifiers
    RArray< TLAPIItemId> iCategoryIds;

    // Array of category names. Must be in sync with the ids
    CDesCArray* iCategoryNames;

};

#endif // CLAPICATEGORYMANAGER_H
// End of file
