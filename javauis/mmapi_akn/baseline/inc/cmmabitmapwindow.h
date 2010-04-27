/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This abstract class implements MMMADisplayWindow functionality
*                in CFbsBitmap based displays.
*
*/


#ifndef CMMABITMAPWINDOW_H
#define CMMABITMAPWINDOW_H

//  INCLUDES
#include "mmmadisplaywindow.h"

//  CONSTANTS

//  FORWARD DECLARATIONS
class CFbsBitmap;
class CFbsBitmapDevice;
class CGraphicsContext;

//  CLASS DECLARATION
/**
*   This abstract class implements MMMADisplayWindow functionality in
*   CFbsBitmap based displays.
*
*
*/


NONSHARABLE_CLASS(CMMABitmapWindow): public CBase,
        public MMMADisplayWindow
{
public:     // Constructors and destructors
    ~CMMABitmapWindow();   // Destructor ()

    static CMMABitmapWindow* NewL();

protected:  // Constructors and destructors
    // Default constructor, protected to allow derivation
    CMMABitmapWindow();

public: // Methods derived from MMMADisplayWindow
    void SetDestinationBitmapL(CFbsBitmap* aBitmap);
    void DrawFrameL(const CFbsBitmap* aBitmap);
    void SetDrawRect(const TRect& aRect);
    void SetDrawRectThread(const TRect& aRect);
    const TRect& DrawRect();
    TSize WindowSize();
    void SetPosition(const TPoint& aPosition);
    void SetVisible(TBool aVisible, TBool aUseEventServer = ETrue);
    void SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType aThreadType);
    const TRect& WindowRect();

protected:  // Data
    /**
     * Pointer to the bitmap that is used for drawing.
     * iBitmap is owned by this class.
     */
    CFbsBitmap* iBitmap;

    /**
     * Owned bitmap device
     */
    CFbsBitmapDevice* iBitmapDevice;

    /**
     * Owned bitmap context
     */
    CGraphicsContext* iBitmapContext;

    /**
     * Actual area used for drawing. Set by SetDrawRect method.
     */
    TRect iDrawRect;
    TRect iClientRect;

    inline TDisplayWindowType GetDisplayWindowType() const
    {
        return EDisplayWindowTypeIsBitmap;
    }
};

#endif // CMMABITMAPWINDOW_H
