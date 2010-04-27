/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CMIDFont for series 60
*
*/


#include <eikenv.h>
#include <coemain.h>
#include <barsread.h>
#include <lcdui.rsg>
#include <fontids.hrh>
#include <gulfont.h>
// API used when font is created
#include <AknUtils.h>
#include <aknenv.h>
// API from CAknLayoutFont related to fonts
#include <AknLayoutFont.h>
#include <j2me/jdebug.h>
#include <aknlayoutscalable_avkon.cdl.h> // for layout data

#include "CMIDFont.h"
// API used for checking graphic scaling related parameters
#include "CMIDMenuHandler.h"
#include "CMIDUIManager.h"

// Indexes for the font size table below
enum TColumnIndexes
{
    EIndexScreenWidth  = 0,
    EIndexScreenHeight = 1,
    EIndexSmallFont    = 2,
    EIndexMediumFont   = 3,
    EIndexLargeFont    = 4,
    EResolutionCount   = 6
};

// Table including the font sizes for known S60 resolutions
// The order of the numbers in one table row:
// 1. screen width
// 2. screen height
// 3. small font height
// 4. medium font height
// 5. large font heigth
// This table must be updated when new resolutions become available in S60.
const TInt KFontSizes[EResolutionCount][5] =
{
    { 176, 208, 13, 15, 17 }, // Low
    { 240, 320, 19, 21, 24 }, // QVGA
    { 320, 480, 25, 28, 32 }, // HVGA
    { 352, 416, 26, 30, 34 }, // Double
    { 360, 640, 24, 27, 30 }, // nHD
    { 480, 640, 32, 36, 41 }  // VGA
};

// Constant for calculating the font heights when in graphic scaling case
// the original MIDlet resolution is smaller than any known S60 resolution.
// These are the averages of screen width / font height ratios of
// Low, QVGA, Double and HVGA resolutions.
const TReal KSmallFontRatioToScreenWidth  = 13.13;
const TReal KMediumFontRatioToScreenWidth = 11.58;
const TReal KLargeFontRatioToScreenWidth  = 10.18;

// Minimum font height. This is used as font height if calculated font
// height would be smaller than this.
const TInt KMinFontHeight = 11;

// Default value of iScalingFactor.
const TReal KNonScaled = 1.0;

// Zero size (0, 0).
#define KZeroSize ::TSize()

MMIDFont* CMIDFont::NewL(TUint aStyle, TUint aSize, TInt aFace,
                         CMIDUIManager* aUIManager, TBool aIsFreeSizeFont)
{
    CMIDFont* font = new(ELeave) CMIDFont(aUIManager);
    CleanupStack::PushL(font);
    font->ConstructL(aStyle, aSize, aFace, aIsFreeSizeFont);
    CleanupStack::Pop(font);
    return font;
}

CMIDFont::CMIDFont(CMIDUIManager* aUIManager)
        : iUIManager(aUIManager), iScalingFactor(KNonScaled)
{
}

/**
 * @return the MIDP font height - including external leading.
 */
TInt CMIDFont::Height()
{
    const CAknLayoutFont* layFont =  CAknLayoutFont::AsCAknLayoutFontOrNull(Font());
    // We are using only CAknLayoutFonts, so panic is raised if the layFont is null.
    ASSERT(layFont);
    return layFont->TextPaneHeight();
}

/**
 * @return vertical distance from top of font to the baseline.
 */
TInt CMIDFont::Baseline()
{
    return Font()->FontMaxAscent();
}

TInt CMIDFont::Width(const TDesC& aString)
{
    return Font()->TextWidthInPixels(aString);
}

CMIDFont::~CMIDFont()
{
    const CAknLayoutFont* layFont =  CAknLayoutFont::AsCAknLayoutFontOrNull(iFont);
    if (layFont)
    {
        delete layFont;
        iFont = NULL;
    }
}

void CMIDFont::ConstructL(TUint aStyle, TUint aSize, TInt /*aFace*/,
                          TBool aIsFreeSizeFont)
{
    // Ensure that menu handler exists. It will be needed later.
    iUIManager->OpenMenuHandlerL();

    // saving of flag iUnderline
    iUnderlined = aStyle & EUnderlined;

    // We don't need underline flag in aStyle.
    aStyle &= ~EUnderlined;

    // saving of size of font
    iSize = aSize;

    // saving iIsFreeSizeFont flag
    iIsFreeSizeFont = aIsFreeSizeFont;

    // saving of iBold flag
    if (aStyle & EBold)
    {
        iBold = ETrue;
    }
    else
    {
        iBold = EFalse;
    }

    // saving of iItalic flag
    if (aStyle & EItalic)
    {
        iItalic = ETrue;
    }
    else
    {
        iItalic = EFalse;
    }

    // seting of orginal size and scaling factor.
    SetOrigFontHeight();

    // checking of this is really resizible font
    if (iIsFreeSizeFont)
    {
        // In case of font with custom height simply set the same value as in iSize
        iPaneHeight = aSize;
    }
    else
    {
        // If is not downscaling on and this is not freely resizible font, then we use predefined size of font.
        iPaneHeight = GetTextPaneHeight(ETrue);
    }

    iFont = NULL;

    // creating of new iFont
    CreateNewFont();
}

