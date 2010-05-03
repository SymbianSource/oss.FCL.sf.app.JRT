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


#include <e32math.h>
#include <e32des16.h>
#include <frmtview.h>
#include <txtglobl.h>
#include <txtfmlyr.h>
#include <AknBidiTextUtils.h>
#include "swtgcbase.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/openfontconstants.h>
#endif

#ifdef RD_JAVA_UI_ALFDRAWER_ENABLED
#include <alf/alfdrawer.h>
#endif // RD_JAVA_UI_ALFDRAWER_ENABLED

/*
 * This method is used only for M2G support purposes
 */
void CFbsBitmapESWT::SetHandles(TInt aHandle, TInt aServerHandle)
{
    iHandle = aHandle;
    iServerHandle = aServerHandle;
};


/*
 * Warning: unlike what happens on most platforms, SWT drawing primitives are
 *          inclusive of both their extremities. This means line primitives
 *          must draw the last pixel and fillable shapes must include their
 *          bottom and right edges.
 */

static const TInt KDefaultPenWidth = 1;
static const TInt KThinPenWidth    = 1;
static const TUint16 KCarriageReturn = 0x000d;
static const TUint16 KLineFeed = 0x000a;
static const TUint16 KTabulation = 0x0009;


// ======== MEMBER FUNCTIONS ========


/**
 * Constructor
 *
 * The GC passed in is set as follows:
 *  - The drawing mode is <code>TDrawMode::EDrawModePen</code>
 *  - There is no clipping rectangle
 *  - The pen settings are: solid, single pixel size
 *  - The brush style is null
 *  - No text font is selected
 *  - Pen & brush colours are set to the specified values
 *
 * @param aDrawable     The drawable that created <code>aGc</code>.
 * @param aGc           The native GC to use, ownership is tranferred.
 * @param aDestructor   The function that will take care of destroying <code>iGc</code>.
 * @param aPenCol       The pen's colour.
 * @param aBrushCol     The brush's colour.
 * @param aFont         The font to use
 * @param aBoundaryRect The rectangle drawing is restricted to, in GC coordinates.
 * @param aSize         The GC's size
 */
CSwtGcBase::CSwtGcBase(MSwtDrawable& aDrawable, CBitmapContext* aGc, TDestructor aDestructor,
                       const TRgb& aPenCol, const TRgb& aBrushCol, const MSwtFont& aFont,
                       const TRect& aBoundaryRect, const TSize& aSize)
        : iDrawable(aDrawable)
        , iGc(aGc)
        , iDestructor(aDestructor)
        , iCurrentPenStyle(CGraphicsContext::ESolidPen)
        , iUserPenStyle(CGraphicsContext::ESolidPen)
        , iLineStyle(ESwtLineSolid)
        , iCurrentPenWidth(KDefaultPenWidth)
        , iUserPenWidth(KDefaultPenWidth)
        , iPenColor(aPenCol)
        , iCurrentBrushStyle(CGraphicsContext::ENullBrush)
        , iUserBrushStyle(CGraphicsContext::ESolidBrush)
        , iBrushColor(aBrushCol)
        , iDrawMode(CGraphicsContext::EDrawModePEN)
        , iDefaultFont(aFont)
        , iFont(&aFont)
        , iBoundaryRect(aBoundaryRect)
        , iSize(aSize)
        , iClippingRect(aSize)
        , iAlpha(0xff)
{
    ASSERT(iGc!=NULL);

    iDefaultFont.AddRef();
    iFont->AddRef();

    iGc->SetPenColor(iPenColor);
    iGc->SetBrushColor(iBrushColor);
    iGc->SetClippingRect(iBoundaryRect);
    iGc->UseFont(&iFont->Font());
}


CSwtGcBase::~CSwtGcBase()
{
    iDestructor(iGc);

    iDefaultFont.RemoveRef();
    if (iFont)
    {
        iFont->RemoveRef();
    }

#ifdef _lint
    //lint -esym(1740, CSwtGcBase::iDestructor)
    iGc   = NULL;
    iFont = NULL;
#endif
}

/**
 * Sets the current pen style to null
 */
void CSwtGcBase::UseNullPen()
{
    if (iCurrentPenStyle != CGraphicsContext::ENullPen)
    {
        iCurrentPenStyle = CGraphicsContext::ENullPen;
        iGc->SetPenStyle(CGraphicsContext::ENullPen);
    }
}

/**
 * Sets the current pen style to null
 */
void CSwtGcBase::UseSolidPen()
{
    if (iCurrentPenStyle != CGraphicsContext::ESolidPen)
    {
        iCurrentPenStyle = CGraphicsContext::ESolidPen;
        iGc->SetPenStyle(CGraphicsContext::ESolidPen);
    }
}

/**
 * Restores the pen to its user-asked value
 */
void CSwtGcBase::RestorePen()
{
    if (iCurrentPenStyle != iUserPenStyle)
    {
        iCurrentPenStyle = iUserPenStyle;
        iGc->SetPenStyle(iUserPenStyle);
    }
}

/**
 * Sets the current pen width to 1
 */
