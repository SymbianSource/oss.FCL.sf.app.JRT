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
* Description:  Helper class for preset handling, reads and writes preset
*                values in/out stream.
*
*/



#ifndef TAMMSTUNERPRESET_H
#define TAMMSTUNERPRESET_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

// CONSTANTS
const TInt KPresetNameLength = 100;

// CLASS DECLARATION
/**
*  Helper class for preset handling, reads and writes preset
*  values in/out stream.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(TAMMSTunerPreset)
{
public:

    /**
    * C++ default constructor.
    */
    TAMMSTunerPreset();

    /**
    * Setter for preset name.
    */
    void SetPresetName(const TDesC* aPresetName);

    /**
    * Setter for preset frequency.
    */
    void SetPresetFrequency(const TInt aPresetFrequency);

    /**
    * Setter for preset stereo mode.
    */
    void SetPresetStereoMode(const TInt aPresetStereoMode);

    /**
    * Getter for preset name.
    */
    TPtrC PresetName();

    /**
    * Getter for preset frequency.
    */
    TInt PresetFrequency();

    /**
    * Getter for preset stereo mode.
    */
    TInt PresetStereoMode();

    /**
    * Serialize the data to stream.
    * @param aStream - used for write stream
    */
    void ExternalizeL(RWriteStream &aStream) const;

    /**
    * deserialize the data out of stream.
    * @param aStream  used for read stream
    */
    void InternalizeL(RReadStream &aStream);

private:

    TBuf<KPresetNameLength> iPresetName;
    TInt iPresetFrequency;
    TInt iPresetStereoMode;

};

#endif // TAMMSTUNERPRESET_H

// End of File
