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

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::TLAPISearchCriteria
// ---------------------------------------------------------------------------
//
inline TLAPISearchCriteria::TLAPISearchCriteria() :
        iText(NULL),
        iCategoryName(NULL),
        iSouthLatitude(KErrNotFound),
        iNorthLatitude(KErrNotFound),
        iWestLongitude(KErrNotFound),
        iEastLongitude(KErrNotFound)
{
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::SetTextAttributes
// ---------------------------------------------------------------------------
//
inline void TLAPISearchCriteria::SetTextAttributes(TUint aTextAttributes)
{
    iTextAttributes = aTextAttributes;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::SetText
// ---------------------------------------------------------------------------
//
inline void TLAPISearchCriteria::SetText(const TDesC* aText)
{
    iText = aText;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::SetCategoryName
// ---------------------------------------------------------------------------
//
inline void TLAPISearchCriteria::SetCategoryName(const TDesC* aCategoryName)
{
    iCategoryName = aCategoryName;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::SetArea
// ---------------------------------------------------------------------------
//
inline void TLAPISearchCriteria::SetArea(
    const TReal64& aSouthLatitude,
    const TReal64& aNorthLatitude,
    const TReal64& aWestLongitude,
    const TReal64& aEastLongitude)
{
    iSouthLatitude = aSouthLatitude;
    iNorthLatitude = aNorthLatitude;
    iWestLongitude = aWestLongitude;
    iEastLongitude = aEastLongitude;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::TextAttributes
// ---------------------------------------------------------------------------
//
inline TUint TLAPISearchCriteria::TextAttributes() const
{
    return iTextAttributes;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::Text
// ---------------------------------------------------------------------------
//
inline const TDesC* TLAPISearchCriteria::Text() const
{
    return iText;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::CategoryName
// ---------------------------------------------------------------------------
//
inline const TDesC* TLAPISearchCriteria::CategoryName() const
{
    return iCategoryName;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::HasValidCoordinates
// ---------------------------------------------------------------------------
//
inline TBool TLAPISearchCriteria::HasValidCoordinates() const
{
    if (iSouthLatitude != KErrNotFound &&
            iNorthLatitude != KErrNotFound &&
            iWestLongitude != KErrNotFound &&
            iEastLongitude != KErrNotFound)
    {
        return ETrue;
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// TLAPISearchCriteria::GetArea
// ---------------------------------------------------------------------------
//
inline void TLAPISearchCriteria::GetArea(
    TReal64& aSouthLatitude,
    TReal64& aNorthLatitude,
    TReal64& aWestLongitude,
    TReal64& aEastLongitude) const
{
    aSouthLatitude = iSouthLatitude;
    aNorthLatitude = iNorthLatitude;
    aWestLongitude = iWestLongitude;
    aEastLongitude = iEastLongitude;
}

// End of file
