/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Creates converters.
*
*/


#ifndef CMIDCONVERTFACTORY_H
#define CMIDCONVERTFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <gdi.h>

// Constants
const TInt KFormatNotSupported = -100;

// Pixel formats from DirectGraphics
const TInt KMIDTypeByte1Gray = 1;
const TInt KMIDTypeByte1GrayVertical = -1;
const TInt KMIDTypeByte2Gray = 2;
const TInt KMIDTypeByte4Gray = 4;
const TInt KMIDTypeByte8Gray = 8;
const TInt KMIDTypeByte332RGB = 332;
const TInt KMIDTypeUshort4444ARGB = 4444;
const TInt KMIDTypeUshort444RGB = 444;
const TInt KMIDTypeUshort555RGB = 555;
const TInt KMIDTypeUshort1555ARGB = 1555;
const TInt KMIDTypeUshort565RGB = 565;
const TInt KMIDTypeInt888RGB = 888;
const TInt KMIDTypeInt888ARGB = 32888; // 888 with 8-bit alpha in a 32-bit bitmap
const TInt KMIDTypeInt8888ARGB = 8888;

// FORWARD DECLARATIONS
class TMIDFormatConverter;

//  CLASS DEFINITION
/**
 *
 * Creates converters.
 *
 */
NONSHARABLE_CLASS(CMIDConvertFactory): public CBase
{
public: // constuctor & destructor
    CMIDConvertFactory();
    ~CMIDConvertFactory();

public:
    // returns new or cached converter
    TMIDFormatConverter* ConverterL(TInt aFormat);
    // returns always new converter
    static TMIDFormatConverter* NewConverterL(TInt aFormat);
    // returns pixel format for given display mode
    static TInt GetPixelFormat(const TDisplayMode& aDisplayMode);

private: // data

    // last used converter
    TMIDFormatConverter* iLastUsedConverter;

    // last used format
    TInt iFormat;

};

#endif // CMIDCONVERTFACTORY_H

