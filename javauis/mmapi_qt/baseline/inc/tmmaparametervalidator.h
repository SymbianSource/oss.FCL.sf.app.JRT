/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for parsing and validating properties
*
*/


#ifndef TMMAPARAMETERVALIDATOR_H
#define TMMAPARAMETERVALIDATOR_H

//  INCLUDES
#include <e32base.h>
#include <mmf/common/mmfutilities.h>
#include <imageconversion.h>
// STRUCTS

/**
 * These structs are used for storing settings
 */

// Default values for recorder
const TInt KDefaultBits =     -1;
const TInt KDefaultRate =     -1;
const TInt KDefaultChannels = -1;


NONSHARABLE_STRUCT(CMMAAudioSettings): public CBase
{

    ~CMMAAudioSettings();

    TUid iController;
    TUid iEncoding;
    TFourCC iDataType;
    TInt iRate;
    TInt iChannels;
    TBool iStreamableFormat;
    HBufC *iContentType;
};

NONSHARABLE_STRUCT(TMMAVideoSettings)
{
    TPtrC iEncoding;
    TReal32 iFps;
    TInt iWidth;
    TInt iHeight;
    inline void operator=(const TMMAVideoSettings& aSettings)
    {
        iEncoding.Set(aSettings.iEncoding);
        iFps = aSettings.iFps;
        iHeight = aSettings.iHeight;
        iWidth = aSettings.iWidth;
    }
};
/**
 * Container for image settings
 */

NONSHARABLE_CLASS(CMMAImageSettings): public CBase
{
public: // destructor
    ~CMMAImageSettings();
    // this is basicly struct owning some data
    // so there is no point to make getters/setters
public:
    HBufC8* iMimeType; //owned
    HBufC8* iType; // owned
    TInt  iWidth;
    TInt  iHeight;
    CFrameImageData* iImageData; //owned
};

//  CLASS DECLARATION
/**
*   This class is used for parsing and validating properties
*   The property string is in format "name=value&name=value&..."
*
*/

NONSHARABLE_CLASS(TMMAParameterValidator)
{
public:// New methods
    /**
     * Validates audio properties. Accepted keys are:
     * "encoding", "rate", "bits", "channels", "endian", "signed"
     * Properties string must start with "encoding" parameter
     * Leaves with KErrArgument if parameters are not valid.
     */
    static CMMAAudioSettings *ValidateAudioPropertiesL(const TDesC& aProperties);

    /**
     * Validates video properties. Accepted keys are:
     * "encoding", "fps", "width", "height"
     * Properties string must start with "encoding" parameter
     * Leaves with KErrArgument if parameters are not valid.
     */
    static TMMAVideoSettings ValidateVideoPropertiesL(const TDesC& aProperties);

    /**
     * Validates image properties. Accepted keys are:
     * "encoding", "width", "height", "quality", "progressive", "interlaced", "type", "colors"
     * Properties string must start with "encoding" parameter
     * Leaves with KErrArgument if parameters are not valid.
     */
    static CMMAImageSettings* ValidateImagePropertiesL(const TDesC& aProperties);

private: // New methods
    /**
     * Every properties must start with "encoding="
     */
    static void CheckEncodingL(const TDesC& aProperties);

    /**
     * Creates new HBufC8 and copies data to it from aDes.
     */
    static HBufC8* CreateHBufC8FromUnicodeL(const TDesC& aDes);
};

#endif // TMMAPARAMETERVALIDATOR_H
