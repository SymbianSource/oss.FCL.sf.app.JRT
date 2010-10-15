/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Render context implementation
*
*/

// INCLUDE FILES
#include <eikenv.h> // CCoeEnv
#include <graphics.h>

#include "CM2GRenderContext.h"
#include "MM2GSVGProxy.h"
#include "svgtbitmap.h"
#include <ImageConversion.h>
#include <f32file.h>

M2G_NS_START

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
/* static */ const TReal32       MM2GRenderContext::KFullOpaque             = 1.0;
/* static */
const TReal32       MM2GRenderContext::KFullTransparency       = 0.0;
/* static */
const TUint8        MM2GRenderContext::KMaxAlphaValue          = 255;
/* static */
const TDisplayMode  MM2GRenderContext::KDefaultDisplayMode     = EColor16MA;
/* static */
const TDisplayMode  MM2GRenderContext::KMaskDisplayMode        = EGray256;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// -----------------------------------------------------------------------------
// CM2GRenderContext::CM2GRenderContext
// -----------------------------------------------------------------------------
CM2GRenderContext::CM2GRenderContext()
        : CBase(),
        iProxy(NULL),
        iEngineHandle(M2G_INVALID_HANDLE),
        iAlpha(MM2GRenderContext::KFullOpaque),
        iScaledAlpha(MM2GRenderContext::KMaxAlphaValue),
        iImgBmp(NULL),
        iWindowSurface(NULL)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::CM2GRenderContext");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::NewL
