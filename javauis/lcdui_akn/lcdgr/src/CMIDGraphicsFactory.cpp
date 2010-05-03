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
* Description:
*
*/

#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <barsread.h>

#include <lcdgr.h>
#include <lcdgdrv.h>
#include <lcdgr.rsg>


//
// Proxies
//
#include "CMIDImage.h"
#include "CMIDGraphics.h"
#include "CMIDImageDecoder.h"
#include "MMIDCanvasGraphicsItemPainter.h"

//
// Implementation
//
#include "MidProxyMap.h"
#include "LcdImage.h"
#include "LcdGraphics.h"
#include "LcdFbsImage.h"
#include "LcdFbsImageCache.h"

#include "s60commonutils.h"

struct TGraphicsMode
{
    TDisplayMode iScreenMode;
    TDisplayMode iPrimaryMode;
    TDisplayMode iTransparencyMode;
    TDisplayMode iUiColorMode;
    TDisplayMode iUiAlphaMode;
    TBool        iUiInvertMask;
};

struct TGraphicsConfig
{
    TGraphicsMode   iGraphicsMode;
    TBool           iUpdateRequired;
    TBool           iDoubleBuffer;
};

TBool ValidGraphicsMode(const TGraphicsMode& aConfig);

TBool IsAlpha(CFbsBitmap* aBitmap);

TInt CompareScreenMode(const TGraphicsMode& aLhs, const TGraphicsMode& aRhs)
{
    return aLhs.iScreenMode - aRhs.iScreenMode;
}

NONSHARABLE_CLASS(CMIDGraphicsFactory) : public CBase
        , public MMIDGraphicsFactory
        , private MImageTypeMap
{
public:
    CMIDGraphicsFactory(RFs&);
    void ConstructL(TDisplayMode);
    ~CMIDGraphicsFactory();

    virtual void            Dispose();
    virtual TDisplayMode    DisplayMode() const;
    virtual TBool           DoubleBuffer() const;
    virtual MMIDImage*      NewMutableImageL(const TSize& aSize);
    virtual MMIDImage*      NewImageL(const TSize& aSize, TInt aTransparency);
    virtual MMIDImage*      NewImageL(MMIDImageDecoder* aDecoder);
    virtual MMIDImage*      NewImageL(MMIDCanvas* aCanvas);
    virtual MMIDGraphics*   NewGraphicsL(MMIDCanvas* aCanvas);
    virtual MMIDGraphics*   NewGraphicsL(MMIDCustomItem* aCustomItem);
    virtual MMIDGraphics*   NewGraphicsL(MMIDImage* aImage);
    virtual MMIDGraphics*   NewGraphicsL(MMIDCanvasGraphicsItemPainter* aCanvasGraphicsPainter);
    virtual MMIDImageDecoder* NewImageDecoderL();

private:
    virtual TImageType GetImageType(MMIDImage::TTransparencyType aType);

private:
    void LoadConfigL(RArray<TGraphicsMode>& aModeArray, const TLinearOrder<TGraphicsMode>&);
    void ConfigureL(TGraphicsMode& aMode);
    TImageType MutableImageType() const;

    virtual MMIDGraphics*   NewGraphicsL(
        const CFbsBitmap* aBitmap, TBool aIsImageTarget, TBool aIsCanvasGraphicsItem);

private:
    RFs&                iFsSession;
    CLcdGraphicsDriver* iDriver;
    CLcdFbsImageCache*  iBitmapCache;
    CMIDProxyMap*       iProxyMap;
    TGraphicsConfig     iConfig;
    TInt                iConfigError;   // indicates an error when loading config from resource file
    TImageType          iImageTypeArray[3];
};

//
// Configuration resource file and constants.
//
_LIT(KLcdgrResourceFileName, "lcdgr.rsc");
const TInt KDefaultDisplayMode = MMIDBitmapImage::EDefaultDisplayMode;

//
// The one and only export.
//
EXPORT_C MMIDGraphicsFactory*   NewFactoryL(RFs& aFsSession, TDisplayMode aScreenMode)
{
    CMIDGraphicsFactory* factory = new(ELeave) CMIDGraphicsFactory(aFsSession);
    CleanupStack::PushL(factory);
    factory->ConstructL(aScreenMode);
    CleanupStack::Pop(factory);
    return factory;
}

