/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtimagedata.h"
#include "javalocalref.h"
#include "swtpalettedata.h"
#include "utils.h"


// ======== MEMBER FUNCTIONS ========


/**
 * Default C++ constructor
 */
inline CSwtImageData::CSwtImageData()
{
}

/**
 * Destructor
 */
CSwtImageData::~CSwtImageData()
{
    delete iPalette;
    delete iPixelBuffer;
    delete iMaskBuffer;
    delete iAlphaBuffer;
}

/**
 * 2nd phase constructor
 * Initialises this object from a Java <code>ImageData</code> object.
 */
void CSwtImageData::ConstructL(JNIEnv* aJniEnv, jobject aImageData, TBool& aFailed)
{
    // Get the class object
    RJavaLocalRef<jclass> clazz(aJniEnv, aJniEnv->GetObjectClass(aImageData));
    if (!clazz)
        User::Leave(KErrNotFound);

    CleanupClosePushL(clazz);

    iInfo.iSize.iWidth      = GetIntField(aJniEnv, aImageData, clazz, "width",            aFailed);
    iInfo.iSize.iHeight     = GetIntField(aJniEnv, aImageData, clazz, "height",           aFailed);
    iInfo.iDepth            = GetIntField(aJniEnv, aImageData, clazz, "depth",            aFailed);
    iInfo.iScanlinePad      = GetIntField(aJniEnv, aImageData, clazz, "scanlinePad",      aFailed);
    iInfo.iBytesPerLine     = GetIntField(aJniEnv, aImageData, clazz, "bytesPerLine",     aFailed);
    iInfo.iTransparentPixel = GetIntField(aJniEnv, aImageData, clazz, "transparentPixel", aFailed);
    iInfo.iMaskPad          = GetIntField(aJniEnv, aImageData, clazz, "maskPad",          aFailed);
    iInfo.iAlpha            = GetIntField(aJniEnv, aImageData, clazz, "alpha",            aFailed);
    iInfo.iType             = static_cast<TSwtImageType>(GetIntField(aJniEnv, aImageData, clazz, "type", aFailed));
    iInfo.iTopLeft.iX       = GetIntField(aJniEnv, aImageData, clazz, "x",                aFailed);
    iInfo.iTopLeft.iY       = GetIntField(aJniEnv, aImageData, clazz, "y",                aFailed);
    iInfo.iDisposalMethod   = static_cast<TSwtGifDisposal>(GetIntField(aJniEnv, aImageData, clazz, "disposalMethod", aFailed));
    iInfo.iDelayTime        = GetIntField(aJniEnv, aImageData, clazz, "delayTime",        aFailed);

    // Get data fields
    iPixelBuffer = GetByteArrayFieldL(aJniEnv, aImageData, clazz, "data",      aFailed);
    iMaskBuffer  = GetByteArrayFieldL(aJniEnv, aImageData, clazz, "maskData",  aFailed);
    iAlphaBuffer = GetByteArrayFieldL(aJniEnv, aImageData, clazz, "alphaData", aFailed);

    // Get the palette
    if (!aFailed)
    {
        jobject objField = GetObjectField(aJniEnv, aImageData, clazz, "palette", "Lorg/eclipse/swt/graphics/PaletteData;", aFailed);
        RJavaLocalRef<jobject> palette(aJniEnv, objField);
        if (palette)
        {
            CleanupClosePushL(palette);
            iPalette = CSwtPaletteData::NewL(aJniEnv, palette);
            CleanupStack::PopAndDestroy(); // palette
        }

        if (iPalette == NULL)
            aFailed = ETrue;
    }

    // Cleanup
    CleanupStack::PopAndDestroy(); // clazz
}

/**
 * Constructs a native copy of the provided Java image data object.
 * Additionally to leaving when it encounters a native error, this method will
 * also return <code>NULL</code> if a Java error occurs.
 */
CSwtImageData* CSwtImageData::NewL(JNIEnv* aJniEnv, jobject aImageData)
{
    ASSERT(aJniEnv!=NULL);
    ASSERT(aImageData!=NULL);

    TBool javaFailure = EFalse;

    CSwtImageData* self = new(ELeave) CSwtImageData;
    CleanupStack::PushL(self);
    self->ConstructL(aJniEnv, aImageData, javaFailure);
    CleanupStack::Pop(self);

    if (javaFailure)
    {
        delete self;
        self = NULL;
    }

    return self;
}


//
// Virtual methods from MSwtImageData
//

const MSwtImageData::TInfo& CSwtImageData::Info() const
{
    return iInfo;
}

const MSwtPaletteData& CSwtImageData::Palette() const
{
    return *iPalette; //lint !e613
}

const HBufC8& CSwtImageData::PixelBuffer() const
{
    return *iPixelBuffer;
}

const HBufC8* CSwtImageData::MaskBuffer() const
{
    return iMaskBuffer;
}

const HBufC8* CSwtImageData::AlphaBuffer() const
{
    return iAlphaBuffer;
}