// -----------------------------------------------------------------------------
CM2GRenderContext* CM2GRenderContext::NewL(MM2GSVGProxy* aProxy)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::NewL()");

    CM2GRenderContext* self = new(ELeave) CM2GRenderContext;
    CleanupStack::PushL(self);

    self->ConstructL(aProxy);

    CleanupStack::Pop();

    return self;
}
// -----------------------------------------------------------------------------
// CM2GRenderContext::~CM2GRenderContext
// -----------------------------------------------------------------------------
CM2GRenderContext::~CM2GRenderContext()
{
    M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::~CM2GRenderContext() - proxy=%d, engine=%d", iProxy, iEngineHandle);
    if ((iEngineHandle != M2G_INVALID_HANDLE) && (iProxy != NULL))
    {
        TRAP_IGNORE(iProxy->DeleteSvgEngineL(iEngineHandle));
    }
    if(iWindowSurface)
        {
            delete iWindowSurface;
        }
    if(targetBitmap)
        {
        delete targetBitmap;
        }
    if(iTargetQImage)
        {
        delete iTargetQImage;
        }
    if(tempBitmapForMask)
        {
        delete tempBitmapForMask;
        }
    delete iImgBmp;
    iFbsSession.Disconnect();
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::BindL
// -----------------------------------------------------------------------------
void CM2GRenderContext::BindL(TInt& aTargetHandle)
    {
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::BindL()");
    // get the screen size
    TSize screenSize = CEikonEnv::Static()->ScreenDevice()->SizeInPixels();             
        
    iWindowSurface = (reinterpret_cast<Java::GFX::WindowSurface*>(aTargetHandle));
    //wSurfaceType   = (Java::GFX::WindowSurfaceType)iWindowSurface->getType();
    wSurfaceType = Java::GFX::WsTypeQtImage;
    
    iWindowSurface->bind(wSurfaceType);
        
    switch(wSurfaceType)
        {
        case Java::GFX::WsTypeQtImage:
            User::LeaveIfNull(iOffScreenQImage = iWindowSurface->getQtImage());
            targetBitmap = new CSvgtBitmap ((TInt8*)iOffScreenQImage->bits(),
                    TSize(iOffScreenQImage->size().width(),iOffScreenQImage->size().height()),
                            EColor16MA,iOffScreenQImage->bytesPerLine());
            break;

        /*TODO for time being..case Java::GFX::WsTypeQtImage:
            User::LeaveIfNull(iTargetQImage   = iWindowSurface->getQtImage());
            InitializeQImageOffscreenBufferL(screenSize,iTargetQImage);
            break;*/
            
        case Java::GFX::WsTypeSymbianBitmap:
            CFbsBitmap* tempBitmap;
            User::LeaveIfNull(tempBitmap = (reinterpret_cast<Java::GFX::WindowSurface*>(aTargetHandle)->getSymbianBitmap()));
            InitializeCFbsBitmapOffscreenBufferL(screenSize,tempBitmap);
            //iOffScreenBitmap = new(ELeave) CFbsBitmap();
            break;
            
        default:
            User::Leave(KErrNotSupported);
            break;
        }
    
    }

// -----------------------------------------------------------------------------
// CM2GRenderContext::InitializeQImageOffscreenBufferL
// -----------------------------------------------------------------------------
void CM2GRenderContext::InitializeQImageOffscreenBufferL(TSize aScreenSize,QImage* aQimage)
    {
    QSize aSize;
    aSize.setHeight(aScreenSize.iHeight);
    aSize.setWidth(aScreenSize.iWidth);
    iOffScreenQImage = new QImage(aSize,aQimage->format());
    
    //TODO For time being the format is EColor16MU 
    targetBitmap = new CSvgtBitmap ((TInt8*)iOffScreenQImage->bits(),
    TSize(iOffScreenQImage->size().width(),iOffScreenQImage->size().height()),
            EColor16MU,iOffScreenQImage->bytesPerLine());
    }

// -----------------------------------------------------------------------------
// CM2GRenderContext::InitializeCFbsBitmapOffscreenBufferL
// -----------------------------------------------------------------------------
void CM2GRenderContext::InitializeCFbsBitmapOffscreenBufferL(TSize aScreenSize,CFbsBitmap* aBitmap)
    {
    
    iOffScreenBitmap = new(ELeave) CFbsBitmap();
    User::LeaveIfError(
            iOffScreenBitmap->Create(aScreenSize, aBitmap->DisplayMode()));
    
//    TODO check for stride in case of bitmap.
//    iTargetBitmap = new CSvgtBitmap( (TInt8*)iOffScreenBitmap->DataAddress() ,aScreenSize,iOffScreenBitmap->DisplayMode()/*KDefaultDisplayMode TODO chk this*/,iOffScreenBitmap->ScanLineLength(iOffScreenBitmap->SizeInPixels().iWidth,KDefaultDisplayMode) );
    }

// -----------------------------------------------------------------------------
// CM2GRenderContext::GetImgHandleL
// -----------------------------------------------------------------------------
/*
 * TODO we don't need method anymore as now we need Surface handle
 * TM2GBitmapHandle CM2GRenderContext::GetImgHandleL() const
{
    User::LeaveIfNull(iImgBmp);

    return REINTERPRET_CAST(TM2GBitmapHandle, iImgBmp);
}*/


// -----------------------------------------------------------------------------
// CM2GRenderContext::InitImageBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::InitImageBitmapL()
    {
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::InitImageBitmapL() - begin");
    // get the screen size
    TSize screenSize = CEikonEnv::Static()->ScreenDevice()->SizeInPixels();
    switch (wSurfaceType)
        {
        case Java::GFX::WsTypeQtImage:
            {
            break;
            }
        case Java::GFX::WsTypeSymbianBitmap:
            {
             break;   
            }
        
        default:
            break;
        }
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::InitImageBitmapL() - end");
    }
// -----------------------------------------------------------------------------
// CM2GRenderContext::ReleaseL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ReleaseL()
    {
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ReleaseL() - begin");
    
    iTargetQImage = NULL;
    iOffScreenQImage = NULL;
    iWindowSurface->release();

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ReleaseL() - end");
    }

// -----------------------------------------------------------------------------
// CM2GRenderContext::RenderL
// -----------------------------------------------------------------------------
void CM2GRenderContext::RenderLCDUIL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    const TReal32 aCurrentTime,
    TInt aSvgW, TInt aSvgH,
    TM2GRenderRect& aRect
)
{
    // prepare viewbox
    TRect viewbox;
    TPoint anchor;

    PrepareViewbox(aRect, aSvgW, aSvgH, viewbox, anchor);
    RenderLCDUIL(aSvgDocHandle, aCurrentTime, viewbox, anchor);
}