CMIDGraphicsFactory::CMIDGraphicsFactory(RFs& aFsSession)
        : iFsSession(aFsSession)
{
    //
    // initialize default configuration.
    //
    iConfig.iGraphicsMode.iPrimaryMode          = (TDisplayMode)KDefaultDisplayMode;
    iConfig.iGraphicsMode.iTransparencyMode = (TDisplayMode)KDefaultDisplayMode;
    iConfig.iGraphicsMode.iUiColorMode  = (TDisplayMode)KDefaultDisplayMode;
    iConfig.iGraphicsMode.iUiAlphaMode  = (TDisplayMode)KDefaultDisplayMode;
    iConfig.iGraphicsMode.iUiInvertMask = EFalse;
    iConfig.iUpdateRequired = EFalse;
    iConfig.iDoubleBuffer   = EFalse;
}

void CMIDGraphicsFactory::ConstructL(TDisplayMode aScreenMode)
{
    //
    // Array of graphics modes sorted on screen mode.
    //
    RArray<TGraphicsMode> modeArray(4, _FOFF(TGraphicsMode, iScreenMode));
    TLinearOrder<TGraphicsMode> screenModeOrder(CompareScreenMode);
    CleanupClosePushL(modeArray);
    TGraphicsMode mode;

    //
    // Load config from resource file.
    //
    TRAP(iConfigError, LoadConfigL(modeArray, screenModeOrder)); // ignore error - we'll just use defaults
    if (iConfigError)
    {
        modeArray.Reset();
        mode.iScreenMode       = aScreenMode;
        mode.iPrimaryMode      = aScreenMode;
        mode.iTransparencyMode = EGray2;
        mode.iUiColorMode      = aScreenMode;
        mode.iUiAlphaMode      = EGray2;
        mode.iUiInvertMask     = EFalse;
        modeArray.AppendL(mode);
    }
    else
    {
        mode.iScreenMode = aScreenMode;
        TInt index = modeArray.FindInOrder(mode, screenModeOrder);
        if (index < 0)
        {
            //
            // Configuration file does not list screen mode.
            //
            User::Leave(KErrNotSupported);
        }
        mode = modeArray[index];
    }
    ASSERT(mode.iScreenMode == aScreenMode);

    //
    // Evaluate configuration, validating any loaded values and filling
    // in defaults. This also populates the image type array.
    //
    ConfigureL(mode);
    CleanupStack::PopAndDestroy();      // modes

    iDriver = CLcdGraphicsDriver::NewL(iConfig.iGraphicsMode.iPrimaryMode);

    //
    // Construct bitmap cache for UI controls.
    //
    iBitmapCache = CLcdFbsImageCache::NewL(*iDriver,
                                           iConfig.iGraphicsMode.iUiColorMode,
                                           iConfig.iGraphicsMode.iUiAlphaMode,
                                           iConfig.iGraphicsMode.iUiInvertMask);

    iProxyMap = new(ELeave) CMIDProxyMap;
}

CMIDGraphicsFactory::~CMIDGraphicsFactory()
{
    if (iBitmapCache)
    {
        delete iBitmapCache;
        iBitmapCache = NULL;
    }
    if (iProxyMap)
    {
        delete iProxyMap;
        iProxyMap = NULL;
    }
    if (iDriver)
    {
        delete iDriver;
        iDriver = NULL;
    }
}

void CMIDGraphicsFactory::Dispose()
{
    delete this;
}

TDisplayMode CMIDGraphicsFactory::DisplayMode() const
{
    return iConfig.iGraphicsMode.iPrimaryMode;
}

TBool CMIDGraphicsFactory::DoubleBuffer() const
{
    return iConfig.iDoubleBuffer;
}

MMIDImage* CMIDGraphicsFactory::NewMutableImageL(const TSize& aSize)
{
    CLcdImage* image = new(ELeave) CLcdImage(*iDriver, ETrue);
    CleanupStack::PushL(image);
    image->ConstructL(aSize, MutableImageType());
    CMIDImage* proxy = new(ELeave) CMIDImage(*iBitmapCache, *this, image);
    CleanupStack::Pop(image);
    CleanupStack::PushL(proxy);
    proxy->ConstructL();
    CleanupStack::Pop(proxy);
    return proxy;
}

