/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Render context interface
*
*/

#ifndef MM2GRENDERCONTEXT_H
#define MM2GRENDERCONTEXT_H

//  INCLUDES
#include <gdi.h> // TDisplayMode
#include "M2GUtils.h"

M2G_NS_START

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
 * Interface class
 */
class MM2GRenderContext
{
public: // STATIC CONSTANTS
    static const TReal32        KFullOpaque;
    static const TReal32        KFullTransparency;
    static const TUint8         KMaxAlphaValue;
    static const TDisplayMode   KDefaultDisplayMode;
    static const TDisplayMode   KMaskDisplayMode;

public: // METHODS

    /**
     * Dtor
     */
    virtual ~MM2GRenderContext() {};

    /**
     * Binds device's bitmap.
     * @since Series S60 3.1
     * @param aGraphicsHandle Target graphics handle
     * @throws exception if not ok
     */
    virtual void BindL(TInt& aGraphicsHandle) = 0;

    /**
     * SVG image bitmap is (re)created if needed.
     * @since Series S60 3.1
     * @see MM2GRenderContext::SetRefreshModeL()
     * @throws exception if not ok
     */
    virtual void InitImageBitmapL() = 0;

    /**
     * Blits an svg and a device bitmaps
     * @since Series S60 3.1
     * @throws M2G_OK if ok
     * @throws exception if not ok
     */
    virtual void ReleaseL() = 0;

    /**
     * Renders a SVG document at a current time.
     *
     * <p> The rendering is done in 2 steps:
     * <li> render the document on an internal SVG buffer (iSvg)
     * <li> alpha-blend it with the bound graphics target (iGraphicsBitmap)
     *
     * @note if transparency factor is not set, iSvg is simply blitted onto iGraphicsTarget
     *
     * @param aSvgDocumentHandle handle to SVG document
     * @param aCurrentTime       the current time of the animation
     * @param aSvgW              Svg image viewport width
     * @param aSvgH              Svg image viewport height
     * @param aRect              contains position about the rendered area
     * @since S60 3.1
     */


    virtual void  RenderLCDUIL(
        TM2GSvgDocumentHandle& aSvgDocHandle,
        const TReal32 aCurrentTime,
        TInt aSvgW,
        TInt aSvgH,
        TM2GRenderRect& aRect) = 0 ;

    virtual void RenderESWTL(
        TM2GSvgDocumentHandle& aSvgDocHandle,
        const TReal32 aCurrentTime,
        TInt aSvgW,
        TInt aSvgH,
        TM2GRenderRect& aRect,
        TBool aUseNativeClear,
        TInt* aReturnData) = 0;

    /**
     * Sets render quality
     * @since Series S60 3.1
     * @param aMode Mode
     * @throws exception if not ok
     */
    virtual void SetRenderingQualityL(TInt aMode) = 0;

    /**
     * Sets transparency
     * @since Series S60 3.1
     * @param aAlpha Alpha factor
     * @throws exception if not ok
     */
    virtual void SetTransparency(TReal32 aAlpha) = 0;

};

M2G_NS_END
#endif // MM2GRENDERCONTEXT_H
