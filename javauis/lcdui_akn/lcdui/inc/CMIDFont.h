/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


// Native peer for Font objects

#ifndef CMIDFONT_H
#define CMIDFONT_H

#include <coecntrl.h>
#include <gulfont.h>
//
#include "lcdui.h"

class CMIDUIManager;

NONSHARABLE_CLASS(CMIDFont) : public CBase, public MMIDFont
{
public:
    /** IDs used for returning default fonts */
    enum TDefaultId
    {
        EDefaultTextId = 0,
        EDefaultLabelId
    };

public:
    static MMIDFont* NewL(TUint aStyle, TUint aSize, TInt aFace,
                          CMIDUIManager* aUIManager, TBool aIsFreeSizeFont);

    //
    // From MMIDFont
    //
    CFont* Font(TBool aHighLevelComponent = EFalse);
    TInt Height();
    TInt Baseline();
    TInt Width(const TDesC& aString);
    TBool IsUnderlined() const;
    void CheckScaling();
    TBool IsBold();
    TBool IsItalic();
    //
    // From MMIDComponent
    //
    void Dispose();
    //
    static const CFont* DefaultFont(const TDefaultId& aId = EDefaultTextId);
    static const CFont* DefaultLabelFont();

private:
    /**
     * Ctor
     * @param aUIManager UI manager
     * @return MMIDFont instance
     *
     * @since s60
     */
    CMIDFont(CMIDUIManager* aUIManager);
    ~CMIDFont();
    void ConstructL(TUint aStyle, TUint aSize, TInt /*aFace*/,
                    TBool aIsFreeSizeFont);
    void CreateNewFont();
    TInt GetTextPaneHeight(TBool aScaling);

    /**
     * When grahphic scaling is in use, this method calculates the font height
     * on the original MIDlet resolution.
     */
    void SetOrigFontHeight();
    /**
     * This method update iScalingFactor.
     */
    void ScalingFactorUpdate();
private:
    TBool   iBold; // bold style
    TBool   iItalic; // italic style
    TUint   iSize; // ESmall, EMedium, ELarge from the MMIDFont
    TBool   iUnderlined;
    CFont*  iFont;
    TInt    iPaneHeight; // Needed when the font is created. Also used to track dynamic resolution change.
    TInt    iOrigFontHeight; // When scaling is used, this is the font height in the original MIDlet resolution
    CMIDUIManager* iUIManager;
    TBool iIsFreeSizeFont; // flag used in case of font with custom height
    // ration between current size of screen original size of MIDlet
    TReal   iScalingFactor;
};

#endif // CMIDFONT_H
