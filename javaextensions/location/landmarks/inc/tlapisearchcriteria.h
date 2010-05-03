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
* Description:  Defines a landmark search criteria
 *
*/


#ifndef TLAPISEARCHCRITERIA_H
#define TLAPISEARCHCRITERIA_H

// EXTERNAL INCLUDES
#include    <e32std.h>

/**
 * Defines a landmark search criteria
 *
 * The class is only a container class for the data. It does not own
 * any of the search attributes but delegates them to the search
 * provider
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(TLAPISearchCriteria)
{

public: // Constructors

    /**
     * Constructs a default search criteria. The default
     * will return all available items
     */
    inline TLAPISearchCriteria();

public: // New functions

    /**
     * Sets the attributes of a search text. The attributes
     * indicate that which text fields are searched from
     * each landmark to find the matching items.
     *
     * Currently, the only supported fields are name and description
     *
     * @param aTextAttributes The attributes of a search text
     */
    inline void SetTextAttributes(TUint aTextAttributes);

    /**
     * Sets the search string.
     *
     * The text must be a non-empty descriptor. Otherwise the search
     * will fail with KErrArgument. The search is case insensitive.
     *
     * Wild card characters "?" and "*" are supported in the search string.
     * "?" matches a single occurrence of any character and "*" matches
     * zero or more consecutive occurrences of any characters.
     *
     * A landmark matches the criteria if specified text is found anywhere
     * in selected attributes set using SetTextAttributes
     *
     * @param aText The text to search for.
     */
    inline void SetText(const TDesC* aText);

    /**
     * Sets the name of the category which should be used as landmark
     * search criteria. The whole category name must be specified. Category
     * searching is not supported using wild cards and will be considered
     * as ordinary characters
     *
     * The matching is case sensitive. An empty descriptor means uncategorized
     * landmarks search.
     *
     * @param aCategoryName The category name.
     */
    inline void SetCategoryName(const TDesC* aCategoryName);

    /**
     * Define the search area from which the landmarks are searched
     *
     * If the parameters are out of range. The search will faile with KErrArgument
     * The correct ranges are:
     *   -# -90 =< aSouthLatitude =< aNorthLatitude =< 90
     *   -# -180 <= aWestLongitude < 180
     *   -# -180 <= aEastLongitude <= 180
     *
     * @param aSouthLatitude The southern latitude border of the search area.
     * @param aNorthLatitude The northern latitude border of the search area.
     * @param aWestLongitude The western longitude border of the search area.
     * @param aEastLongitude The eastern longitude border of the search area.
     */
    inline void SetArea(const TReal64& aSouthLatitude,
                        const TReal64& aNorthLatitude, const TReal64& aWestLongitude,
                        const TReal64& aEastLongitude);

    /**
     * Returns the search attributes of the search text
     */
    inline TUint TextAttributes() const;

    /**
     * Returns the search text. NULL if not set
     */
    inline const TDesC* Text() const;

    /**
     * Returns the search category name. NULL if not set
     */
    inline const TDesC* CategoryName() const;

    /**
     * Checks if the coordinates are set using SetArea
     */
    inline TBool HasValidCoordinates() const;

    /**
     * Gets the search area. The area may not have been set yet and
     * should be checked with HasValidCoordinates()
     */
    inline void GetArea(TReal64& aSouthLatitude, TReal64& aNorthLatitude,
                        TReal64& aWestLongitude, TReal64& aEastLongitude) const;

private: // Data

    // Search text. Not owned
    const TDesC* iText;

    // Search text attributes
    TUint iTextAttributes;

    // Searched category. Not owned
    const TDesC* iCategoryName;

    // Marks a search area
    TReal64 iSouthLatitude;
    TReal64 iNorthLatitude;
    TReal64 iWestLongitude;
    TReal64 iEastLongitude;

}
;

// Inline functions
#include    "tlapisearchcriteria.inl"

#endif // TLAPISEARCHCRITERIA_H
// End of file
