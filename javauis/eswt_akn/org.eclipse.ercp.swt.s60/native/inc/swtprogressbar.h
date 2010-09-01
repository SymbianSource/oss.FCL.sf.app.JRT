/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTPROGRESSBAR_H
#define SWTPROGRESSBAR_H


#include "swtcontrolbase.h"


class CAORotateImage;


/**
 * CSwtProgressBar
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtProgressBar)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtProgressBar
{
public:
    /**
     * The 1st and 2nd phase constructor wrapper.
     * @param aDisplay The Display to associate with.
     * @param aPeer The peer to its java counterpart.
     * @param aParent Parent composite.
     * @param aStyle The control's style.
     * @return CSwtProgressBar*  A pointer to the  created CSwtProgressBar object.
     */
    static CSwtProgressBar* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

private:
    /**
     * Contructor.
     * @param aDisplay The Display to associate with
     * @param aPeer The peer to its java counterpart
     * @param aParent Parent composite
     * @param aStyle The style
     */
    CSwtProgressBar(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                    MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor.
     */
    ~CSwtProgressBar();

    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    /**
     * Load all needed images
     */
    void LoadImagesL();

    /**
     * Delete all images.
     * This method delete all images contained by iImagesToDelete,
     * and reset iBarImages and iBarMasks arrays.
     */
    void DeleteImages();

    /**
     * Adjust the current selection value with the maximun, minimun values.
     */
    void UpdateSelection();

    /**
     * Drawing the background on the empty area, which are relative
     * with the current selection. This method is used by the SMOOTH styles.
     */
    void DrawEmptyArea(CWindowGc& aGc) const;

    /**
     * Add a image to the "rotator".
     * @param aBitmap The bitmap to rotate.
     */
    void AddBitmapToRotator(CFbsBitmap*& aBitmap);

    /**
     * Add all images to the "rotator".
     */
    void AddAllImagesToRotate();

    /**
     * Draw the progressBar images.
     * @param aGC    The graphic context where the image will be drawing.
     * @param aRect  The progress bar rectangle.
     */
    void DrawProgressBar(CWindowGc& gc, TRect targetRect) const;

    /**
     * Draw the progressBar frame.
     * @param aGC    The graphic context where the image will be drawing.
     * @param aRect  The progress bar rectangle.
     */
    void DrawFrame(CWindowGc& gc, const TRect& targetRect) const;

    /**
     * Get the prefered size of the ProgressBar from the layout.
     * @return The prefered size of the ProgressBar from the layout.
     */
    TSize LayoutPreferedSizeSize() const;

    void SetImagesSize(const TSize& aImageSize);

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

    /**
     * Returns ETrue when bitmap needs to be converted to plain bitmap.
     * @param aBitmap Bitmap to be tested.
     * @return ETrue when bitmap needs to be converted.
     */
    TBool IsBitmapConversionNeeded(CFbsBitmap* aBitmap) const;

    /**
     * Creates new bitmap which is neither compressed, nor extentended.
     * Ownership of the new bitmap is passed to the caller.
     * Source bitmap is not destroyed.
     * @param aSrcBitmap Bitmap to be converted
     * @return New un-compressed, non-extended bitmap.
     */
    CFbsBitmap* CreatePlainBitmapL(CFbsBitmap* aSrcBitmap) const;


// Some private methods related to the KSwtStyleIndeterminate mode
private:
    void StartAnimation();
    void StopAnimation();
    static TInt AnimationCallBack(TAny* aPtr);

// From CCoeControl
public:
    TSize           MinimumSize();
    void            HandleResourceChange(TInt aType);
    void            SetDimmed(TBool aDimmed);
    void            MakeVisible(TBool aVisible);
    TKeyResponse    OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
protected:
    void            FocusChanged(TDrawNow aDrawNow);
    void            SizeChanged();
    void            PositionChanged();
    void            Draw(const TRect& aRect) const;
    TTypeUid::Ptr   MopSupplyObject(TTypeUid aId);

// From MSwtControl
public:
    CCoeControl&        CoeControl();
    const CCoeControl&  CoeControl() const;
    TBool               IsFocusable(TInt aReason = KSwtFocusByApi) const;
    void                ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TSize               ComputeSizeL(TInt aWHint, TInt aHHint);

// From MSwtProgressBar
public:
    MSwtControl*    Control();
    TInt            GetMaximum() const;
    TInt            GetMinimum() const;
    TInt            GetSelection() const;
    void            SetMaximum(TInt aValue);
    void            SetMinimum(TInt aValue);
    void            SetSelection(TInt aValue);

// Data
private:
    /**
     *  The minimum value for the Slider.
     */
    TInt iMinimum;

    /**
     *  The maximum value for the ProgressBar.
     */
    TInt iMaximum;

    /**
     *  The current selection on the ProgressBar.
     */
    TInt iSelection;

    /**
     *  The active object which rotate the VERTICAL ProgressBar images.
     */
    CAORotateImage* iImagesRotator;

    /**
     *  Array which contains all images to delete.
     */
    RArray<CFbsBitmap*>     iImagesToDelete;

    TInt                    iImageIndex;            // Actual displayed frame of the animation.
    RArray<CFbsBitmap*>     iBarImages;             // Bitmaps for the waiting animation.
    RArray<CFbsBitmap*>     iBarMasks;              // Mask for the waiting animation.
    CFbsBitmap*             iBarFrameLeftImage;     // Bitmap of the progress bar frame image.
    CFbsBitmap*             iBarFrameLeftMask;      // Bitmap of the progress bar frame mask.
    CFbsBitmap*             iBarFrameRightImage;    // Bitmap of the progress bar frame image.
    CFbsBitmap*             iBarFrameRightMask;     // Bitmap of the progress bar frame mask.
    CFbsBitmap*             iBarFrameCenterImage;   // Bitmap of the progress bar frame image.
    CFbsBitmap*             iBarFrameCenterMask;    // Bitmap of the progress bar frame mask.
    CPeriodic*              iAnimationPeriodic;     // Asyncronous periodic caller for the animation.
    TInt                    iFrameIntervalInMilliSeconds;
};

#endif // SWTPROGRESSBAR_H



