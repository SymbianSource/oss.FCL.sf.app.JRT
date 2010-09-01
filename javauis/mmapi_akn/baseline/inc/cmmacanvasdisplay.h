/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Draws to Canvas.
*
*/


#ifndef CMMACANVASDISPLAY_H
#define CMMACANVASDISPLAY_H

//  INCLUDES
#include "cmmadisplay.h"

//  FORWARD DECLARATIONS
class CFbsBitmap;
class CMMABitmapWindow;
class MMIDCanvas;

//  CLASS DECLARATION
/**
*   Display for MMIDCanvas objects.
*
*
*/

NONSHARABLE_CLASS(CMMACanvasDisplay): public CMMADisplay
{
public:     // Constructors and destructors
    static CMMACanvasDisplay* NewLC(MMIDCanvas* aCanvas);

    ~CMMACanvasDisplay();  // Destructor ()


public: // From MMMADisplay
    void SourceSizeChanged(const TSize& aSourceSize);
    void SetFullScreenL(TBool aFullScreen);
    void SetDisplayLocationL(const TPoint& aPosition);
    TPoint DisplayLocation();
    void SetWindowL(MMMADisplayWindow* aWindow);

public:
    void MdcContentBoundsChanged(const TRect& aRect);
protected:  // Constructors and destructors

    // Default constructor, protected to allow derivation
    CMMACanvasDisplay(MMIDCanvas* aCanvas);

private:    // Data
    MMIDCanvas* iCanvas;
};

#endif // CMMACANVASDISPLAY_H