void CSwtGcBase::UseThinPen()
{
    if (iCurrentPenWidth != KThinPenWidth)
    {
        iCurrentPenWidth = KThinPenWidth;
        iGc->SetPenSize(TSize(KThinPenWidth,KThinPenWidth));
    }
}

/**
 * Restores the pen width to its user-asked value
 */
void CSwtGcBase::RestorePenWidth()
{
    if (iCurrentPenWidth != iUserPenWidth)
    {
        iCurrentPenWidth = iUserPenWidth;
        TInt width = (iCurrentPenWidth==0) ? 1 : iCurrentPenWidth;
        iGc->SetPenSize(TSize(width,width));
    }
}

/**
 * Sets the current brush style to null
 */
void CSwtGcBase::UseNullBrush()
{
    if (iCurrentBrushStyle != CGraphicsContext::ENullBrush)
    {
        iCurrentBrushStyle = CGraphicsContext::ENullBrush;
        iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    }
}

/**
 * Restores the brush to its user-asked value
 */
void CSwtGcBase::RestoreBrush()
{
    if (iCurrentBrushStyle != iUserBrushStyle)
    {
        iCurrentBrushStyle = iUserBrushStyle;
        iGc->SetBrushStyle(iUserBrushStyle);
    }
}

/**
 * Restores the clipping
 */
void CSwtGcBase::RestoreClipping()
{
    TRect clipRect(iClippingRect);
    clipRect.Intersection(iBoundaryRect); // We can never go outside the boundary rect
    iGc->SetClippingRect(clipRect);
}

/**
 * Converts a degree angle to a (x,y) position.
 */
TPoint CSwtGcBase::AngleToPosition(TInt aAngle, const TSize& aSize)
{
    ASSERT(aSize.iWidth>0 && aSize.iHeight>0);

    TReal radAngle = aAngle * KDegToRad;
    TReal cos;
    TReal sin;
    Math::Cos(cos, radAngle);
    Math::Sin(sin, radAngle);
    return TPoint(static_cast<TInt>(cos*aSize.iWidth),
                  -static_cast<TInt>(sin*aSize.iHeight));
}

/**
 * Computes the parameters of an elliptical arc.
 */
void CSwtGcBase::ComputeArc(const TRect& aRect,
                            TInt aStartAngle, TInt aArcAngle, TPoint& aStart, TPoint& aEnd)
{
    const TPoint center(aRect.Center());

    if (aRect.IsEmpty())
    {
        aStart = aEnd = center;
        return;
    }

    TRect rect(aRect);
    rect.Normalize();

    if (aArcAngle>=360 || aArcAngle<=-360) // Full ellipse
    {
        aStart.iX = aEnd.iX = rect.iBr.iX;
        aStart.iY = aEnd.iY = center.iY;
    }
    else                                   // Arc
    {
        TInt angle1 = aStartAngle;
        TInt angle2 = aStartAngle + aArcAngle;
        if (aArcAngle < 0)
        {
            TInt tmp = angle1;
            angle1 = angle2;
            angle2 = tmp;
        }

        const TSize size(rect.Size());

        aStart  = center;
        aStart += AngleToPosition(angle1, size);

        aEnd    = center;
        aEnd   += AngleToPosition(angle2, size);
    }
}

/**
 * Workaround for issue in the rectangle drawing code.
 * When the line style is other than solid and the pen is wider than 1 pixel,
 * the outline is drawn as solid.
 */
void CSwtGcBase::DrawRectangleWorkaround(const TRect& aRect)
{
    if (iUserPenWidth==1 || iUserPenStyle==CGraphicsContext::ESolidPen)
        iGc->DrawRect(InclusiveRect(aRect));
    else
    {
        TPoint points[4];
        points[0] = aRect.iTl;
        points[1] = TPoint(aRect.iBr.iX, aRect.iTl.iY);
        points[2] = aRect.iBr;
        points[3] = TPoint(aRect.iTl.iX, aRect.iBr.iY);
        iGc->DrawPolygon(points, 4);
    }
}

/**
 * Extracts a portion of a bitmap and scales it to a given size
 * The returned bitmap is a newly created one, it is left on the cleanup stack.
 */
CFbsBitmap* CSwtGcBase::ScaleBitmapPortionLC(
    const CFbsBitmap& aBitmap, const TRect& aRect, const TSize& aNewSize)
{
    // Create the bitmap
    CFbsBitmap* resizedBitmap = new(ELeave) CFbsBitmap;
    CleanupStack::PushL(resizedBitmap);
    User::LeaveIfError(resizedBitmap->Create(aNewSize, aBitmap.DisplayMode()));

    // Create the device
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(resizedBitmap);
    CleanupStack::PushL(bitmapDevice);

    // Create the context
    CBitmapContext* bitmapContext;
    User::LeaveIfError(bitmapDevice->CreateBitmapContext(bitmapContext));

    // Scaled copy
    bitmapContext->DrawBitmap(TRect(aNewSize), &aBitmap, aRect);

    delete bitmapContext;
    CleanupStack::PopAndDestroy(bitmapDevice);
    return resizedBitmap;
}

/**
 * Scales a size by multiplying and dividing it.
 */
