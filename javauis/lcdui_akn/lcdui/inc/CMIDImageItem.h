/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDIMAGEITEM_H
#define CMIDIMAGEITEM_H

#include <gdi.h>
//
#include <lcdui.h>
// using bitmaps (MMIDBitmapImage) and API for them
#include <lcdgr.h>
// CMIDControlItem inheritance in class declaration
// and some API

#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif


#include "CMIDControlItem.h"

class CEikLabel;
class CAknButton;
class CGulIcon;

// wrapper for embedding image in CMIDForm

NONSHARABLE_CLASS(CMIDImageItem) : public CMIDControlItem, public MMIDImageItem
{
public:
    /**
     * Double ctor
     * @return CMIDImageItem instance
     *
     * @since s60
     */
    static CMIDImageItem* NewL(
        const TDesC& aLabel, MMIDImage* aImage, TLayout aLayout,
        TAppearance aAppearance, const TDesC& aAltText, CMIDUIManager* aUIManager);

    //
    // From MMIDImageItem
    //
    void SetAltTextL(const TDesC& aAltText);
    void SetImageL(MMIDImage* aImage);
    //
    // From MMIDItem
    //
    void SetLabelL(const TDesC& aLabel);
    inline void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);
    TBool IsSelectable() const;
    //
    // From MMIDComponent
    //
    void Dispose();
    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt /*aIndex*/) const;
    void Draw(const TRect& /*aRect*/) const;
    void SizeChanged();
    void FocusChanged(TDrawNow aDrawNow);
    void SetContainerWindowL(const CCoeControl& aContainer);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
    /**
     * Returns area that is sensitive to tapping actions
     */
    TRect TappingActionRect();
#endif // RD_SCALABLE_UI_V2

#ifdef RD_TACTILE_FEEDBACK
private:
    MTouchFeedback* iFeedback;
#endif

    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    //
    // From CMIDControlItem
    //
    TSize ResetPreferredSize() const;

    void ResolutionChange(TInt aType);

    //
    ~CMIDImageItem();

private:
    /**
     * Ctor
     *
     * @since s60
     */
    CMIDImageItem(
        TLayout aLayout, TAppearance aAppearance, CMIDUIManager* aUIManager);

    void ConstructL(const TDesC& aLabel,MMIDImage* aBitmap,const TDesC& aAltText);
    void UpdatePreferredSizeL();
    MMIDItem::TAppearance RenderedAppearance() const;

    void  AdjustToSizeL(const TSize& aSize);

    void DrawHyperlinkIndication(const TRect& parentRect) const;

    void UpdateMarginsAndImageSize(); // updates iImageMargins, iImageSize
    // and iItemHeightWithoutLabel
    void UpdateImageSize(); // updates iImageSize and iItemHeightWithoutLabel

    /**
    * Updates possition of image button according to it's size.
    * Should be called every time when variables affecting positioning are set.
    */
    void UpdateButtonPosition();

private:
    TAppearance iAppearance;
    MMIDBitmapImage*  iBitmapImage;

    TInt iItemHeightWithoutLabel; // imageitem height, includes margins
    // Image position in relation to CCoeControl (CMIDControlItem Rect) area
    TMargins iImageMargins; // Margins for the image in relation to controlItem area
    TSize iImageSize; // Size of the image

    //Hyperlink frame bitmaps
    CFbsBitmap* iHyperlinkVerticalLine;
    CFbsBitmap* iHyperlinkVerticalLineMask;
    CFbsBitmap* iHyperlinkHorizontalLine;
    CFbsBitmap* iHyperlinkHorizontalLineMask;
    // for button appearance
    CAknButton* iButton;
    CGulIcon* iButtonIcon; // iButton owns the instance
#ifdef RD_SCALABLE_UI_V2
    TBool iPointerDownOnContentArea; // ETrue, if EButton1Down happened on item content area
#endif // RD_SCALABLE_UI_V2
};


#endif // CMIDIMAGEITEM_H