// -----------------------------------------------------------------------------
void CM2GRenderContext::RenderESWTL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    const TReal32 aCurrentTime,
    TInt aSvgW, TInt aSvgH,
    TM2GRenderRect& aRect,
    TBool aUseNativeClear,
    TInt* aReturnData)
{
    // prepare viewbox
    TRect viewbox;
    TPoint anchor;

    PrepareViewbox(aRect, aSvgW, aSvgH, viewbox, anchor);

    aReturnData[0] = 0;
    aReturnData[1] = 0;
    aReturnData[2] = 0;
    aReturnData[3] = 0;
    aReturnData[4] = anchor.iX;
    aReturnData[5] = anchor.iY;
    aReturnData[6] = viewbox.iTl.iX;
    aReturnData[7] = viewbox.iTl.iY;
    aReturnData[8] = viewbox.Width();
    aReturnData[9] = viewbox.Height();

    RenderESWTL(aSvgDocHandle, aCurrentTime, viewbox, anchor, aUseNativeClear, aReturnData);

    return;
}
// CM2GRenderContext::SetRenderingQualityL
// -----------------------------------------------------------------------------
void CM2GRenderContext::SetRenderingQualityL(TInt aMode)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::SetRenderingQualityL()");
    User::LeaveIfNull(iProxy);
    iProxy->RenderQualityL(iEngineHandle, aMode);
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::SetTransparencyL
// -----------------------------------------------------------------------------
void CM2GRenderContext::SetTransparency(TReal32 aAlpha)
{
    iAlpha = aAlpha;
    iScaledAlpha = STATIC_CAST(TUint8, (aAlpha * MM2GRenderContext::KMaxAlphaValue));
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::ConstructL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ConstructL(MM2GSVGProxy* aProxy)
    {
    // Init member variables
    SetTransparency(MM2GRenderContext::KFullOpaque);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - begin");

    if (aProxy)
    {
        iProxy = aProxy;
        iProxy->CreateSvgEngineL(iEngineHandle);
        M2G_DEBUG_2("M2G_DEBUG: CM2GRenderContext::ConstructL() - proxy: %d, new engine: %d", iProxy, iEngineHandle);
    }
    else
    {
        M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - proxy is invalid");
        M2G_THROW(KM2GArgNotOk);
    }

    User::LeaveIfError(iFbsSession.Connect());
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ConstructL() - end");
    }
// -----------------------------------------------------------------------------
// CM2GRenderContext::PrepareViewbox
// -----------------------------------------------------------------------------
void CM2GRenderContext::PrepareViewbox(
    TM2GRenderRect& aRr,
    TInt aSvgW, TInt aSvgH,
    TRect& aViewbox, TPoint& aAnchor)
{
    M2G_DEBUG_6("M2G_DEBUG: CM2GRenderContext::PrepareViewbox() cX=%d, cY=%d, cW=%d, cH=%d, anchorX=%d, anchorY=%d - begin", aRr.GetClipX(), aRr.GetClipY(), aRr.GetClipW(), aRr.GetClipH(), aRr.GetAnchorX(), aRr.GetAnchorY());

    // Create an anchor point and an svg render area rect
    aAnchor.SetXY(aRr.GetAnchorX(), aRr.GetAnchorY());
    aViewbox.SetRect(aAnchor, TSize(aSvgW, aSvgH));

    // NOTE: It's already verified in Java side that the SVG render area and
    // the clip area intersects each other
    aViewbox.Intersection(aRr);

    // Check if the clip rect has changes the svg rect
    if (aViewbox.iTl != aAnchor)
    {
        // Update anchor position
        TPoint oldAnchor(aAnchor);
        aAnchor = aViewbox.iTl;

        // Update svg rect
        aViewbox.Move((-oldAnchor.iX), (-oldAnchor.iY));
    }
    else
    {
        // The clip rect has not changed the svg rect. Only the
        // anchor position need to be updated
        aViewbox.Move(-aAnchor.iX, -aAnchor.iY);
    }
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::PrepareViewbox() - end");
}

void CM2GRenderContext::RenderLCDUIL(
                TM2GSvgDocumentHandle& aSvgDocHandle,
                TReal32 aCurrentTime,
                const TRect& aViewbox,
                const TPoint& aAnchor)
    {
    M2G_DEBUG_4("M2G_DEBUG: CM2GRenderContext::RenderL() viewbox: x=%d, y=%d, w=%d, h=%d begin", aViewbox.iTl.iX, aViewbox.iTl.iY, aViewbox.Size().iWidth, aViewbox.Size().iHeight);

    // No need to render if content is fully transparency (i.e. alpha=0)
    if (iScaledAlpha == 0)
    {
        return;
    }

    QStringList list;
    list <<"QImage-Format_RGB32";
    TBuf8<32> fname;
    
    // 1: render the svg document on the iImgBmp
    iProxy->RenderDocumentL(
        iEngineHandle,
        aSvgDocHandle,
        (TM2GSvgBitmapHandle)targetBitmap 
        , (TUint)NULL, aCurrentTime);
      
      fname.Zero();
      fname.Format(_L8("c:\\%s.bmp"), list.at(0).toLocal8Bit().constData());
      TBool ret = iOffScreenQImage->save((const char*)fname.PtrZ());
      
      QPixmap pixmap = QPixmap::fromImage(*iOffScreenQImage);
      tempBitmapForMask  = pixmap.toSymbianCFbsBitmap();
      
      TFileName       bitmapFilename;
      bitmapFilename.Copy(_L("c:\\bugbitmap"));
      bitmapFilename.AppendNum(tempBitmapForMask->Handle());
      bitmapFilename.Append(_L(".bmp"));
      SaveBitmapL(*tempBitmapForMask, bitmapFilename);
      //TODO Release function should be called from FrameWork
      //iWindowSurface->release();
      M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::RenderL() end");
    }

// -----------------------------------------------------------------------------
/*TODO
 * Write the separate RenderDocumentL method for QImage and CFbsBitmap 
 * also handle subsequent BitBlt and
 * CreateAlphaBlendMaskL
 * */
void CM2GRenderContext::RenderESWTL(
    TM2GSvgDocumentHandle& aSvgDocHandle,
    TReal32 aCurrentTime,
    const TRect& aViewbox,
    const TPoint& aAnchor,
    TBool /*aUseNativeClear*/,
    TInt* aReturnData)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::RenderESWTL() Start");
    // No need to render if content is fully transparency (i.e. alpha=0)
    if (iScaledAlpha == 0)
    {
        return;
    }
    
    // 1: render the svg document on the iImgBmp
   iProxy->RenderDocumentL(iEngineHandle,
       aSvgDocHandle,
       (TM2GSvgBitmapHandle)targetBitmap, (TUint)NULL, aCurrentTime);
   M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::RenderESWTL() end");
    
}
// CM2GRenderContext::CreateAlphaBlendMask
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CM2GRenderContext::ClearBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::ClearBitmapL(CFbsBitmap* aBmp)
{
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap - begin");

    User::LeaveIfNull(aBmp);

    TM2GBitmapLock lock(aBmp);

    TSize   size            = aBmp->SizeInPixels();
    TInt    scanlineLength  = aBmp->ScanLineLength(size.iWidth, aBmp->DisplayMode());

    TUint32* buf = aBmp->DataAddress();
    char* bufBytes = REINTERPRET_CAST(char*, buf);

    Mem::FillZ(bufBytes,  size.iHeight * scanlineLength);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap - end");
}