const CFont* CMIDFont::DefaultFont(const TDefaultId& aId)
{
    if (aId == EDefaultTextId)
    {
        return AknLayoutUtils::FontFromId(
                   AknLayoutScalable_Avkon::form2_mdip_string_pane_t1(1,0).Font(), NULL);
    }
    else if (aId == EDefaultLabelId)
    {
        return AknLayoutUtils::FontFromId(
                   AknLayoutScalable_Avkon::form2_midp_label_pane_t1(0).Font(), NULL);
    }
    else
    {
        ASSERT(0); // unsupported default font id
        return NULL;
    }
}

// deprecated, use DefaultFont(EDefaultLabelId)
const CFont* CMIDFont::DefaultLabelFont()
{
    return DefaultFont(EDefaultLabelId);
}

/**
 * Returns iFont that is a pointer to CFont object.
 * Parameter aHighLevelUIComponent indicates whether this method has been called
 * from a non-high level ui component (Graphics), or a high level UI component (List, StringItem, etc.).
 */
CFont* CMIDFont::Font(TBool aHighLevelComponent)
{
    TBool useScaling = !aHighLevelComponent;

    TInt paneHeight;
    if (iIsFreeSizeFont)
    {
        // free size font is independent on resolution, so it should not be
        // re-created if resolution is changed
        paneHeight = iPaneHeight;
    }
    else
    {
        // standard font depends on resolution, if resolution is changed
        // pane height is changed too
        paneHeight = GetTextPaneHeight(useScaling);
    }

    // Check if the font size has changed e.g. because of the dynamic
    // resolution change. If so re-create the font.
    if (iPaneHeight != paneHeight)
    {
        iPaneHeight = paneHeight;
        CreateNewFont();
    }

    if (iFont)
    {
        return iFont;
    }
    else
    {
        // returns the default font if font creation has failed
        return const_cast<CFont*>(CMIDFont::DefaultFont(EDefaultTextId));
    }

}

/**
 * CreteNewFont function creates a new iFont object.
 * Font specification attributes are retrieved from the class member variables.
 */
void CMIDFont::CreateNewFont()
{
    // delete existing iFont object
    const CAknLayoutFont* layFont =  CAknLayoutFont::AsCAknLayoutFontOrNull(iFont);
    if (layFont)
    {
        delete layFont;
        iFont = NULL;
    }

    // init spec
    TAknFontSpecification spec(EAknLogicalFontSecondaryFont);
    // size
    spec.SetTextPaneHeight(iPaneHeight);
    // italic
    if (iItalic)
        spec.SetPosture(EPostureItalic);
    // bold
    if (iBold)
        spec.SetWeight(EStrokeWeightBold);

    TInt err = 0;
    DEBUG("CMIDFont::CreateNewFont - calling CreateLayoutFontFromSpecificationL");
    TRAP(err, iFont = AknLayoutUtils::CreateLayoutFontFromSpecificationL(spec));   // Now owned by the client code
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDFont::CreateNewFont - Exception from AknLayoutUtils::CreateLayoutFontFromSpecificationL. Error = %d", err);
    }
}

TBool CMIDFont::IsUnderlined() const
{
    return iUnderlined;
}

void CMIDFont::Dispose()
{
    delete this;
}


/**
 * This function returns the height of the text pane for the current font size.
 * The text pane height value is retrieved from the LAF.
 * If the aScaling is true and graphic scaling is effective in current device screen,
 * the font height in the original MIDlet resolution is returned.
 */
TInt CMIDFont::GetTextPaneHeight(TBool aScaling)
{
    // iOrigFontHeight > 0 only when orignial MIDlet resolution has been defined in .jad
    if (aScaling && iOrigFontHeight > 0 &&
            iUIManager->GetMenuHandler()->IsScalingEffectiveInCurrentScreen())
    {
        return iOrigFontHeight;
    }

    TInt paneHeight;
    if (iSize == MMIDFont::ESmall)
    {
        //Small size is retrieved from the form item label LAF
        paneHeight = AknLayoutScalable_Avkon::form2_midp_label_pane_t1(0).H();
    }
    else if (iSize == MMIDFont::ELarge)
    {
        paneHeight = AknLayoutScalable_Avkon::form2_mdip_string_pane_t1(2,0).H();
    }
    else // EMedium
    {
        paneHeight = AknLayoutScalable_Avkon::form2_mdip_string_pane_t1(1,0).H();
    }

    return paneHeight;
}


