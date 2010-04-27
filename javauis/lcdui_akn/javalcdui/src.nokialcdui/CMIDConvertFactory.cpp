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



//  INCLUDE FILES
#include <e32std.h>

#include "CMIDConvertFactory.h"

#include "TMID4444Format.h"
#include "TMID444Format.h"
#include "TMID565Format.h"
#include "TMID8888Format.h"
#include "TMID888Format.h"
#include "TMID888AFormat.h"
#include "TMIDGray1.h"
#include "TMIDGray1Vertical.h"


CMIDConvertFactory::CMIDConvertFactory()
{
}

CMIDConvertFactory::~CMIDConvertFactory()
{
    delete iLastUsedConverter;
}

TMIDFormatConverter* CMIDConvertFactory::ConverterL(TInt aFormat)
{
    if (aFormat != iFormat)
    {
        TMIDFormatConverter* converter = NewConverterL(aFormat);
        delete iLastUsedConverter;
        iLastUsedConverter = converter;
        iFormat = aFormat;
    }
    return iLastUsedConverter;
}

TMIDFormatConverter* CMIDConvertFactory::NewConverterL(TInt aFormat)
{
    switch (aFormat)
    {
    case KMIDTypeUshort4444ARGB:
    {
        return new(ELeave)TMID4444Format();
    }
    case KMIDTypeUshort444RGB:
    {
        return new(ELeave)TMID444Format();
    }
    case KMIDTypeUshort565RGB:
    {
        return new(ELeave)TMID565Format();
    }
    case KMIDTypeInt8888ARGB:
    {
        return new(ELeave)TMID8888Format();
    }
    case KMIDTypeInt888RGB:
    {
        return new(ELeave)TMID888Format();
    }
    case KMIDTypeInt888ARGB:
    {
        return new(ELeave)TMID888AFormat();
    }
    case KMIDTypeByte1Gray:
    {
        return new(ELeave)TMIDGray1();
    }
    case KMIDTypeByte1GrayVertical:
    {
        return new(ELeave)TMIDGray1Vertical();
    }
    default:
    {
        User::Leave(KFormatNotSupported);
    }
    }
    // not possible to come here.
    ASSERT(false);
    return NULL;
}

TInt CMIDConvertFactory::GetPixelFormat(const TDisplayMode& aDisplayMode)
{
    TInt nativePixelFormat(KFormatNotSupported);
    switch (aDisplayMode)
    {
    case EGray2:
    {
        nativePixelFormat = KMIDTypeByte1Gray;
        break;
    }
    case EColor64K:
    {
        nativePixelFormat = KMIDTypeUshort565RGB;
        break;
    }
    case EColor16MA:
    case EColor16M:
    {
        nativePixelFormat = KMIDTypeInt8888ARGB;
        break;
    }
    case ERgb:
    {
        nativePixelFormat = KMIDTypeInt888RGB;
        break;
    }
    case EColor4K:
    {
        nativePixelFormat = KMIDTypeUshort4444ARGB;
        break;
    }
    case EColor16MU:
    {
        nativePixelFormat = KMIDTypeInt888RGB;
        break;
    }
    default:
    {
        nativePixelFormat = KFormatNotSupported;
        break;
    }
    }
    return nativePixelFormat;
}