TSize CSwtGcBase::ScaleSize(const TSize& aSize,
                            const TSize& aMultiplier, const TSize& aDivider)
{
    ASSERT(aDivider.iWidth!=0 && aDivider.iHeight!=0);

    TInt scaledWidth  = (aSize.iWidth *aMultiplier.iWidth) / aDivider.iWidth;
    TInt scaledHeight = (aSize.iHeight*aMultiplier.iHeight) / aDivider.iHeight;
    return TSize(scaledWidth, scaledHeight);
}

/**
 * Draws a resized image with an alpha mask
 */
void CSwtGcBase::DrawResizedAlphaImageL(const CFbsBitmap& aBitmap,
                                        const CFbsBitmap& aMask, const TRect& aSrceRect, const TRect& aDestRect)
{
    const TSize bmpSize(aBitmap.SizeInPixels());
    const TSize mskSize(aMask.SizeInPixels());
    const TSize srceSize(aSrceRect.Size());
    const TSize destSize(aDestRect.Size());

    CFbsBitmap* resizedBitmap;
    CFbsBitmap* resizedMask;
    TPoint      scaledSrceOrig(TPoint::EUninitialized);

    // The mask is large enough for the bitmap --> resize only portions
    if (mskSize.iWidth>=bmpSize.iWidth && mskSize.iHeight>=bmpSize.iHeight)
    {
        scaledSrceOrig = TPoint(0,0);
        resizedBitmap  = ScaleBitmapPortionLC(aBitmap, aSrceRect, destSize);
        resizedMask    = ScaleBitmapPortionLC(aMask,   aSrceRect, destSize);
    }
    // The mask is too small for the bitmap, it will be tiled --> resize the whole bitmaps
    else
    {
        TSize newBmpSize(ScaleSize(bmpSize, destSize, srceSize));
        TSize newMskSize(ScaleSize(mskSize, destSize, srceSize));

        scaledSrceOrig = ScaleSize(aSrceRect.iTl.AsSize(), destSize, srceSize).AsPoint();
        resizedBitmap  = ScaleBitmapPortionLC(aBitmap, TRect(bmpSize), newBmpSize);
        resizedMask    = ScaleBitmapPortionLC(aMask,   TRect(mskSize), newMskSize);
    }

    iGc->BitBltMasked(aDestRect.iTl, resizedBitmap,
                      TRect(scaledSrceOrig,destSize), resizedMask, EFalse);

    CleanupStack::PopAndDestroy(resizedMask);
    CleanupStack::PopAndDestroy(resizedBitmap);
}

/**
 * Creates a <code>CTextLayout</code> object from a descriptor and its
 *        drawing flags, for display or text extent computaion. The new
 *        <code>CTextLayout</code> object will be pushed on the cleanup stack.
 *
 * The descriptor will be modified to respect the values of the flags.
 * The global text set in parameters will then contain a copy of the updated
 * descriptor, and will be used by the new text layout.
 *
 * @param aText            The descriptor.
 * @param aFlags           Drawing flags of the descriptor.
 * @param aBaseLineSpace   Baseline space.
 *                         new text layout. This global text will contain the
 *                         descriptor with delimiters and tab
 * @param aMinimumViewSize In return, width and height of the bounding box of
 *                          the text.
 *
 * @return Pointer to the bdiitext. Should be released when no more needed.
 */