void CM2GRenderContext::ClearSurfaceL(TM2GSvgBitmapHandle aSvgtBmpHandle )
    {
    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap - begin");
//TODO As Currently we are not using this functionality.     
/*     switch(wSurfaceType)
         {
         case Java::GFX::WsTypeQtImage:
             QImage* tempQImage;
             User::LeaveIfNull(tempQImage = (reinterpret_cast<Java::GFX::WindowSurface*>(aSvgtBmpHandle)->getQtImage()));
             //TODO Do we need to lock the qimage as implemented below        TM2GBitmapLock lock(tempBitmap);?
             QSize   sizeQimage            = tempQImage->size();//TODO Check for SizeInPixels
             TInt    scanlineLengthQimage  = tempQImage->bytesPerLine();
             //uchar* bufBytesQimage = REINTERPRET_CAST(uchar*, tempQImage->bits());
             
             Mem::FillZ(tempQImage->bits(),  sizeQimage.height() * scanlineLengthQimage);
             
             M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearSurface Qimage Clear function- end");
             break;
             
         case Java::GFX::WsTypeSymbianBitmap:
             CFbsBitmap* tempBitmap;
             User::LeaveIfNull(tempBitmap = (reinterpret_cast<Java::GFX::WindowSurface*>(aSvgtBmpHandle)->getSymbianBitmap()));
             TM2GBitmapLock lock(tempBitmap);
             TSize   sizeBmp            = tempBitmap->SizeInPixels();
             TInt    scanlineLengthBmp  = tempBitmap->ScanLineLength(sizeBmp.iWidth, tempBitmap->DisplayMode());
             TUint32* bufBmp = tempBitmap->DataAddress();
             char* bufBytesBmp = REINTERPRET_CAST(char*, bufBmp);

             Mem::FillZ(bufBytesBmp,  sizeBmp.iHeight * scanlineLengthBmp);

             M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearSurface Bitmap Clear function- end");
             break;
             
         default:
             M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::ClearBitmap Type Not Supported.- end");
             User::Leave(KErrNotSupported);
             break;
         }
 */
    }

