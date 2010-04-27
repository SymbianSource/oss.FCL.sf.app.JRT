/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used to create tuner player and control
*
*/



#ifndef AMMSTUNERFACTORY_H
#define AMMSTUNERFACTORY_H

//  INCLUDES
#include <e32base.h>

#include "CAMMSTunerControl.h"

// FORWARD DECLARATIONS
class CAMMSTunerPlayer;
class CAMMSTunerControl;

// CLASS DECLARATION
/**
*  This class is used to create tuner player and control.
*  It parses locator parameters and set them to control.
*/

NONSHARABLE_CLASS(AMMSTunerFactory)
{
public:  // Constructors and destructor

    /**
    * Destructor.
    */
    virtual ~AMMSTunerFactory();

public: // New functions

    /**
    * ?member_description.
    * @param aTunerPlayer
    * @param aLocatorParams
    */
    static void CreatePlayerL(CAMMSTunerPlayer** aTunerPlayer,
                              const TDesC* aLocatorParams);

protected:  // New functions

    /**
    * Parse all locator parameters.
    * @param aLocatorParams
    * @param aFrequency
    * @param aStereoMode
    * @param aPreset
    */
    static void ParseParamsL(const TDesC* aLocatorParams,
                             TInt& aFrequency,
                             TInt& aStereoMode,
                             TInt& aPreset);

    /**
    * Parses frequency parameter
    * @param aFrequency - locator's frequency parameter
    * @return frequency in TInt
    */
    static TInt ParseFreqL(const TPtrC aFrequency);

    /**
    * Converts TPtrC to TReal using TLex.
    * @param aHertz - hertzs in TPtrC
    * @return hertzs in TReal
    */
    static TReal TDesCToTRealL(const TPtrC aHertz);

private:

    /**
    * C++ default constructor.
    */
    AMMSTunerFactory();
};

#endif // AMMSTUNERFACTORY_H

// End of File
