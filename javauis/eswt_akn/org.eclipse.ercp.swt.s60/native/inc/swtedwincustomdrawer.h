/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTEDWINCUSTOMDRAWER_H
#define SWTEDWINCUSTOMDRAWER_H

class CEikEdwin;
class CLafEdwinCustomDrawBase;

/**
 * Class extending CLafEdwinCustomDrawBase so it supports custom clipping
 *
 * This class is used by CSwtEdwin, CSwtIntegerEdwin and CSwtFloatingPointEdwin
 * to implement custom clipping when drawing. When clipping is not applied,
 * edwin controls draw outside of parent controls of CSwtText and
 * CSwtContrainedText.
 */
NONSHARABLE_CLASS(CSwtEdwinCustomDrawer)
        : public CLafEdwinCustomDrawBase
{
public:
    CSwtEdwinCustomDrawer(const MLafEnv& aLafEnv,
    const MFormCustomDraw* aParentDraw, const CEikEdwin& aEdwin);

    ~CSwtEdwinCustomDrawer();

    /**
     * Sets custom clipping rectangle.
     */
    void SetClippingRect(const TRect& aRect);

// From CLafEdwinCustomDrawBase
public:
    void DrawBackground(const TParam& aParam, const TRgb& aBackground,
                        TRect& aDrawn) const;

    void DrawLineGraphics(const TParam& aParam,
                          const TLineInfo& aLineInfo) const;

    void DrawText(const TParam& aParam, const TLineInfo& aLineInfo,
                  const TCharFormat& aFormat, const TDesC& aText,
                  const TPoint& aTextOrigin, TInt aExtraPixels) const;

#ifdef RD_JAVA_S60_RELEASE_9_2
    void DrawText(const TParam& aParam, const TLineInfo& aLineInfo,
                  const TCharFormat& aFormat, const TDesC& aText,
                  const TInt aStart, const TInt aEnd, const TPoint& aTextOrigin,
                  TInt aExtraPixels) const;
#endif
    
    TRgb SystemColor(TUint aColorIndex, TRgb aDefaultColor) const;

private:
    /**
     * Control owning the drawer.
     */
    const CEikEdwin& iEdwin;
    
    /**
     * Original drawer used for actual drawing.
     * Not own.
     */
    const MFormCustomDraw* iEdWinDrawer;
    
    /**
     * Clipping rectangle.
     * Applied before iEdWinDrawer draws.
     */
    TRect iClippingRect;
};

#endif // SWTEDWINCUSTOMDRAWER_H
