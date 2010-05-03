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
* Description:  Provides access to CParserProperty::iArrayOfParams
 *
*/


#ifndef CPIMPARSERPROPERTY_H
#define CPIMPARSERPROPERTY_H

// INTERNAL INCLUDES
#include "pimtypes.h"

// EXTERNAL INCLUDES
#include <vprop.h>

// CLASS DECLARATION

/**
 * Class deriving from CParserProperty to access its protected iArrayOfParams
 * member. This class also provides some additional features to match attributes
 */
NONSHARABLE_CLASS(CPIMParserProperty) : public CParserProperty
{
public: // New functions

    /**
     * Returns this property's array of parameters.
     */
    const CArrayPtr<CParserParam>* ArrayOfParams() const;

    /**
     * Attributes
     * Returns PIM API sttributes which are included to this property
     * Matches all attributes from the array
     *
     * @return Matched attributes
     */
    TPIMAttribute MatchAllAttributes() const;

    /**
     * Attributes
     * Returns PIM API sttributes which are included to this property
     * Matches home and work attributes from the array
     *
     * @return Matched attributes
     */
    TPIMAttribute MatchHomeAndWorkAttributes() const;

    /**
     * ContainsParameter
     * Check if this property contains requested parameters
     *
     * @param aParam Parameter to find
     * @return ETrue if parameter is found, EFalse if not
     */
    TBool ContainsParameter(const TDesC8& aParam) const;

    /**
     * MatchContactField
     * Matches PIM contact field to the property name
     *
     * @return PIM field matching the name of this property
     */
    TPIMField MatchContactField() const;

    /**
     * MatchEventField
     * Matches PIM event field to the property name
     *
     * @return PIM field matching the name of this property
     */
    TPIMField MatchEventField() const;

    /**
     * MatchToDoField
     * Matches PIM todo field to the property name
     *
     * @return PIM field matching the name of this property
     */
    TPIMField MatchToDoField() const;

private: // New functions

    /**
     * DoCompareNameAndValue
     * Compares the name and value fields of the parser parameter
     *
     * @param aParam Parser param from which the name and value are compared
     * @param aValue Value which will be compared
     *
     * @return ETrue if either the name or the value match with requested
     *         value
     */
    TBool DoCompareNameAndValue(
        const CParserParam& aParam,
        const TDesC8& aValue) const;

private:

    /**
     * Constructor
     */
    CPIMParserProperty(); // Not implemented (just suppress warnings)

};

#endif // CPIMPARSERPROPERTY_H
// End of File