TBidiText* CSwtGcBase::CreateTextLayoutLC(TDes& aText, TInt aFlags,
        TInt& aSpaceBetweenBaseLines, TSize& aMinimumViewSize) const
{
    ASSERT(iFont!=NULL);

    TBool drawDelimiter(aFlags & KSwtDrawDelimiter);
    TBool drawTab(aFlags & KSwtDrawTab);

    // Delimiters management
    TUint16 delimiter = static_cast<TUint16>(drawDelimiter ?
                        CEditableText::ELineBreak : KReplacementCharacter);
    TUint16 tabchar = static_cast<TUint16>(KSwtDrawTab ?
                                           CEditableText::ETabCharacter : KReplacementCharacter);

    // Scan all text in order to get
    // 1. the number of lines
    // 2. the text length
    // 3. replace delimiter if draw delimiter flag is not set
    TInt descLength(aText.Length());
    TInt numberOfLines(0);
    TInt numberOfTabs(0);
    TInt ind(0);
    for (ind = 0; ind < descLength; ind++)
    {
        switch (aText[ind])
        {
        case KCarriageReturn:
        case KLineFeed:
            aText[ind] = delimiter;
            if (drawDelimiter)
            {
                numberOfLines++;
            }
            break;

        case KTabulation:
            aText[ind] = tabchar;
            if (drawTab)
            {
                numberOfTabs++;
            }
            break;
        default:
            break;
        }
    }
    numberOfLines++;

    // Create text buffer and copy the text to the text buffer
    const TInt KTabLength(4);
    _LIT(KExtraTabSpaces, "   ");   // 3 spaces
    HBufC* textBuffer = HBufC::NewLC(descLength
                                     + numberOfLines * KAknBidiExtraSpacePerLine  + numberOfTabs * (KTabLength -1));
    TPtr ptr = textBuffer->Des();
    ptr = aText.Mid(0);

    // Replace tabchar with two space
    TInt tabPos(KErrNotFound);
    ptr = textBuffer->Des();
    while ((tabPos = ptr.Locate(CEditableText::ETabCharacter)) != KErrNotFound)
    {
        // tab is replaced by 4 spaces
        ptr[ tabPos ] = ' ';
        ptr.Insert(tabPos + 1, KExtraTabSpaces);
    }


    AknBidiTextUtils::PrepareRunInfoArray(textBuffer->Des());

    // Get width of text
    ptr = textBuffer->Des();
    descLength = textBuffer->Length();
    TInt lineWidth(0);
    TInt maxLineWidth(0);
    TInt lineStartPos(0);
    const CFont& font = iFont->Font();
    for (ind = 0; ind < descLength; ind++)
    {
        if (ptr[ ind ] == CEditableText::ELineBreak)
        {
            lineWidth =
                iFont->TextWidthInPixels(ptr.MidTPtr(lineStartPos, ind  - lineStartPos  + 1));
            if (lineWidth > maxLineWidth)
            {
                maxLineWidth = lineWidth;
            }
            lineStartPos = ind + 1;
        }
    }

    lineWidth = font.TextWidthInPixels(ptr.MidTPtr(lineStartPos, descLength - lineStartPos));
    if (lineWidth > maxLineWidth)
    {
        maxLineWidth = lineWidth;
    }

    // Create ad prepare biditext
    TBidiText* bidiText = TBidiText::NewL(textBuffer->Des(), numberOfLines);
    CleanupStack::PushL(bidiText);
    bidiText->WrapText(maxLineWidth, font);

    // Get the minimum size and the baseline spacing
    // WARNING! This must be kept similar to the way Label sets the gap!
    // See CSwtLabel::DoSetFontL
    TInt padding = Max(font.FontLineGap() - font.FontMaxHeight(), KSwtMinLinePadding);
    aMinimumViewSize = bidiText->MinimumSize(maxLineWidth, font, padding, numberOfLines);
    aSpaceBetweenBaseLines = (aMinimumViewSize.iHeight + padding) / numberOfLines;
    if (numberOfLines <= 1)
    {
        aSpaceBetweenBaseLines = 0;
    }

    CleanupStack::Pop(bidiText);
    CleanupStack::PopAndDestroy(textBuffer);
    CleanupStack::PushL(bidiText);

    return bidiText;
}

/**
 * This method removes the '&' characters from a string as mnemonics are not supported in Symbian.
 * @param aText The text from which the mnemonics are to be removed.
 */
void CSwtGcBase::RemoveMnemonics(TDes& aText)
{
    TInt pos = aText.LocateReverse('&');
    while (pos != KErrNotFound)
    {
        aText.Delete(pos, 1);
        // we skip the character before in case there are two '&'.
        if (pos == 0)
        {
            break;
        }
        pos = aText.Left(pos).LocateReverse('&');
    }
}


//
// Virtual methods from MSwtGc
//

void CSwtGcBase::Dispose()
{
    delete this;
}

CBitmapContext& CSwtGcBase::BitmapContext()
{
    return *iGc;
}

void CSwtGcBase::SetAlpha(TInt aAlpha)
{
    iAlpha = aAlpha;

    SetPenRgbValue(iPenColor);
    SetBrushRgbValue(iBrushColor);
}

void CSwtGcBase::SetBackground(const MSwtColor& aColor)
{
    iBrushColor = aColor.RgbValue();
    SetBrushRgbValue(iBrushColor);
}

void CSwtGcBase::SetForeground(const MSwtColor& aColor)
{
    iPenColor = aColor.RgbValue();
    SetPenRgbValue(iPenColor);
}

TSwtLineStyle CSwtGcBase::LineStyle() const
{
    return iLineStyle;
}

void CSwtGcBase::SetLineStyleL(TSwtLineStyle aLineStyle)
{
    // Convert SWT line style to Symbian pen style
    switch (aLineStyle)
    {
    case ESwtLineSolid:
        iUserPenStyle = CGraphicsContext::ESolidPen;
        break;
    case ESwtLineDash:
        iUserPenStyle = CGraphicsContext::EDashedPen;
        break;
    case ESwtLineDot:
        iUserPenStyle = CGraphicsContext::EDottedPen;
        break;
    case ESwtLineDashDot:
        iUserPenStyle = CGraphicsContext::EDotDashPen;
        break;
    case ESwtLineDashDotDot:
        iUserPenStyle = CGraphicsContext::EDotDotDashPen;
        break;
    default:
        User::Leave(KErrArgument);
        break;
    }

    iLineStyle = aLineStyle;
}

TInt CSwtGcBase::LineWidth() const
{
    return iUserPenWidth;
}

void CSwtGcBase::SetLineWidth(TInt aLineWidth)
{
    ASSERT(aLineWidth>=0);
    iUserPenWidth = aLineWidth;
}

const MSwtFont* CSwtGcBase::Font() const
{
    ASSERT(iFont!=NULL);
    return iFont;
}

