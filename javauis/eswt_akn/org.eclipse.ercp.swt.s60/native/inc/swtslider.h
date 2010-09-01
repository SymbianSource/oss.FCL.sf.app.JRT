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


#ifndef SWTSLIDER_H
#define SWTSLIDER_H


#include "swtcontrolbase.h"


class CAORotateImage;
class MTouchFeedback;


/**
 * CSwtSlider
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtSlider)
        : public CAknControl
        , public ASwtControlBase
        , public MSwtSlider
{
public:
    /**
    * The 1st and 2nd phase constructor wrapper.
    * @param aDisplay The Display to associate with.
    * @param aPeer The peer to its java counterpart.
    * @param aParent Parent composite.
    * @param aStyle The control's style.
    * @return CSwtSlider*  A pointer to the  created CSwtSlider object.
    */
    static CSwtSlider* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

private:
    /**
    * Contructor.
    * @param aDisplay The Display to associate with
    * @param aPeer The peer to its java counterpart
    * @param aParent Parent composite
    * @param aStyle The style
    */
    CSwtSlider(MSwtDisplay& aDisplay, TSwtPeer aPeer,
               MSwtComposite& aParent, TInt aStyle);

    /**
    * Destructor.
    */
    ~CSwtSlider();

    /**
    * Second phase constructor
    */
    void ConstructL();

private:
    /**
    * Adjust the current selection value with the maximun, minimun and iIncrement.
    */
    void AdjustSelectionValue();

    /**
    * Update the Slider image with the maximun, minimun and iIncrement.
    * Just call this method if one of this previous value changed,
    * because it reload the image, and compute the rotation if the
    * Slider have the VERTICAL Style and draw it.
    */
    void UpdateSliderImageL();

    /**
    * Clip the image with the rectangle of this Control and draw it.
    *
    * @param aGC    The graphic context where the image will be drawing.
    * @param aRect  The rectangle need to be redrawing to draw.
    *
    */
    void ClipAndDrawImage(CWindowGc& aGC, const TRect& aRect) const;

    /**
    * Creates the slider graphic. The graphic is recreated every time
    * the marker is moved. The input parameters are used to calculate
    * the position of the marker in the returned slider graphic.
    *
    * @param aValue     The current value of the slider.
    * @param aMinValue  The minimum value of the slider.
    * @param aMaxValue  The maximum value of the slider.
    * @return The slider graphic owned by the caller.
    */
    CGulIcon* CreateSliderIconL(TInt aValue, TInt aMinValue, TInt aMaxValue);

    /**
    * Determines the preferred size of the slider graphic for
    * the currently active screen resolution.
    *
    * @return The preferred size of the slider graphic.
    */
    TSize GetPreferredSliderSize() const;

    /**
     * Make sure the feedback area follows the control's bounds.
     */
    void UpdateTouchFeedbackRect() const;

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

#ifdef RD_SCALABLE_UI_V2
    TInt CalcAlignedValue(const TPoint& aPoint);
#endif // RD_SCALABLE_UI_V2

// From CCoeControl
public:
    TSize MinimumSize();
    void  MakeVisible(TBool aVisible);
    void  SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void  Draw(const TRect& aRect) const;
    void  HandleResourceChange(TInt aType);
protected:
    void  FocusChanged(TDrawNow aDrawNow);
    void  SizeChanged();
    void  PositionChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

#ifdef RD_SCALABLE_UI_V2

    void  HandlePointerEventL(const TPointerEvent& aPointerEvent);

#endif // RD_SCALABLE_UI_V2

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void  ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);

// From MSwtSlider
public:
    MSwtControl* Control();
    TInt GetMaximum() const;
    TInt GetMinimum() const;
    TInt GetPageIncrement() const;
    TInt GetIncrement() const;
    TInt GetSelection() const;
    void SetMaximum(TInt aValue);
    void SetMinimum(TInt aValue);
    void SetPageIncrement(TInt aValue);
    void SetIncrement(TInt aValue);
    void SetSelection(TInt aValue);
    void SetValues(TInt aSelection, TInt aMinimum, TInt aMaximum,
                   TInt aIncrement, TInt aPageIncrement);

// Data
private:
    /**
     *  The minimum value for the Slider.
     */
    TInt iMinimum;

    /**
     *  The maximun value for the Slider.
     */
    TInt iMaximum;

    /**
     *  The current selection on the Slider.
     */
    TInt iSelection;

    /**
     *  The increment value for the Slider.
     */
    TInt iIncrement;

    /**
     *  The page increment value for the Slider.
     */
    TInt iPageIncrement;

    /**
     *  Contains the key value which increase the selection.
     */
    TInt iIncreaseKey;

    /**
     *  Contains the key value which decrease the selection.
     */
    TInt iDecreaseKey;

    /**
     *  The image of the Slider.
     */
    CFbsBitmap* iSliderImage;

    /**
     *  The mask for image of the Slider.
     */
    CFbsBitmap* iSliderImageMask;

    /**
     *  The active object which rotate the VERTICAL Slider image.
     */
    CAORotateImage* iImagesRotator;

    /**
     *  The image Size in pixel.
     */
    TSize iImageSize;

    /**
     * Feedback instance.
     * Not own.
     */
    MTouchFeedback* iFeedback;
};


#endif // SWTSLIDER_H
