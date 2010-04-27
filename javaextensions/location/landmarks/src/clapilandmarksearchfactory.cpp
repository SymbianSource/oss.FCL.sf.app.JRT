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


// INTERNAL INCLUDES
#include    "clapilandmarksearchfactory.h"
#include    "tlapisearchcriteria.h"
#include    "logger.h"
// EXTERNAL INCLUDES
#include    <EPos_CPosLandmarkDatabase.h>
#include    <EPos_CPosLandmarkSearch.h>
#include    <EPos_CPosLmItemIterator.h>
#include    <EPos_CPosLmOperation.h>
#include    <EPos_CPosLmCompositeCriteria.h>
#include    <EPos_CPosLmCategoryCriteria.h>
#include    <EPos_CPosLmTextCriteria.h>
#include    <EPos_CPosLmAreaCriteria.h>

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::NewL
// ---------------------------------------------------------------------------
//
CLAPILandmarkSearchFactory* CLAPILandmarkSearchFactory::NewL(
    CPosLandmarkDatabase& aDatabase)
{
    JELOG2(EJavaLocation);
    CLAPILandmarkSearchFactory* self = CLAPILandmarkSearchFactory::NewLC(
                                           aDatabase);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::NewLC
// ---------------------------------------------------------------------------
//
CLAPILandmarkSearchFactory* CLAPILandmarkSearchFactory::NewLC(
    CPosLandmarkDatabase& aDatabase)
{
    JELOG2(EJavaLocation);
    CLAPILandmarkSearchFactory* self =
        new(ELeave) CLAPILandmarkSearchFactory(aDatabase);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::~CLAPILandmarkSearchFactory
// ---------------------------------------------------------------------------
//
CLAPILandmarkSearchFactory::~CLAPILandmarkSearchFactory()
{
    JELOG2(EJavaLocation);
    delete iLmSearch;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::CreateIteratorL
// ---------------------------------------------------------------------------
//
CPosLmItemIterator* CLAPILandmarkSearchFactory::CreateIteratorL(
    const TLAPISearchCriteria* aCriteria)
{
    JELOG2(EJavaLocation);
    CPosLmItemIterator* iterator(NULL);
    // Sorting is left for the MIDlet. There is no requirement for this
    // as it slows the performance of listing landmarks. Usually there
    // is no need to sort the landmarks because those are plotted to a map

    // Match all landmark without using the Landmark Search API if the passed
    // search criteria is null or does not have any parameters set
    if (!aCriteria || !aCriteria->Text() && !aCriteria->CategoryName()
            && !aCriteria->HasValidCoordinates())
    {
        iterator = iDatabase.LandmarkIteratorL();
    }
    else
    {
        // Create a composite criteria and add other criterias to it
        CPosLmCompositeCriteria* criterias = CPosLmCompositeCriteria::NewLC(
                                                 CPosLmCompositeCriteria::ECompositionAND);

        // Currently, category criteria is the first which will be matched
        // if the argument has been specified by the client
        const TDesC* categoryName = aCriteria->CategoryName();
        if (categoryName)
        {
            CPosLmCategoryCriteria* criteria = CPosLmCategoryCriteria::NewLC();
            // Empty string must find uncategorized landmarks
            if (categoryName->Length() > 0)
            {
                criteria->SetCategoryNameL(*categoryName);
            }
            // The ownership of the criteria is transferred to criterias
            // and the value is popped from the cleanup stack
            AddCriteriaAndPopL(*criterias, criteria);
        }

        const TDesC* text = aCriteria->Text();
        // Next search argument will be the text criteria. The search
        // criteria must specify at least one attribute which is searched
        if (text)
        {
            CPosLmTextCriteria* criteria = CPosLmTextCriteria::NewLC();
            criteria->SetTextL(*text);
            // Currently only name attribute is supported for searching
            criteria->SetAttributesToSearch(aCriteria->TextAttributes());
            // The ownership of the criteria is transferred to criterias
            // and the value is popped from the cleanup stack
            AddCriteriaAndPopL(*criterias, criteria);
        }

        // Finally use the match area if specified
        if (aCriteria->HasValidCoordinates())
        {
            TReal64 southLat;
            TReal64 northLat;
            TReal64 westLon;
            TReal64 eastLon;

            aCriteria->GetArea(southLat, northLat, westLon, eastLon);
            // Match all landmarks within the specified area
            CPosLmAreaCriteria* criteria = CPosLmAreaCriteria::NewLC(southLat,
                                           northLat, westLon, eastLon);
            // The ownership of the criteria is transferred to criterias
            // and the value is popped from the cleanup stack
            AddCriteriaAndPopL(*criterias, criteria);
        }
        // Start the search and do not use previous results
        CPosLmOperation* op = iLmSearch->StartLandmarkSearchL(*criterias);
        // The ownership is transferred to the following function call which
        // puts the operation to the clean-up stack so no need to push here
        ExecuteAndDeleteLD(op);
        CleanupStack::PopAndDestroy(criterias);
        // Get the match iterator from the landmark search manager
        iterator = iLmSearch->MatchIteratorL();
    }

    // The ownership is transferred to the caller
    return iterator;
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::AddCriteriaAndPopL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkSearchFactory::AddCriteriaAndPopL(
    CPosLmCompositeCriteria& aCompositeCriteria,
    CPosLmSearchCriteria* aCriteria) const
{
    JELOG2(EJavaLocation);
    User::LeaveIfError(aCompositeCriteria.AddArgument(aCriteria));
    CleanupStack::Pop(aCriteria);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::CLAPILandmarkSearchFactory
// ---------------------------------------------------------------------------
//
CLAPILandmarkSearchFactory::CLAPILandmarkSearchFactory(
    CPosLandmarkDatabase& aDatabase) :
        iDatabase(aDatabase)
{
    JELOG2(EJavaLocation);
}

// ---------------------------------------------------------------------------
// CLAPILandmarkSearchFactory::ConstructL
// ---------------------------------------------------------------------------
//
void CLAPILandmarkSearchFactory::ConstructL()
{
    JELOG2(EJavaLocation);
    iLmSearch = CPosLandmarkSearch::NewL(iDatabase);
}

// End of file