void CSwtGcBase::SetFont(const MSwtFont* aFont)
{
    ASSERT(iGc!=NULL);

    iFont->RemoveRef();
    iFont = (aFont!=NULL) ? aFont : &iDefaultFont;
    iFont->AddRef();
    iGc->UseFont(&(iFont->Font()));
}

TBool CSwtGcBase::XORMode() const
{
    return (iDrawMode==CGraphicsContext::EDrawModeXOR) ? ETrue : EFalse;
}

void CSwtGcBase::SetXORMode(TBool aXor)
{
    iDrawMode = (aXor) ? CGraphicsContext::EDrawModeXOR : CGraphicsContext::EDrawModePEN;
    iGc->SetDrawMode(iDrawMode);
}

void CSwtGcBase::CopyArea(const TRect& aSource, const TPoint& aDestination)
{
// NGA does not support CopyRect function in 9.2
#ifdef RD_JAVA_NGA_ENABLED
    CCoeEnv* env = CCoeEnv::Static();
    CWsScreenDevice* screenDevice = env->ScreenDevice();

    if (screenDevice)
    {
        TRect sourceRect = InclusiveRect(aSource);
        CFbsBitmap* screenRectImage = new CFbsBitmap();
        screenRectImage->Create(sourceRect.Size(), screenDevice->DisplayMode());
        screenDevice->CopyScreenToBitmap(screenRectImage, sourceRect);

        iGc->BitBlt(aDestination, screenRectImage);
        env->WsSession().Flush();
        env->WsSession().Finish();
        delete screenRectImage;
        screenRectImage = NULL;
    }

#else
    TPoint offset(aDestination - aSource.iTl);
    iGc->CopyRect(offset, InclusiveRect(aSource));
#endif
}

#ifdef RD_JAVA_NGA_ENABLED
TInt CSwtGcBase::FinishOperationL()
{
    TInt err = KErrNone;
#ifdef RD_JAVA_UI_ALFDRAWER_ENABLED
    CAlfDrawer* drawer = CAlfDrawer::NewL();
    CleanupStack::PushL(drawer);
    err = drawer->Finish();
    CleanupStack::PopAndDestroy(drawer);
#else
    err = CCoeEnv::Static()->WsSession().Finish();
#endif // RD_JAVA_UI_ALFDRAWER_ENABLED
    return err;
}
#endif // RD_JAVA_NGA_ENABLED

void CSwtGcBase::DrawM2GImageL(const TInt* aBitmapHandles, const TPoint& aSrcePos,
                               const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize,
                               const TBool aUseNativeClear)
{
    // Reading handles sent from M2G
    TInt bitmapServerHandle = aBitmapHandles[0];
    TInt bitmapM2GHandle = aBitmapHandles[1];
    TInt maskServerHandle = aBitmapHandles[2];
    TInt maskM2GHandle = aBitmapHandles[3];

    // Trying to duplicate image bitmap for our use
    // This does not copy the bitmap !!
    // It only creates another handle on the server
    CFbsBitmapESWT* m2gImageBitmap = new(ELeave) CFbsBitmapESWT;
    CleanupStack::PushL(m2gImageBitmap);
    User::LeaveIfError(m2gImageBitmap->Duplicate(bitmapServerHandle));
    CleanupStack::Pop();

    // Again duplicating mask bitmap, if exists
    if (maskServerHandle)
    {
        CFbsBitmapESWT* m2gMaskBitmap = new(ELeave) CFbsBitmapESWT;
        CleanupStack::PushL(m2gMaskBitmap);
        User::LeaveIfError(m2gMaskBitmap->Duplicate(maskServerHandle));
        CleanupStack::Pop();

        // Drawing image with mask
        DrawImageFromBitmaps(*m2gImageBitmap, m2gMaskBitmap, aSrcePos, aSrceSize,
                             aDestPos, aDestSize, aUseNativeClear);

        // Releasing eSWT mask bitmap handle from server
        m2gMaskBitmap->Reset();
        // Deleting the mask bitmap from server
        m2gMaskBitmap->SetHandles(maskM2GHandle, maskServerHandle);
        delete m2gMaskBitmap;
    }
    else
    {
        // Drawing image without mask
        DrawImageFromBitmaps(*m2gImageBitmap, NULL, aSrcePos, aSrceSize,
                             aDestPos, aDestSize, aUseNativeClear);
    }

    // Releasing eSWT image bitmap handle from server
    m2gImageBitmap->Reset();
    // Deleting the bitmap from server
    m2gImageBitmap->SetHandles(bitmapM2GHandle, bitmapServerHandle);
    delete m2gImageBitmap;
}

void CSwtGcBase::DrawImage(const MSwtImage& aImage, const TPoint& aSrcePos,
                           const TSize& aSrceSize, const TPoint& aDestPos, const TSize& aDestSize)
{
    const CFbsBitmap& bitmap = aImage.Bitmap();
    const CFbsBitmap* mask = aImage.MaskBitmap();
    DrawImageFromBitmaps(bitmap, mask, aSrcePos, aSrceSize, aDestPos, aDestSize, EFalse);
}