MMIDImage* CMIDGraphicsFactory::NewImageL(const TSize& aSize, TInt aTransparency)
{
    CLcdImage* image = new(ELeave) CLcdImage(*iDriver, EFalse);
    CleanupStack::PushL(image);
    image->ConstructL(aSize, iImageTypeArray[aTransparency]);
    CMIDImage* proxy = new(ELeave) CMIDImage(*iBitmapCache, *this, image);
    CleanupStack::Pop(image);
    CleanupStack::PushL(proxy);
    proxy->ConstructL();
    CleanupStack::Pop(proxy);
    return proxy;
}

MMIDImage* CMIDGraphicsFactory::NewImageL(MMIDImageDecoder* aDecoder)
{
    MMIDBitmapImage* bitmapImage = aDecoder->BitmapImage();
    if (!bitmapImage)
    {
        User::Leave(KErrArgument);
    }

    CFbsBitmap* colorBitmap = bitmapImage->ColorBitmap();
    CFbsBitmap* alphaBitmap = bitmapImage->AlphaBitmap();

    TInt index = MMIDImage::ENone;
    if (alphaBitmap)
    {
        if (IsAlpha(alphaBitmap))
        {
            index = MMIDImage::EAlpha;
        }
        else
        {
            index = MMIDImage::EMask;
        }
    }

    CLcdImage* image = new(ELeave) CLcdImage(*iDriver, EFalse);
    CleanupStack::PushL(image);
    image->ConstructL(colorBitmap, alphaBitmap, iImageTypeArray[index]);
    CMIDImage* proxy = new(ELeave) CMIDImage(*iBitmapCache, *this, image);
    CleanupStack::Pop(image);
    CleanupStack::PushL(proxy);
    proxy->ConstructL();
    CleanupStack::Pop(proxy);
    return proxy;
}

/**
 * Create a framebuffer image
 */
MMIDImage* CMIDGraphicsFactory::NewImageL(MMIDCanvas* aCanvas)
{
    CFbsBitmap* colorBitmap = aCanvas->FrameBuffer();
    CFbsBitmap* alphaBitmap = NULL;

    if (NULL == colorBitmap)
    {
        User::Leave(KErrNotSupported);
    }

    CLcdImage* image = new(ELeave) CLcdImage(*iDriver, ETrue);
    CleanupStack::PushL(image);
    image->ConstructL(colorBitmap, alphaBitmap, MutableImageType());
    CMIDImage* proxy = new(ELeave) CMIDImage(*iBitmapCache, *this, image);
    CleanupStack::Pop(image);
    CleanupStack::PushL(proxy);
    proxy->ConstructL();
    CleanupStack::Pop(proxy);
    return proxy;
}


#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// NGA extension
// ---------------------------------------------------------------------------
//
MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(MMIDCanvas* aCanvas)
{
    CLcdGraphics* graphics = CLcdGraphics::NewL(*iDriver, aCanvas->FrameBuffer());
    CleanupStack::PushL(graphics);
    CMIDGraphics* proxy = new(ELeave) CMIDGraphics(*iProxyMap, graphics, EFalse, aCanvas);
    CleanupStack::Pop(graphics);
    return proxy;
}

#else // !RD_JAVA_NGA_ENABLED

MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(MMIDCanvas* aCanvas)
{
    CCoeControl&      window    = aCanvas->Control();
    MDirectContainer& container = aCanvas->DirectContainer();

    CLcdGraphics* graphics = CLcdGraphics::NewL(*iDriver, window, container, iConfig.iUpdateRequired);
    CleanupStack::PushL(graphics);
    CMIDGraphics* proxy = new(ELeave) CMIDGraphics(*iProxyMap, graphics, EFalse);
    CleanupStack::Pop(graphics);
    return proxy;
}
#endif // RD_JAVA_NGA_ENABLED

MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(MMIDCustomItem* aCustomItem)
{
    return NewGraphicsL(aCustomItem->FrameBuffer(), EFalse, EFalse);
}

MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(MMIDImage* aImage)
{
    //
    // All mutable images must be registered bitmap images as we currently
    // rely on BITGDI to render many primitives.
    //
    // We could have a separate map for mutable images if necessary.
    //
    MMIDBitmapImage* image = iBitmapCache->GetBitmapImage(aImage);
    ASSERT(image);
    MMIDGraphics* graphics = NewGraphicsL(image->ColorBitmap(), ETrue, EFalse);

    //Graphics duplicates the fbs handle.
    image->RemoveRef();
    return graphics;
}

MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(MMIDCanvasGraphicsItemPainter* aCanvasGraphicsItemPainter)
{
    return NewGraphicsL(aCanvasGraphicsItemPainter->FrameBuffer(), EFalse, ETrue);
}

MMIDGraphics* CMIDGraphicsFactory::NewGraphicsL(
    const CFbsBitmap* aBitmap, TBool aIsImageTarget, TBool aIsCanvasGraphicsItem)

{
    if (NULL == aBitmap)
    {
        User::Leave(KErrArgument);
    }

    if (aBitmap->DisplayMode() != iConfig.iGraphicsMode.iPrimaryMode)
    {
        User::Leave(KErrArgument);
    }

    CLcdGraphics* graphics = CLcdGraphics::NewL(*iDriver, aBitmap);
    CleanupStack::PushL(graphics);
    CMIDGraphics* proxy = new(ELeave) CMIDGraphics(*iProxyMap, graphics, aIsImageTarget);
    CleanupStack::Pop(graphics);

    // CLcdGraphics instance has to know that rendering target bitmap is framebuffer
    // of CanavsGraphicsItem. Its because drawing images its not allowed to
    // transparent target and CanvasGraphicsItem is transparent by default.
    graphics->SetCanvasGraphicsItemtarget(aIsCanvasGraphicsItem);

    return proxy;
}

MMIDImageDecoder* CMIDGraphicsFactory::NewImageDecoderL()
{
    CMIDImageDecoder* decoder = new(ELeave) CMIDImageDecoder(iFsSession, DisplayMode());
    CleanupStack::PushL(decoder);
    decoder->ConstructL();
    CleanupStack::Pop(decoder);
    return decoder;
}

void CMIDGraphicsFactory::LoadConfigL(RArray<TGraphicsMode>& aModeArray, const TLinearOrder<TGraphicsMode>& aOrder)
{
    TFileName fileName;

    fileName.Append(KLcdgrResourceFileName);
    fileName = java::util::S60CommonUtils::ResourceLanguageFileNameL(fileName);

    RResourceFile configFile;
    configFile.OpenL(iFsSession, fileName);
    CleanupClosePushL(configFile);
    configFile.ConfirmSignatureL();

    //
    // read R_GRAPHICS_CONFIG
    //

    //
    // WARNING! TResourceReader will panic or endless loop on error
    // (e.g. if the config file is corrupt).
    //

    HBufC8* resourceData = configFile.AllocReadLC(R_GRAPHICS_CONFIG);
    TResourceReader reader;
    reader.SetBuffer(resourceData);

    int count = reader.ReadInt16();
    for (int i=0; i<count; i++)
    {
        TGraphicsMode mode;
        mode.iScreenMode  = (TDisplayMode)reader.ReadInt16();
        mode.iPrimaryMode = (TDisplayMode)reader.ReadInt16();
        mode.iTransparencyMode = (TDisplayMode)reader.ReadInt16();
        mode.iUiColorMode  = (TDisplayMode)reader.ReadInt16();
        mode.iUiAlphaMode  = (TDisplayMode)reader.ReadInt16();
        mode.iUiInvertMask = (TBool)reader.ReadInt16();
        aModeArray.InsertInOrderL(mode, aOrder);
    }

    iConfig.iUpdateRequired = (TBool)reader.ReadInt16();
    iConfig.iDoubleBuffer   = (TBool)reader.ReadInt16();

    CleanupStack::PopAndDestroy(resourceData);

    CleanupStack::PopAndDestroy(); //configFile
}


