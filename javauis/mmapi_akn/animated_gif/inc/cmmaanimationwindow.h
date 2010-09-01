/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMMAANIMATIONWINDOW_H
#define CMMAANIMATIONWINDOW_H

//  INCLUDES
#include <MIHLImageViewer.h>
//#include "MMMADisplayWindow.h"
#include "cmmabitmapwindow.h"
#include "cmmaeventsource.h"

//  CONSTANTS

//  FORWARD DECLARATIONS
//class CFbsBitmap;


//  CLASS DECLARATION
/**
*   This abstract class implements MMMADisplayWindow functionality in
*   CFbsBitmap based displays.
*
*
*/


NONSHARABLE_CLASS(CMMAAnimationWindow): public CMMABitmapWindow
{
public:     // Constructors and destructors
    ~CMMAAnimationWindow();   // Destructor ()

    static CMMAAnimationWindow* NewL(CMMAEventSource* aEventSource);

protected:  // Constructors and destructors
    // Default constructor, protected to allow derivation
    CMMAAnimationWindow(CMMAEventSource* aEventSource);

public: // new methods
    /**
     * Setter for viewer, used for changing the size
     */
    void SetViewer(MIHLImageViewer* aViewer);

public: // Methods derived from MMMADisplayWindow
    void SetDestinationBitmapL(CFbsBitmap* aBitmap);
    void DrawFrameL(const CFbsBitmap* aBitmap);
    void SetDrawRect(const TRect& aRect);
    void SetDrawRectThread(const TRect& aRect);

    /*private:
        static void StaticSetDrawRect(
            CMMAAnimationWindow* aWindow,
            TRect aRect,
            CMMAEventSource* aEventSource );*/

protected:  // Data
    /**
     * not owned, used for switching from UI thread to MMA thread
     */
    CMMAEventSource* iEventSource;

    /**
     * Pointer to bitmap context, used for BitBlt instead of
     * slow DrawRect
     */
    CBitmapContext* iBitContext;

    /**
     * Viewer used for setting new size, not owned
     */
    MIHLImageViewer* iViewer;
};

#endif // CMMAANIMATIONWINDOW_H