void CSwtGcBase::DrawImageFromBitmaps(const CFbsBitmap& aBitmap, const CFbsBitmap* aMask,
                                      const TPoint& aSrcePos, const TSize& aSrceSize, const TPoint& aDestPos,
                                      const TSize& aDestSize, TBool aUseNativeClear)
{
    const TSize srceSize((aSrceSize==TSize(-1,-1)) ? aBitmap.SizeInPixels() : aSrceSize);
    const TSize destSize((aDestSize==TSize(-1,-1)) ? aBitmap.SizeInPixels() : aDestSize);
    const TRect srceRect(aSrcePos, srceSize);

    CFbsBitmap* alphaBitmap(NULL) ;
    if (iAlpha < 0xff)
    {
        alphaBitmap = new CFbsBitmap();
        //Create alpha bitmap
        if (alphaBitmap)
        {
            TSize alphaBitmapSize = aBitmap.SizeInPixels();
            TRAPD(err, alphaBitmap->Create(alphaBitmapSize, EGray256));
            if (err != KErrNone)
            {
                delete alphaBitmap;
                alphaBitmap = NULL;
            }
            else
            {
                TBitmapUtil bitmapUtil = TBitmapUtil(alphaBitmap);
                // Set alpha value
                bitmapUtil.Begin(TPoint(0, 0));
                TRgb maskRgb(0);
                TDisplayMode displayMode(EGray2);
                if (aMask)
                {
                    displayMode =  aMask->DisplayMode();
                }
                for (TInt x = 0; x<aBitmap.SizeInPixels().iWidth; x++)
                {
                    for (TInt y =0; y<aBitmap.SizeInPixels().iHeight; y++)
                    {
                        bitmapUtil.SetPos(TPoint(x, y));
                        if (aMask)
                        {
                            aMask->GetPixel(maskRgb, TPoint(x, y));
                            if (displayMode == EGray2)
                            {
                                bitmapUtil.SetPixel(iAlpha * maskRgb._Gray2());
                            }
                            else if (displayMode == EGray4)
                            {
                                bitmapUtil.SetPixel(iAlpha * maskRgb._Gray4() / 4);
                            }
                            else if (displayMode == EGray16)
                            {
                                bitmapUtil.SetPixel(iAlpha * maskRgb._Gray16() / 16);
                            }
                            else
                            {
                                bitmapUtil.SetPixel(iAlpha * maskRgb._Gray256() / 0xff);
                            }
                        }
                        else
                        {
                            bitmapUtil.SetPixel(iAlpha);
                        }
                    }
                }
                bitmapUtil.End();
            }
        }
    }

    // For M2G purposes. While drawing animation, the background has to be redrawn.
    if (aUseNativeClear)
    {
        iGc->SetBrushColor(KRgbWhite);
        iGc->Clear(destSize);
    }

    if (alphaBitmap)
    {
        aMask = alphaBitmap;
    }

    if (srceSize == destSize)
    {
        if (aMask == NULL)
        {
            iGc->BitBlt(aDestPos, &aBitmap, srceRect);
        }
        else
        {
            UseNullBrush();
            iGc->BitBltMasked(aDestPos, &aBitmap, srceRect, aMask, EFalse);
        }
    }
    else
    {
        const TRect destRect(aDestPos, destSize);
        if (aMask == NULL)
            iGc->DrawBitmap(destRect, &aBitmap, srceRect);
        else
        {
            UseNullBrush();
            TRAP_IGNORE(DrawResizedAlphaImageL(aBitmap, *aMask, srceRect, destRect));
        }
    }
    if (alphaBitmap)
    {
        delete alphaBitmap;
    }

    NotifyUpdate();
}

void CSwtGcBase::DrawPoint(const TPoint& aPoint)
{
    RestorePen();
    UseThinPen();
    iGc->Plot(aPoint);

    NotifyUpdate();
}

void CSwtGcBase::DrawLine(const TPoint& aPoint1, const TPoint& aPoint2)
{
    RestorePen();
    RestorePenWidth();
    if (aPoint1 != aPoint2)
    {
        iGc->DrawLine(aPoint1, aPoint2);
    }
    iGc->Plot(aPoint2);

    NotifyUpdate();
}

void CSwtGcBase::DrawPolyline(const TPoint* aPoints, TInt aCount)
{
    ASSERT(aPoints!=NULL);

    RestorePen();
    RestorePenWidth();
    iGc->DrawPolyLine(aPoints, aCount);
    iGc->Plot(aPoints[aCount-1]);

    NotifyUpdate();
}

void CSwtGcBase::DrawPolygon(const TPoint* aPoints, TInt aCount)
{
    ASSERT(aPoints!=NULL);

    RestorePen();
    RestorePenWidth();
    UseNullBrush();
    iGc->DrawPolygon(aPoints, aCount, CGraphicsContext::EAlternate);

    NotifyUpdate();
}

void CSwtGcBase::FillPolygon(const TPoint* aPoints, TInt aCount)
{
    ASSERT(aPoints!=NULL);

    RestoreBrush();
    UseNullPen();
    iGc->DrawPolygon(aPoints, aCount, CGraphicsContext::EAlternate);

    NotifyUpdate();
}

