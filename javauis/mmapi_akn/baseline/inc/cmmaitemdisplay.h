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
* Description:  Bitmap display that draws to the Java CustomItem's bitmap.
*
*/


#ifndef CMMAITEMDISPLAY_H
#define CMMAITEMDISPLAY_H

//  INCLUDES
#include "cmmadisplay.h" // base class

class MMIDCustomItem;

//  CLASS DECLARATION
/**
*   Display that uses MMIDCustomItem.
*
*
*/
NONSHARABLE_CLASS(CMMAItemDisplay): public CMMADisplay
{
public:     // Constructors and destructors
    static CMMAItemDisplay* NewLC(MMIDCustomItem* aCustomItem);

    ~CMMAItemDisplay();    // Destructor ()

protected:  // Constructors and destructors
    // Default constructor, protected to allow derivation
    CMMAItemDisplay(MMIDCustomItem* aCustomItem);

public:     // New methods
    /**
     * To be called when java display's size is changed.
     * This method also informs when context for the item is
     * created for the first time.
     */
    static void SizeChangedL(CMMAItemDisplay* aDisplay,
                             TInt aWidth,
                             TInt aHeight);

    /**
     * Sets source size to aSize.
     */
    static void StaticSourceSize(CMMAItemDisplay* aDisplay,
                                 TSize* aSize);

public: // Methods derived from MMMADisplay
    void SourceSizeChanged(const TSize& aSourceSize);
    void SetFullScreenL(TBool aFullScreen);
    void SetDisplayLocationL(const TPoint& aPosition);
    TPoint DisplayLocation();
    void SetWindowL(MMMADisplayWindow* aWindow);

public: // From MDirectContent
    void MdcContentBoundsChanged(const TRect& aRect);
    void MdcItemContentRectChanged(const TRect& aContentRect,
                                   const TRect& aScreenRect);
private:    // Data
    MMIDCustomItem* iItem;
};

#endif // CMMAITEMDISPLAY_H
