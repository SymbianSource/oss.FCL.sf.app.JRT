/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Defines custom drawing for text editor edwin.
*
*/


#ifndef CMIDTEXTEDITOREDWINCUSTOMDRAW_H
#define CMIDTEXTEDITOREDWINCUSTOMDRAW_H

// INTERNAL INCLUDES
#include "CMIDTextEditorEdwin.h"

// EXTERNAL INCLUDES
#include <lafmain.h>

// CLASS DESCRIPTION
/**
 * Defines custom drawing for text editor edwin.
 *
 * Currently, only selection color is handled in this class. All other
 * drawing is delegated to the parent custom drawer.
 */
NONSHARABLE_CLASS(CMIDTextEditorEdwinCustomDraw) :
        public CLafEdwinCustomDrawBase
{

public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance
     *
     * @param aLafEnv Reference to LAF interface.
     * @param Reference to the parent custom drawer.
     * @param aEdwin The editor window to which this custom draw
     *        is associated to.
     * @return New instance from this class
     */
    CMIDTextEditorEdwinCustomDraw(
        const MLafEnv& aLafEnv,
        const MFormCustomDraw& aParentDraw,
        const CMIDTextEditorEdwin& aEdwin);

    /**
     * Destructor.
     */
    virtual ~CMIDTextEditorEdwinCustomDraw();

public: // From CLafEdwinCustomDrawBase

    /**
     * Called by the editor FW in order to draw the background for
     * the editor window.
     *
     * Currently ignores any drawing related to background.
     *
     * @param aParam Drawing parameters
     * @param aBackground Default background colour
     * @param aDrawn On return, the rectangle to which the function has
     *        drawn
         * @since S60 5.0
     */
    void DrawBackground(
        const TParam& aParam,
        const TRgb& aBackground,
        TRect& aDrawn) const;

    /**
     * Called by the editor FW in order to draw the line
     * graphics for the editor window.
     *
     * Currently delegates the drawing to the parent custom drawer.
     *
     * @param aParam Drawing parameters
     * @param aLineInfo The line information.
         * @since S60 5.0
     */
    void DrawLineGraphics(
        const TParam& aParam,
        const TLineInfo& aLineInfo) const;

    /**
     * Called by the editor FW in order to draw the the content of
     * the editor.
     *
     * Currently delegates the drawing to the parent custom drawer.
     *
     * @param aParam Drawing parameters.
     * @param aLineInfo The line information.
     * @param aFormat The current character format.
     * @param aText The content to draw.
     * @param aTextOrigin The origin of the text.
     * @param aExtraPixels The amount of extra pixels.
         * @since S60 5.0
     */
    void DrawText(
        const TParam& aParam,
        const TLineInfo& aLineInfo,
        const TCharFormat& aFormat,
        const TDesC& aText,
        const TPoint& aTextOrigin,
        TInt aExtraPixels) const;
    
#ifdef RD_JAVA_S60_RELEASE_9_2
    /**
     * Called by the editor FW in order to draw the the content of
     * the editor.
     *
     * Currently delegates the drawing to the parent custom drawer.
     *
     * @param aParam Drawing parameters.
     * @param aLineInfo The line information.
     * @param aFormat The current character format.
     * @param aText The content to draw.
     * @param aStart 
     * @param aEnd 
     * @param aTextOrigin The origin of the text.
     * @param aExtraPixels The amount of extra pixels.
     * @since S60 5.0
     */
    void DrawText(
        const TParam& aParam,
        const TLineInfo& aLineInfo,
        const TCharFormat& aFormat,
        const TDesC& aText,
        const TInt aStart,
        const TInt aEnd,
        const TPoint& aTextOrigin,
        TInt aExtraPixels) const;
#endif
    
    /**
     * Retrieves the system color for the specified color index.
     *
     * @param aColorIndex The index of the color to be returned.
     * @param aDefaultColor The default color for the specified index.
     * @return The system's (or custom) color for the specified color
     *         index.
     * @since S60 5.0
     */
    TRgb SystemColor(
        TUint aColorIndex,
        TRgb aDefaultColor) const;

private: // Data

    // Reference to the editor window. Used.
    const CMIDTextEditorEdwin& iEdwin;

    // Reference to the parent drawer. Used.
    const MFormCustomDraw& iParentDraw;
};

#endif // CMIDTEXTEDITOREDWINCUSTOMDRAW_H

// End of file