void CSwtGcBase::DrawRectangle(const TRect& aRect)
{
    RestorePen();
    RestorePenWidth();
    UseNullBrush();

    DrawRectangleWorkaround(aRect);

    NotifyUpdate();
}

void CSwtGcBase::FillRectangle(const TRect& aRect)
{
    RestoreBrush();
    UseNullPen();
    iGc->DrawRect(aRect);

    NotifyUpdate();
}

void CSwtGcBase::FillGradientRectangle(
    const TRect& aRect, TBool aVertical, TBool aInvertGradient)
{
    // NTH: optimise drawing using rectangles ? only rectangles or lines + rectangles ?

    // No gradient --> resort to the faster method
    if (iPenColor == iBrushColor)
    {
        FillRectangle(aRect);
        return;
    }

    UseSolidPen();
    UseThinPen();

    // Decide which colour is which
    TRgb color1;
    TRgb color2;
    if (aInvertGradient)
    {
        color1 = iBrushColor;
        color2 = iPenColor;
    }
    else
    {
        color1 = iPenColor;
        color2 = iBrushColor;
    }

    // Compute the coordinate parameters
    TPoint pt1(aRect.iTl);
    TPoint pt2(TPoint::EUninitialized);
    TInt   count;
    TInt TPoint::*coordinateToIncrement;
    if (aVertical)
    {
        pt2.SetXY(aRect.iBr.iX+1, aRect.iTl.iY);
        count = aRect.Height();
        coordinateToIncrement = &TPoint::iY;
    }
    else
    {
        pt2.SetXY(aRect.iTl.iX, aRect.iBr.iY+1);
        count = aRect.Width();
        coordinateToIncrement = &TPoint::iX;
    }

    // Compute the colour increments using 16.16 fixed point math
    const TInt KOneHalf = 0x8000;
    const TInt incRed   = ((color2.Red()   - color1.Red()) << 16) / count;
    const TInt incGreen = ((color2.Green() - color1.Green()) << 16) / count;
    const TInt incBlue  = ((color2.Blue()  - color1.Blue()) << 16) / count;

    // Draw
    TUint r = color1.Red()   << 16;
    TUint g = color1.Green() << 16;
    TUint b = color1.Blue()  << 16;
    for (TInt i=0; i<=count; ++i)
    {
        TRgb color((r+KOneHalf)>>16, (g+KOneHalf)>>16, (b+KOneHalf)>>16); //lint !e702
        iGc->SetPenColor(color);
        iGc->DrawLine(pt1, pt2);

        ++(pt1.*coordinateToIncrement);
        ++(pt2.*coordinateToIncrement);
        r += incRed;
        g += incGreen;
        b += incBlue;
    }

    // Restore pen colour
    iGc->SetPenColor(iPenColor);

    NotifyUpdate();
}

void CSwtGcBase::DrawRoundRectangle(const TRect& aRect, const TSize& aArcSize)
{
    RestorePen();
    RestorePenWidth();
    UseNullBrush();

    // When the arc's size is 0, the same issue occurs as with a regular rectangle
    if (aArcSize.iWidth==0 || aArcSize.iHeight==0)
        DrawRectangleWorkaround(aRect);
    else
        iGc->DrawRoundRect(InclusiveRect(aRect), aArcSize);

    NotifyUpdate();
}

void CSwtGcBase::FillRoundRectangle(const TRect& aRect, const TSize& aArcSize)
{
    RestoreBrush();
    UseNullPen();
    iGc->DrawRoundRect(InclusiveRect(aRect), aArcSize);

    NotifyUpdate();
}

void CSwtGcBase::DrawOval(const TRect& aRect)
{
    RestorePen();
    RestorePenWidth();
    UseNullBrush();

    iGc->DrawEllipse(InclusiveRect(aRect));

    NotifyUpdate();
}

void CSwtGcBase::FillOval(const TRect& aRect)
{
    RestoreBrush();
    UseNullPen();
    iGc->DrawEllipse(aRect);

    NotifyUpdate();
}

void CSwtGcBase::DrawArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle)
{
    RestorePen();
    RestorePenWidth();
    UseNullBrush();

    TPoint start(TPoint::EUninitialized);
    TPoint end(TPoint::EUninitialized);
    ComputeArc(aRect, aStartAngle, aArcAngle, start, end);
    iGc->DrawArc(InclusiveRect(aRect), start, end);

    NotifyUpdate();
}

void CSwtGcBase::FillArc(const TRect& aRect, TInt aStartAngle, TInt aArcAngle)
{
    RestoreBrush();
    UseNullPen();

    TPoint start(TPoint::EUninitialized);
    TPoint end(TPoint::EUninitialized);
    ComputeArc(aRect, aStartAngle, aArcAngle, start, end);
    iGc->DrawPie(InclusiveRect(aRect), start, end);

    NotifyUpdate();
}

TInt CSwtGcBase::GetCharWidth(const TChar& aChar) const
{
    ASSERT(iFont!=NULL);
    return iFont->Font().CharWidthInPixels(aChar);
}