void CMIDFont::SetOrigFontHeight()
{
    iOrigFontHeight = 0;

    if (!iUIManager->GetMenuHandler()->IsScalingEnabled())
    {
        return;
    }

    ::TSize origResolution =
        iUIManager->GetMenuHandler()->GetScalingParameterOrgMIDletScrSize();

    // ensure that resolution is in portrait orientation
    ::TSize resolution(Min(origResolution.iWidth, origResolution.iHeight),
                       Max(origResolution.iWidth, origResolution.iHeight));

    TInt fontSizeIndex = 0;
    switch (iSize)
    {
    case MMIDFont::ESmall:
        fontSizeIndex = EIndexSmallFont;
        break;
    case MMIDFont::EMedium:
        fontSizeIndex = EIndexMediumFont;
        break;
    case MMIDFont::ELarge:
        fontSizeIndex = EIndexLargeFont;
        break;
    default:
        break;
    }

    // These indexes are for the case that original resolution is not a known
    // resolution and font height needs to be calculated based on the font heights
    // in KFontSizes table.
    TInt smallerResolutionIndex = KErrNotFound;
    TInt biggerResolutionIndex = KErrNotFound;

    for (TInt i = 0; i < EResolutionCount && biggerResolutionIndex == KErrNotFound; ++i)
    {
        if (KFontSizes[i][EIndexScreenWidth] == resolution.iWidth)
        {
            iOrigFontHeight = KFontSizes[i][fontSizeIndex];
            return;
        }

        if (KFontSizes[i][EIndexScreenWidth] < resolution.iWidth)
        {
            smallerResolutionIndex = i;
        }
        else if (KFontSizes[i][EIndexScreenWidth] > resolution.iWidth &&
                 biggerResolutionIndex == KErrNotFound)
        {
            biggerResolutionIndex = i;
        }
    }

    // Orig resolution is not a known S60 resolution, need to calculate an estimate for
    // original font height based on the screen width of the original resolution.
    if (iOrigFontHeight == 0)
    {
        // First check if the original MIDlet resolution is smaller than any known
        // S60 resolution. If so, the original font height estimate is calculated
        // by using the KXXXFontRatioToScreenWidth constants.
        if (smallerResolutionIndex == KErrNotFound)
        {
            TReal fontSizeRatio = KSmallFontRatioToScreenWidth;

            switch (iSize)
            {
            case MMIDFont::ESmall:
                fontSizeRatio = KSmallFontRatioToScreenWidth;
                break;
            case MMIDFont::EMedium:
                fontSizeRatio = KMediumFontRatioToScreenWidth;
                break;
            case MMIDFont::ELarge:
                fontSizeRatio = KLargeFontRatioToScreenWidth;
                break;
            default:
                break;
            }

            iOrigFontHeight = TReal(resolution.iWidth) / fontSizeRatio;
            iOrigFontHeight = Max(KMinFontHeight, iOrigFontHeight);
        }
        else if (biggerResolutionIndex == KErrNotFound)
        {
            // Original screen width is bigger than the highest resolution in the KFontSizes table.
            // Use the font size defined for the highest resolution in the table.
            iOrigFontHeight = KFontSizes[EResolutionCount - 1][fontSizeIndex];
        }
        else
        {
            // The screen width of the original MIDlet resolution is between 2 known S60 screen
            // widths. Calculate estimate for font height based on the heights of the 2 known resolutions.
            TReal fontHeightDelta = KFontSizes[biggerResolutionIndex][fontSizeIndex] -
                                    KFontSizes[smallerResolutionIndex][fontSizeIndex];

            TReal screenWidthDelta = KFontSizes[biggerResolutionIndex][EIndexScreenWidth] -
                                     KFontSizes[smallerResolutionIndex][EIndexScreenWidth];

            TReal origScreenWidthDelta = resolution.iWidth -
                                         KFontSizes[smallerResolutionIndex][EIndexScreenWidth];

            iOrigFontHeight = KFontSizes[smallerResolutionIndex][fontSizeIndex] +
                              ((origScreenWidthDelta / screenWidthDelta) *
                               fontHeightDelta);
        }
    }
}

/**
 * @return ETrue if font is bold, EFalse if not.
 */
TBool CMIDFont::IsBold()
{
    return iBold;
}

/**
 * @return ETrue if font is italic, EFalse if not.
 */
TBool CMIDFont::IsItalic()
{
    return iItalic;
}
// End of File

