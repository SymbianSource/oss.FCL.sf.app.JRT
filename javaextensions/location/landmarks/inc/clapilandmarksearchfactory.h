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
* Description:  Landmark search factory
 *
*/


#ifndef CLAPILANDMARKSEARCHFACTORY_H
#define CLAPILANDMARKSEARCHFACTORY_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32base.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CPosLmCompositeCriteria;
class CPosLmSearchCriteria;
class CPosLandmarkSearch;
class CPosLmItemIterator;
class TLAPISearchCriteria;

/**
 * Landmark search factory
 *
 * Creates a landmark iterator based on the provided search
 * arguments. The arguments can contain multiple combinations
 * of different search criterias.
 *
 * Currently supported search criterias are category, text or
 * area specific searches or combination of these three types
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPILandmarkSearchFactory) : public CBase
{
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     * @return New instance from this class
     */
    static CLAPILandmarkSearchFactory* NewL(
        CPosLandmarkDatabase& aDatabase);

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     * @return New instance from this class
     */
    static CLAPILandmarkSearchFactory* NewLC(
        CPosLandmarkDatabase& aDatabase);

    /**
     * Destructor.
     */
    virtual ~CLAPILandmarkSearchFactory();

public: // New functions

    /**
     * Creates a landmark item iterator based on the passed
     * criteria argument. Currently it is possible to search
     * using a text, category or area criterias or a combination
     * of these criterias
     *
     * If the parameter is null an iterator containing all landmarks
     * will be returned to the caller
     *
     * @param aCriteria A search criteria
     * @return An iterator of landmarks matching the specified criteria
     *         The ownership is transferred to the caller
     */
    CPosLmItemIterator* CreateIteratorL(
        const TLAPISearchCriteria* aCriteria);

private: // New functions

    /**
     * Adds a criteria to the passed composite search criteria
     * The ownership of the added criteria is taken from the caller
     *
     * @param aCompositeCriteria The composite search criteria to which
     *        the criteria argument is added.
     * @param aCriteria The added criteria. The criteria is popped from
     *        the cleanup stack
     */
    void AddCriteriaAndPopL(
        CPosLmCompositeCriteria& aCompositeCriteria,
        CPosLmSearchCriteria* aCriteria) const;

protected: // Constructors

    /**
     * Default C++ constructor
     */
    CLAPILandmarkSearchFactory(CPosLandmarkDatabase& aDatabase);

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // Data

    // Database manager. Used
    CPosLandmarkDatabase& iDatabase;

    // Landmark search manager. Owned
    CPosLandmarkSearch* iLmSearch;

};

#endif // CLAPILANDMARKSEARCHFACTORY_H
// End of file