TInt CSwtGcBase::GetAdvanceWidth(const TChar& aChar) const
{
    ASSERT(iFont!=NULL);
    TOpenFontCharMetrics metrics;
    const TUint8* bitmap;
    TSize bitmapSize(TSize::EUninitialized);
    iFont->Font().GetCharacterData(aChar, metrics, bitmap, bitmapSize);
    return metrics.HorizAdvance();
}

void CSwtGcBase::GetFontMetrics(TSwtFontMetrics& aMetrics) const
{
    ASSERT(iFont!=NULL);

    const CFont& font = iFont->Font();
    TOpenFontMetrics fontMetrics(&font);

    aMetrics.iAscent           = fontMetrics.Ascent();
    aMetrics.iAverageCharWidth = font.WidthZeroInPixels(); // Closest we have to a real average
    aMetrics.iDescent          = fontMetrics.Descent();

    // To get the leading, we retrieve the bitmap height of "Latin Capital Letter E
    // with Circumflex" (code U+00CA). This letter has been chosen because it is an
    // uppercase with an accent and no descender. The leading is therefore the
    // difference between the bitmap's height and the ascent.
    CFont::TCharacterDataAvailability dataAvailable;
    TOpenFontCharMetrics charMetrics;
    const TUint8* bitmap;
    TSize bitmapSize(TSize::EUninitialized);
    dataAvailable = font.GetCharacterData(0x00CA, charMetrics, bitmap, bitmapSize);
    if (dataAvailable == CFont::EAllCharacterData)
        aMetrics.iLeading = Max(0, charMetrics.Height() - aMetrics.iAscent);
    else
        aMetrics.iLeading = 0;

    aMetrics.iHeight = aMetrics.iAscent + aMetrics.iDescent + aMetrics.iLeading;
}

TSize CSwtGcBase::TextExtentL(TDes& aText, TInt aFlags) const
{
    TSize viewSize(TSize::EUninitialized);
    TInt spaceBetweenBaseLines;
    TBidiText* textLayout = CreateTextLayoutLC(aText, aFlags, spaceBetweenBaseLines, viewSize);
    CleanupStack::PopAndDestroy(textLayout);
    return viewSize;
}

void CSwtGcBase::DrawTextL(TDes& aText, const TPoint& aPosition, TInt aFlags)
{
    ASSERT(iFont!=NULL);
    ASSERT(iGc!=NULL);

    // Mnemonics
    if (aFlags & KSwtDrawMnemonic)
    {
        // Mnemonics seem not to exist in Symbian OS
        // By now, we simply remove the '&' characters.
        RemoveMnemonics(aText);
    }

    TSize viewSize(TSize::EUninitialized);
    TInt spaceBetweenBaseLines;
    TBidiText* text = CreateTextLayoutLC(aText, aFlags, spaceBetweenBaseLines, viewSize);

    if (!(aFlags & KSwtDrawTransparent))
    {
        FillRectangle(TRect(aPosition, viewSize));
    }

    // 2nd argument is the point specifying the position of the baseline, therefore the max ascent
    text->DrawText(*iGc, aPosition + TPoint(0, iFont->Font().FontMaxAscent()), spaceBetweenBaseLines);
    CleanupStack::PopAndDestroy(text);

    RestoreClipping();
}

TBool CSwtGcBase::IsClipped() const
{
    return (iClippingRect != TRect(iSize));
}

TRect CSwtGcBase::ClippingRect() const
{
    return iClippingRect;
}

void CSwtGcBase::SetClippingRect(const TRect& aRect)
{
    iClippingRect = aRect;
    RestoreClipping();
}

void CSwtGcBase::CancelClipping()
{
    iClippingRect = TRect(iSize);
    iGc->SetClippingRect(iBoundaryRect);
}

void CSwtGcBase::DrawFocusRect(const TRect& aRect, const MSwtUiUtils& aUtils)
{
    aUtils.DrawFocusRect(*this, aRect);
    // Restore brush because DrawFocusRect may have changed it
    iGc->SetBrushStyle(iCurrentBrushStyle);
}

void CSwtGcBase::SetBrushRgbValue(const TRgb& aRgb)
{
    iBrushColor = aRgb;
    iBrushColor.SetAlpha(iAlpha);
    iGc->SetBrushColor(iBrushColor);
}

void CSwtGcBase::SetPenRgbValue(const TRgb& aRgb)
{
    iPenColor = aRgb;
    iPenColor.SetAlpha(iAlpha);
    iGc->SetPenColor(iPenColor);
}


//
// Virtual methods from MFormCustomDraw
//


/**
 * Implements the background draw method uses by <code>CTextLayout</code>.
 * The CTextLayout class, used in the <code>DrawTextL</code> method always,
 * draws a background color. If we want transparency, we have to specify, to
 * the text layout, a <code>MFormCustomDraw</code> object which implements a
 * specific background drawing method the text layout should use instead. This
 * is why <code>CSwtGcBase</code> derived from <code>MFormCustomDraw</code>.
 * <code>CSwtGcBase::DrawBackground</code> only draws nothing.
 */
void CSwtGcBase::DrawBackground(const TParam& /*aParam*/,
                                const TRgb& /*aBackground*/, TRect& aDrawn) const
{
    aDrawn = ClippingRect();
}
