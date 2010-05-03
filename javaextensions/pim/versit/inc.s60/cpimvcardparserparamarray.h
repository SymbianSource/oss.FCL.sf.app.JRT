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
* Description:  Helper class for CParserParam array
 *
*/


#ifndef CPIMVCARDPARSERPARAMARRAY_H
#define CPIMVCARDPARSERPARAMARRAY_H

// EXTERNAL INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CParserParam;

// CLASS DECLARATION
/**
 *  This class is a wrapper for the CparserParam pointer array
 *  and helps to handle CParserParam array object in low
 *  memory situations
 *
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS(CPIMVCardParserParamArray) : public CArrayPtrFlat< CParserParam>
{
public:

    /**
     * C++ Constructor.
     * Just generates an array object of this class
     *
     * @param aGranurality array granularity
     */
    CPIMVCardParserParamArray(TInt aGranularity);

    /**
     * Destructor.
     */
    ~CPIMVCardParserParamArray();
};

#endif // CPIMVCARDPARSERPARAMARRAY_H
// End of File