// -----------------------------------------------------------------------------
// CM2GRenderContext::FillBitmapL
// -----------------------------------------------------------------------------
void CM2GRenderContext::FillBitmapL(CFbsBitmap* aBmp, const TUint8& aChar)
{
    M2G_DEBUG_1("M2G_DEBUG: CM2GRenderContext::FillBitmap() filled with=%d - begin", aChar);
    User::LeaveIfNull(aBmp);
    TM2GBitmapLock lock(aBmp);

    TSize   size            = aBmp->SizeInPixels();
    TInt    scanlineLength  = aBmp->ScanLineLength(size.iWidth, aBmp->DisplayMode());

    TUint32* buf = aBmp->DataAddress();
    char* bufBytes = REINTERPRET_CAST(char*, buf);

    Mem::Fill(bufBytes, size.iHeight * scanlineLength, aChar);

    M2G_DEBUG_0("M2G_DEBUG: CM2GRenderContext::FillBitmap - end");
}

// -----------------------------------------------------------------------------
// CM2GRenderContext::GetBufferHandleL
// -----------------------------------------------------------------------------
TM2GSvgBitmapHandle CM2GRenderContext::GetBufferHandleL() const
    {
    switch(wSurfaceType)
           {
           case Java::GFX::WsTypeQtImage:
               User::LeaveIfNull(iOffScreenQImage);
               return REINTERPRET_CAST(TM2GSvgBitmapHandle , iOffScreenQImage);
	           break;

           case Java::GFX::WsTypeSymbianBitmap:
               User::LeaveIfNull(iOffScreenBitmap);
               return REINTERPRET_CAST(TM2GSvgBitmapHandle , iOffScreenBitmap);
               break;
               
           default:
               User::Leave(KErrNotSupported);
               break;
           }
    
    }

//CODE to check the Bitmap Contain.
TInt CM2GRenderContext::SaveBitmapL(const CFbsBitmap& aNVGBitmap, const TFileName& aFileName)
    {
        TFileName       bitmapFilename;
        bitmapFilename.Copy(aFileName);
        RFs aFs;
        User::LeaveIfError(aFs.Connect());
        CImageEncoder * imgEncoder = 0;
        TRequestStatus  requesStatus = 0;
        imgEncoder = CImageEncoder::FileNewL(aFs, bitmapFilename, _L8("image/bmp"), CImageEncoder::EOptionAlwaysThread);
        imgEncoder->Convert(&requesStatus, aNVGBitmap);
        User::WaitForRequest(requesStatus);
        delete imgEncoder;       
        aFs.Close();
        return 0;
    }

M2G_NS_END
