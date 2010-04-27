/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation class for TextBox custom drawer
*
*/


#ifndef CMIDTEXTBOXEDWINCUSTOMDRAW_H
#define CMIDTEXTBOXEDWINCUSTOMDRAW_H

// ========== INCLUDE FILES ================================

#include <e32base.h>
#include <lafmain.h>
#include <AknUtils.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CGulIcon;
class CEikEdwin;

// ========== CLASS DECLARATION ============================

/**
*  Implementation of MFormCustomDraw
*       Class is used to draw editor lines to text editor.
*
* @since 3.1
*/
NONSHARABLE_CLASS(CMIDTextBoxEdwinCustomDraw): public CLafEdwinCustomDrawBase
{
public:

    /**
    * Static constructor.
    *
    * @param    aEnv reference to LAF interface.
    * @param    aParentCustomDraw ref on default implementation instance
    * @param    aParentControl ref to the parent control (edwin)
    *
    * @return   pointer to instance
    */
    static CMIDTextBoxEdwinCustomDraw* NewL(const MLafEnv& aEnv,
    const MFormCustomDraw* aParentCustomDraw,
    const CEikEdwin* aParentControl);
    /**
    * Destructor
    */
    virtual ~CMIDTextBoxEdwinCustomDraw();

    /**
     * Calculates layout with given parent rectangle
     * @param aParentRect an editor rectangle
     */
    void CalculateLayout(TRect aParentRect);

public: // Functions from base classes

    /**
    * From MFormCustomDraw
    * See frmtlay.h for more information.
    */
    void DrawBackground(const TParam& aParam,
                        const TRgb& aRgb,
                        TRect& aDrawn) const;

    /**
    * From MFormCustomDraw
    * See frmtlay.h for more information.
    */
    void DrawText(const TParam& aParam,
                  const TLineInfo& aLineInfo,
                  const TCharFormat& aFormat,
                  const TDesC& aText,
                  const TPoint& aTextOrigin,
                  TInt aExtraPixels) const;

    /**
    * From MFormCustomDraw
    * See frmtlay.h for more information.
    */
    void DrawLineGraphics(const TParam& aParam,
                          const TLineInfo& aLineInfo) const;

    /**
    * From MFormCustomDraw
    * See frmtlay.h for more information.
    */
    TRgb SystemColor(TUint aColorIndex,
                     TRgb aDefaultColor) const;

private:

    /**
    * C++ contructor
    */
    CMIDTextBoxEdwinCustomDraw(const MLafEnv& aEnv,
                               const MFormCustomDraw* aParentCustomDraw,
                               const CEikEdwin* aParentControl);

    /**
    * 2nd phase constructor
    */
    void ConstructL();

    /**
    * Draws editor lines
    * @param aParam, contains also reference on GraphicsContext
    * @param aDrawn a rectangle that needs to be drawn
    */
    void DrawEditorLines(const TParam& aParam, TRect& aDrawn) const;

    /**
     * Create the rectangles used for setting the shape of our window.
     * There is a rectanlge for each line. If the line number increases we
     * create new rectangles. This may happen when changing screen resolution.
     */
    void CreateRectsIfNeededL();

    /**
     * Get the layout used to position the horizontal graphic lines (grid)
     * @note Sets the delta between lines: iLineDelta
     *
     * @param aRect Edwin area without scrollbar
     * @param aLineTopOffset output the top margin of list_single_midp_pane,
     *  the offset of the first top graphic line
     * @return layout for horizontal graphic lines
     */
    TAknLayoutRect GetLayoutRectAndSetLineDelta(TRect& aRect,
            TUint8& aLineTopOffset);

public:
    /**
     * Update iIcon with actual skin.
     */
    void UpdateIconL();

private: // data
    // height of each line
    TInt iLineDelta;
    // icon for the line drawing
    CGulIcon* iIcon;
    // ref to the parent control (edwin)
    const CEikEdwin* iParentControl;
    // array of rectangles
    CArrayPtrFlat<TRect>* iRects;
    // number of lines in editor
    TUint iNumberOfLinesInBody;
    // ref on default implementation instance, we own it
    const MFormCustomDraw* iParentCustomDraw;
};

#endif // CMIDTEXTBOXEDWINCUSTOMDRAW_H

// End of File
