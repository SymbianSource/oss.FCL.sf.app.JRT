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


#include <gdi.h>
#include <gulutil.h>
#include <AknFontAccess.h>
#include <AknUtils.h>
#include <AknLayoutFont.h>
#include "swtfont.h"
#include "swtdisplay.h" // needed for macro definitions


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtFont::NewL
// ---------------------------------------------------------------------------
//
CSwtFont* CSwtFont::NewL(const MSwtDevice& aDevice,
                         const TDesC& aTypefaceName, const TInt aHeight, const TInt aStyle)
{
    CSwtFont* self = new(ELeave) CSwtFont(aDevice, NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aTypefaceName, aHeight, aStyle);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtFont::NewL
// ---------------------------------------------------------------------------
//
CSwtFont* CSwtFont::NewL(const MSwtDevice& aDevice, const CFont& aFont)
{
    CSwtFont* self = new(ELeave) CSwtFont(aDevice, NULL);
    CleanupStack::PushL(self);
    self->ConstructL(aFont);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtFont::CSwtFont
// ---------------------------------------------------------------------------
//
CSwtFont::CSwtFont(const MSwtDevice& aDevice, CFont* aFont)
        : iFont(aFont)
        , iDevice(aDevice)
        , iIsFontToRelease(EFalse)
        , iName(NULL)
{
}

// ---------------------------------------------------------------------------
// CSwtFont::~CSwtFont
// ---------------------------------------------------------------------------
//
CSwtFont::~CSwtFont()
{
    if (iIsFontToRelease)
    {
        const_cast<MSwtDevice&>(iDevice).GraphicsDevice().ReleaseFont(iFont);
    }

    delete iName;
}

// ---------------------------------------------------------------------------
// CSwtFont::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFont::ConstructL(const CFont& aFont)
{
    CWsScreenDevice* screenDev = CEikonEnv::Static()->ScreenDevice();
    CFont* newFont = NULL;
    User::LeaveIfError(screenDev->GetNearestFontToDesignHeightInTwips(newFont, aFont.FontSpecInTwips()));

    iFont = newFont;
    iIsFontToRelease = ETrue;
    iName = iFont->FontSpecInTwips().iTypeface.iName.AllocL();
}

// ---------------------------------------------------------------------------
// CSwtFont::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtFont::ConstructL(const TDesC& aTypefaceName, const TInt aHeight, const TInt aStyle)
{
    TOpenFontSpec openSpec;
    openSpec.SetName(aTypefaceName.Left(KMaxTypefaceNameLength));
    openSpec.SetHeight(FontUtils::TwipsFromPoints(aHeight));
    openSpec.SetBitmapType(EAntiAliasedGlyphBitmap);
    openSpec.SetBold(aStyle & KSwtBold);
    openSpec.SetItalic(aStyle & KSwtItalic);

    TFontSpec fontSpec;
    openSpec.GetTFontSpec(fontSpec);

    CWsScreenDevice* screenDev = CEikonEnv::Static()->ScreenDevice();
    CFont* newFont = NULL;
    User::LeaveIfError(screenDev->GetNearestFontToDesignHeightInTwips(newFont, fontSpec));

    iFont = newFont;
    iIsFontToRelease = ETrue;
    iName = iFont->FontSpecInTwips().iTypeface.iName.AllocL();
}

// ---------------------------------------------------------------------------
// CSwtFont::NewL
// From MSwtFont
// ---------------------------------------------------------------------------
//
void CSwtFont::Dispose()
{
    // Likely reasons for this assertion to fail:
    //  - The SWT program disposing of its fonts before the objects that
    //    reference them;
    //  - Memory leak in one of the objects referencing the font ( AddRef() was
    //    called but not RemoveRef() ).
#ifdef ESWT_EARLY_DISPOSAL_CHECKING_ENABLED
    ASSERT(RefCount() == 1);
#endif // ESWT_EARLY_DISPOSAL_CHECKING_ENABLED
    RemoveRef();
}

// ---------------------------------------------------------------------------
// CSwtFont::Font
// From MSwtFont
// ---------------------------------------------------------------------------
//
const CFont& CSwtFont::Font() const
{
    ASSERT(iFont);
    return *iFont;
}

// ---------------------------------------------------------------------------
// CSwtFont::DesignHeightInPoints
// From MSwtFont
// ---------------------------------------------------------------------------
//
TInt CSwtFont::DesignHeightInPoints() const
{
    ASSERT(iFont);
    return FontUtils::PointsFromTwips(iFont->FontSpecInTwips().iHeight);
}

// ---------------------------------------------------------------------------
// CSwtFont::GetNameL
// From MSwtFont
// ---------------------------------------------------------------------------
//
HBufC* CSwtFont::GetNameL() const
{
    ASSERT(iFont);
    if (iName)
    {
        return iName->AllocL();
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtFont::Style
// From MSwtFont
// ---------------------------------------------------------------------------
//
TInt CSwtFont::Style() const
{
    ASSERT(iFont);

    TFontSpec fontSpec(iFont->FontSpecInTwips());
    TInt style = KSwtNone;

    if (fontSpec.iFontStyle.StrokeWeight() == EStrokeWeightBold)
    {
        style |= KSwtBold;
    }

    if (fontSpec.iFontStyle.Posture() == EPostureItalic)
    {
        style |= KSwtItalic;
    }

    return style;
}

// ---------------------------------------------------------------------------
// CSwtFont::TextWidthInPixels
// From MSwtFont
// ---------------------------------------------------------------------------
//
TInt CSwtFont::TextWidthInPixels(const TDesC& aText) const
{
    ASSERT(iFont);
    return iFont->TextWidthInPixels(aText);
}

