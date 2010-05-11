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
* Description:  Render context
*
*/

#ifndef CM2GRENDERCONTEXT_H
#define CM2GRENDERCONTEXT_H

//  INCLUDES
#include <fbs.h>
#include "MM2GRenderContext.h"

M2G_NS_START

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MM2GSVGProxy;
class CBitmapContext;
#ifdef RD_JAVA_NGA_ENABLED
class MMIDCanvas;
#endif // RD_JAVA_NGA_ENABLED

// CLASS DECLARATION
/**
 * Render context
 */
class CM2GRenderContext : public CBase, public MM2GRenderContext
{
private: // METHODS
    /**
     * C++ default constructor.
     */
    CM2GRenderContext();

    /**
     * C++ copy constructor
     */
    CM2GRenderContext(const CM2GRenderContext&);

    /**
     * C++ assignment operator
     */
    CM2GRenderContext& operator=(const CM2GRenderContext&);

    /**
     * Initialization
     */
    void ConstructL(MM2GSVGProxy* aProxy);

public: // METHODS
    /**
     * Two-phased constructor.
     */
    static CM2GRenderContext* NewL(MM2GSVGProxy* aProxy);

    /**
     * Destructor.
     */
    virtual ~CM2GRenderContext();

    /**
     * @see MM2GRenderContext::BindL()
     */
    virtual void BindL(TInt& aGraphicsHandle, TInt aUiToolKit);

    /**
     * @see MM2GRenderContext::InitImageBitmapL()
     */
    virtual void InitImageBitmapL();

    /**
     * @see MM2GRenderContext::BlitToL()
     */
    virtual void ReleaseL();

    /**
     * @see MM2GRenderContext::RenderL()
     */
    virtual void RenderLCDUIL(
        TM2GSvgDocumentHandle& aSvgDocumentHandle,
        const TReal32 aCurrentTime,
        TInt aSvgW, TInt aSvgH,
        TM2GRenderRect& aRect);
    virtual void RenderESWTL(
        TM2GSvgDocumentHandle& aSvgDocHandle,
        const TReal32 aCurrentTime,
        TInt aSvgW, TInt aSvgH,
        TM2GRenderRect& aRect,
        MSwtClient* aClientHandle,
        TBool aUseNativeClear,
        TInt* aReturnData);

    /**
     * @see MM2GRenderContext::SetRenderingQualityL()
     */
    virtual void SetRenderingQualityL(TInt aMode);

    /**
     * @see MM2GRenderContext::SetTransparency()
     */
    virtual void SetTransparency(TReal32 aAlpha);

protected: // METHODS
    /**
     * Init mask
     *
     * @param aSize the size of the mask
     */
    void InitMaskL(TSize aSize);

    /**
     * Get the image's bitmap handle
     *
     * @return Svg surface handle.
     * @throws Exception if not ok.
     */
    TM2GBitmapHandle GetImgHandleL() const;

private:
    /**
     * Prepare viewbox and anchor
     *
     * @param aRr clip area information
     * @return aViewbox render rectangle area
     * @return aAnchor recalculated anchor point
     */
    void PrepareViewbox(
        TM2GRenderRect& aRr,
        TInt aSvgW, TInt aSvgH,
        TRect& aViewbox, TPoint& aAnchor);

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
     * @param aAnchor            anchor point
     *
     * @since S60 3.1
     */
    void RenderLCDUIL(
        TM2GSvgDocumentHandle& aSvgDocumentHandle,
        const TReal32 aCurrentTime,
        const TRect& aViewbox,
        const TPoint& aAnchor);

    void RenderESWTL(
        TM2GSvgDocumentHandle& aSvgDocHandle,
        TReal32 aCurrentTime,
        const TRect& aViewbox,
        const TPoint& aAnchor,
        MSwtClient* aClientHandle,
        TBool aUseNativeClear,
        TInt* aReturnData);
    /**
     * Creates the mask used for doing alpha blending
     *
     * @note Does not do anything if fully opaque
     *
     * @param aMask pointer to mask
     * @param aMaskSize mask size
     */
    void CreateAlphaBlendMaskL(const TSize& aViewportSize);


    /**
     * Clear the bitmap with 0
     * Fills all pixel with 0 value
     *
     * @param aBmp the bitmap to be filled
     */
    void CM2GRenderContext::ClearBitmapL(CFbsBitmap* aBmp);

    /**
     * Fills all the pixels of a bitmap with a specific byte
     *
     * @param aBmp the bitmap to be filled
     * @param aChar the character(byte) to fill with
     */
    void FillBitmapL(CFbsBitmap* aBmp, const TUint8& aChar);


private: // VARIABLES
    MM2GSVGProxy*       iProxy;

    TM2GSvgEngineHandle iEngineHandle;

    TReal32             iAlpha;
    TUint8              iScaledAlpha;

    CFbsBitmap*         iMaskBmp;
    CFbsBitmap*         iTargetBmp;
#ifdef RD_JAVA_NGA_ENABLED
    MMIDCanvas*         iTargetCanvas;
#endif // RD_JAVA_NGA_ENABLED
    
//for eswt compatibility
    CFbsBitmap*         iImgBmp;
    RFbsSession         iFbsSession;
};
/*-----------------------------------------------------------
class  : CFbsBitmapHack
purpose: Class is provided as a solution for double render
         issue in eSWT
*-----------------------------------------------------------*/
class CFbsBitmapHack : public CFbsBitmap
{
public:
    TInt GetMyHandle()
    {
        return iHandle;
    }
};


M2G_NS_END
#endif // CM2GRENDERCONTEXT_H
