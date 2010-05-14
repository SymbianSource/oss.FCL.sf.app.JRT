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
#include "jni.h"

//  FORWARD DECLARATIONS
class CFbsBitmap;
class CMMABitmapWindow;
//class MMAFunctionServer;
//class MMIDCanvas;

//  CLASS DECLARATION
/**
*   Display for MMIDCanvas objects.
*
*
*/

NONSHARABLE_CLASS(CMMACanvasDisplay): public CMMADisplay
{
public:     // Constructors and destructors
    static CMMACanvasDisplay* NewLC(MMAFunctionServer* aEventSource , jobject obj/*MMIDCanvas* aCanvas*/);

    ~CMMACanvasDisplay();  // Destructor ()


public: // From MMMADisplay
    //void SourceSizeChanged(const TSize& aSourceSize);
    void SetFullScreenL(TBool aFullScreen);
    void SetDisplayLocationL(const TPoint& aPosition);
    TPoint DisplayLocation();
    void SetWindowL(MMMADisplayWindow* aWindow);

private:
    /** ask java side peer about the bound
	Returns a rectangle describing the receiver's size
	and location relative to its parent (or its display if its parent is null),
	unless the receiver is a shell. In this case, the location is relative to the display
	*/
   TRect& CMMACanvasDisplay::BoundRect();
   TRect& CMMACanvasDisplay::ContainerWindowRect();

//public:
//    void MdcContentBoundsChanged(const TRect& aRect);
protected:  // Constructors and destructors

    // Default constructor, protected to allow derivation
    CMMACanvasDisplay(MMAFunctionServer* aEventSource , jobject aJavaDisplayRef/*MMIDCanvas* aCanvas*/);

private:    // Data
//    MMIDCanvas* iCanvas;

//MMAFunctionServer* iEventSource; // not owned

};

#endif // CMMACANVASDISPLAY_H