void CMIDGraphicsFactory::ConfigureL(TGraphicsMode& aMode)
{
    //
    // Fill in defaulted values.
    //
    if (aMode.iPrimaryMode == KDefaultDisplayMode)
    {
        aMode.iPrimaryMode = aMode.iScreenMode;
    }

    if (aMode.iTransparencyMode == KDefaultDisplayMode)
    {
        if (aMode.iPrimaryMode == EColor16MA)
        {
            aMode.iTransparencyMode = ENone;
        }
        else
        {
            aMode.iTransparencyMode = EGray2;
        }
    }

    //
    // Validate the configuration.
    //

    if (aMode.iUiColorMode == KDefaultDisplayMode)
    {
        aMode.iUiColorMode = aMode.iPrimaryMode;
    }

    if (aMode.iUiAlphaMode == KDefaultDisplayMode)
    {
        aMode.iUiAlphaMode = EGray2;
    }

    if (!ValidGraphicsMode(aMode))
    {
        User::Leave(KErrNotSupported);
    }

    iConfig.iGraphicsMode = aMode;

    //
    // configure opaque image type
    //
    iImageTypeArray[MMIDImage::ENone].iColorMode    = aMode.iPrimaryMode;
    iImageTypeArray[MMIDImage::ENone].iAlphaMode    = ENone;
    iImageTypeArray[MMIDImage::ENone].iTransparency = ETransparencyNone;

    //
    // Configure transparent image type
    //
    iImageTypeArray[MMIDImage::EMask].iColorMode = aMode.iPrimaryMode;
    if (aMode.iPrimaryMode == EColor16MA)
    {
        iImageTypeArray[MMIDImage::EMask].iAlphaMode    = ENone;
        iImageTypeArray[MMIDImage::EMask].iTransparency = ETransparencyMaskChannel;
    }
    else
    {
        iImageTypeArray[MMIDImage::EMask].iAlphaMode    = aMode.iTransparencyMode;
        iImageTypeArray[MMIDImage::EMask].iTransparency = ETransparencyMaskBitmap;
    }

    //
    // Configure alpha image type
    //
    iImageTypeArray[MMIDImage::EAlpha].iColorMode = aMode.iPrimaryMode;
    if (aMode.iPrimaryMode == EColor16MA)
    {
        iImageTypeArray[MMIDImage::EAlpha].iAlphaMode    = ENone;
        iImageTypeArray[MMIDImage::EAlpha].iTransparency = ETransparencyAlphaChannel;
    }
    else
    {
        iImageTypeArray[MMIDImage::EAlpha].iAlphaMode    = EGray256;
        iImageTypeArray[MMIDImage::EAlpha].iTransparency = ETransparencyAlphaBitmap;
    }

    iConfig.iGraphicsMode   = aMode;

#ifdef __WINS__
    iConfig.iUpdateRequired = ETrue;
#endif
}

TImageType CMIDGraphicsFactory::MutableImageType() const
{
    return iImageTypeArray[MMIDImage::ENone];
}

TBool ValidGraphicsMode(const TGraphicsMode& aMode)
{
    if (aMode.iPrimaryMode == EColor16MA)
    {
        if (aMode.iTransparencyMode != ENone)
        {
            return EFalse;
        }
    }
    else
    {
        // transparency alpha mode must be EGray2 or matched mode.
        if ((aMode.iTransparencyMode != EGray2) && (aMode.iTransparencyMode != aMode.iPrimaryMode))
        {
            return EFalse;
        }
    }

    return ETrue;
}

TBool IsAlpha(CFbsBitmap* aBitmap)
{
    TBool alpha = EFalse;

    if (aBitmap->DisplayMode() == EGray256)
    {
        TSize size = aBitmap->SizeInPixels();
        TInt  scan = CFbsBitmap::ScanLineLength(size.iWidth, EGray256);

        aBitmap->LockHeap();

        TUint8* address = (TUint8*)aBitmap->DataAddress();
        for (TInt h = size.iHeight; --h>=0;)
        {
            TUint8* pix = address;
            TUint8* end = address + size.iWidth;
            while (pix < end)
            {
                TUint8 value = *pix++;
                if ((value > 0) && (value < 255))
                {
                    alpha = ETrue;
                    goto endLoop;
                }
            }
            address += scan;
        }

endLoop:

        aBitmap->UnlockHeap();
    }

    return alpha;
}

#define TYPE_ARRAY_SIZE (sizeof(iImageTypeArray)/sizeof(iImageTypeArray[0]))

TImageType CMIDGraphicsFactory::GetImageType(MMIDImage::TTransparencyType aType)
{
    ASSERT((0 <= aType) && (aType < TYPE_ARRAY_SIZE));
    return iImageTypeArray[ aType ];
}
