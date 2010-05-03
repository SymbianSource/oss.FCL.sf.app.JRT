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
* Description:  This class handles presets central repository usage.
*
*/



#ifndef CAMMSTUNERPRESETSHANDLER_H
#define CAMMSTUNERPRESETSHANDLER_H

//  INCLUDES
#include    <e32base.h>
#include    <centralrepository.h>


// FORWARD DECLARATIONS
class TAMMSTunerPreset;

// CLASS DECLARATION
/**
* This class handles presets central repository usage.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CAMMSTunerPresetsHandler) : public CBase
{
public:  // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CAMMSTunerPresetsHandler* NewL();

    /**
    * Destructor.
    */
    virtual ~CAMMSTunerPresetsHandler();

public: // New functions

    /**
    * Gets presets from central repository
    * @param aPreset - presets index
    * @param aTunerPreset
    */
    void GetPresetL(TInt aPreset, TAMMSTunerPreset &aTunerPreset);

    /**
    * Save presets to central repository
    * @param aPreset - presets index
    * @param aTunerPreset
    */
    void SavePresetL(TInt aPreset, TAMMSTunerPreset aTunerPreset);

private:

    /**
    * C++ default constructor.
    */
    CAMMSTunerPresetsHandler();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

    // Prohibit copy constructor if not deriving from CBase.
    // CAMMSTunerPresetsHandler( const CAMMSTunerPresetsHandler& );
    // Prohibit assigment operator if not deriving from CBase.
    // CAMMSTunerPresetsHandler& operator=( const CAMMSTunerPresetsHandler& );


private:    // Data

    CRepository* iCentralRepository;


};

#endif // CAMMSTUNERPRESETSHANDLER_H

// End of File
