/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDGAUGEITEM_H
#define CMIDGAUGEITEM_H

// CMIDControlItem inheritance in class declaration
// using TAknLayoutRect (indirect from CMIDForm.h)
#include <lcdui.h>
#include "CMIDControlItem.h"

/**
 *
 * This class uses a CTimer which could potentially keep the machine alive. One way round
 * this is to use the MMIDEnvObserver to receive switch on/foreground events and so suspend
 * the timer when in the backgound or when the user has turned the device off.
 *
 * This is not done in this reference API because an alternative view would be to place the
 * responsibility on the MIDlet.
 *
 */

class CEikProgressInfo;
class CAknSlider;
class CAknBitmapAnimation;

/** This interface is implemented by the alert dialog (CMIDAlertDialog). The current
    design is such that the alert dialog uses its own animations (loaded from resource)
    when a gauge is added to an alert. So when the gauge java peer is updated (for example
    because its value is changed) we need to notify the alert, which will then have to
    update its animations or bitmaps.*/
class MMIDGaugeToAlertListner
{
public:
    /** For definite gauge items or if the gauge indefinite state does not change
    we just need to tell the dialog to incremenat the frame or the anim status */
    virtual void UpdateGaugeInAlertL(TInt aValue)=0;

    /**
    * Function is called when gauge type was changed.
    */
    virtual void GaugeTypeInAlertChangedL()=0;
};

NONSHARABLE_CLASS(CMIDGaugeItem) : public CMIDControlItem, public MMIDGauge
{
public:
    NONSHARABLE_CLASS(CGaugeTimer) : public CTimer
    {
public:
        static CGaugeTimer* NewL(CMIDGaugeItem& aGauge);
private:
        CGaugeTimer(CMIDGaugeItem& aGauge);
        void RunL();
private:
        CMIDGaugeItem& iGauge;
    };
public:
    virtual ~CMIDGaugeItem();
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    //
    // From MMIDGauge
    //
    virtual void SetValueL(TInt aValue) = 0;
    virtual TInt GetValue();
    virtual void SetMaxValueL(TInt aValue) = 0;
    //
    // From MMIDItem
    //
    virtual void SetLabelL(const TDesC& aLabel);
    void SetLayoutL(TLayout aLayout);
    void SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;
    TSize MinimumSize() const;
    void AddCommandL(MMIDCommand* aCommand);
    void RemoveCommand(MMIDCommand* aCommand);
    void SetDefaultCommand(MMIDCommand* aCommand);

    TBool IsSelectable() const;
    TInt GetIndefiniteState() const;
    //
    // From MMIDComponent
    //
    void Dispose();
    virtual TType Type() const
    {
        return EGauge;
    }

    //
    // From CMIDControlItem
    //
    void ColorChange(TInt aType);

    /**
    * Handles event when application switches to/from the foreground.
    *
    * @param aForeground ETrue to switch to the foreground. EFalse to switch
    *   from the foreground.
    * @note Animation of non-interactive gauges is started/stopped.
    */
    virtual void HandleForegroundL(TBool aForeground);

protected:
    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDGaugeItem(CMIDUIManager* aUIManager);

    void BaseConstructL(const TDesC& aLabel,TInt aMaxValue,
                        TInt aInitialValue);

    void FocusChanged(TDrawNow aDrawNow);

    void CreateBitmapsIfNeededL();

    /**
    * Creates bitmaps in thread-local bitmap storage.
    *
    * Thread-local bitmap storage has to be already created else has no effect.
    */
    void CreateBitmapsInTlsL();

    /**
    * Deletes bitmaps from thread-local bitmap storage.
    *
    * Thread-local bitmap storage has to be already created else has no effect.
    * Fuction does not delete thread-local bitmap storage.
    */
    void DeleteBitmapsInTls();

    /**
    * Resizes bitmaps in thread-local bitmap storage.
    *
    * Thread-local bitmap storage and bitmaps have to be already created else
    * has no effect.
    */
    void ResizeBitmapsInTlsL();

protected:
    TBool iIndefinite;
    TInt iIndefiniteState;
    TInt iMaxValue;
    TInt iValue;

    /** Bitmaps are stored in the TLS so they can be shared across gauge items*/
    struct TGaugeFrameData
    {
        TInt iNumInstances;
        RArray<CFbsBitmap*> iContRunBitmap; // continuous running bitmaps
        RArray<CFbsBitmap*> iContRunBitmapMask;
        RArray<CFbsBitmap*> iIncUpdBitmap; // incremental updating bitmaps
        RArray<CFbsBitmap*> iIncUpdBitmapMask;
        CFbsBitmap* iContIdleBitmap; // continuous idle bitmap
        CFbsBitmap* iContIdleBitmapMask;
        CFbsBitmap* iIncIdleBitmap; // incremental idle bitmap
        CFbsBitmap* iIncIdleBitmapMask;
        CFbsBitmap* iFrameL; // frame bitmaps
        CFbsBitmap* iFrameLMask;
        CFbsBitmap* iFrameCenter;
        CFbsBitmap* iFrameCenterMask;
        CFbsBitmap* iFrameR;
        CFbsBitmap* iFrameRMask;

        /**
         *
         *
         */
        void Clean()
        {
            iNumInstances = 1;
            iContRunBitmap.Reset();
            iContRunBitmapMask.Reset();
            iIncUpdBitmap.Reset();
            iIncUpdBitmapMask.Reset();
            iContIdleBitmap = NULL;
            iContIdleBitmapMask = NULL;
            iIncIdleBitmap = NULL;
            iIncIdleBitmapMask = NULL;
            iFrameL = NULL;
            iFrameLMask = NULL;
            iFrameCenter = NULL;
            iFrameCenterMask = NULL;
            iFrameR = NULL;
            iFrameRMask = NULL;
        }

        /**
         *
         *
         */
        void Close()
        {
            iContRunBitmap.Close();
            iContRunBitmapMask.Close();
            iIncUpdBitmap.Close();
            iIncUpdBitmapMask.Close();
        }
    };

    /** An array of SVG bitmaps stored in the TLS so that it can be shared amongst gauge items*/
    TGaugeFrameData* iGaugeFrameData;

    friend class CGaugeTimer;
};



NONSHARABLE_CLASS(CMIDNonInteractiveGauge) : public CMIDGaugeItem
{
public:
    ~CMIDNonInteractiveGauge();
    /**
     * Double ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    static MMIDGauge* NewL(
        const TDesC& aLabel,TInt aMaxValue,TInt aInitialValue, CMIDUIManager* aUIManager);

    //
    // From MMIDGauge
    //
    virtual void SetValueL(TInt aValue);
    virtual void SetMaxValueL(TInt aValue);
    virtual TInt GetMaxValue();

    // From CCoeControl
    virtual TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    void SetContainerWindowL(const CCoeControl& aContainer);
    void DoSafeDraw();

#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2

    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    //
    // From CMIDControlItem
    //
    /**
    * Handles event when application switches to/from the foreground.
    *
    * @param aForeground ETrue to switch to the foreground. EFalse to switch
    *   from the foreground.
    * @note Animation of non-interactive gauges is started/stopped.
    */
    void HandleForegroundL(TBool aForeground);
    void ResolutionChange(TInt aType);

    /**
    * Handles addition of ni-gauge to form.
    */
    void ItemAddedToFormL();

    /**
    * Handles removal of ni-gauge from form.
    */
    void ItemRemovedFromForm();

    /**
    * Function is called when skin or color scheme change occurs.
    *
    * @param aType Type of change.
    */
    void ColorChange(TInt aType);

    void SetGaugeListenerFromAlert(MMIDGaugeToAlertListner* aGaugeToAlertListner);

private:
    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDNonInteractiveGauge(CMIDUIManager* aUIManager);

    /**
     * second pfase constructor
     */
    void ConstructL();
    void ConstructProgressInfoL();
    friend class CMIDAlertDialog;
    void UpdateProgressInfo(CEikProgressInfo* aProgressInfo) const;
    void UpdateMemberVariables();

    /**
     * Creats new instance of CAknBitmapAnimation for Gauge animation
     * and sets it to iBitmapAnimation
     */
    void CreateNewBitmapAnimationIfNeededL();

    /**
     * setting of size of iBitmapAnimation
     */
    void SetAnimationSize();

    /**
     * End of construction of aBitmapAnimation and its setting up.
     */
    void ConstructAnimation(CAknBitmapAnimation* aBitmapAnimation);
    /**
     * getting of correct TAknsItemID for current Gauge
     * @return correct TAknsItemID
     */
    TAknsItemID GetAknsItemID();

    void ColorChangeL(TInt aType);

    void DisposeTimer();

    /**
     * From CMIDGaugeItem.
     * Starts animation timer (see iTimer) when needed.
     * In case that timer is not needed, but has been created,
     * it's disposed by this method.
     *
     * @since  S60 v5.0
     */
    void InstallGaugeTimerWhenNeededL();

    /**
     * Indicates that the current skin contains the animation and
     * the animation has been instantiated to iBitmapAnimation.
     *
     * @return ETrue iBitmapAnimation has been created
     *               and the animation is useful.
     * @since  S60 v5.0
     */
    TBool BitmapAnimationUsed() const;

private:
    CEikProgressInfo* iProgressInfo;
    MMIDGaugeToAlertListner* iGaugeToAlertListner;
    TAknLayoutRect iWaitGaugeRect; // form2_midp_gauge_wait_pane
    TAknLayoutRect iWaitBarRect; // form2_midp_wait_pane
    TSize iWaitBarSize; // Size of form2_midp_wait_pane
    TInt iItemheightWithoutLabel; // Height of the GaugeItem without label

    // WaitBar position in relation to CCoeControl (CMIDControlItem Rect) area
    // Note that label height has to be added to the y coordinate
    TPoint iProgressInfoWithLabelHeightTl; // Top left point of the progress info
    CAknBitmapAnimation* iBitmapAnimation; // Animation for animated Gauge
    CGaugeTimer* iTimer;
    // Indicates that the application is in foreground
    TBool iIsInForeground;
};

NONSHARABLE_CLASS(CMIDInteractiveGauge) : public CMIDGaugeItem
{
public:
    ~CMIDInteractiveGauge();
    /**
     * Double ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    static MMIDGauge* NewL(
        const TDesC& aLabel,TInt aMaxValue,TInt aInitialValue, CMIDUIManager* aUIManager);

    //
    // From MMIDGauge
    //
    virtual void SetValueL(TInt aValue);
    virtual void SetMaxValueL(TInt aValue);

    // From CCoeControl
    virtual TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    void PositionChanged();
    void SetContainerWindowL(const CCoeControl& aContainer);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    void DoSafeDraw();

    //
    // From CMIDItem
    //
    TInt ItemPreferredHeightWithoutLabel();
    //
    // From CMIDControlItem
    //
    void ResolutionChange(TInt aType);

private:

    /**
     * Ctor
     * @param aUIManager UI manager
     *
     * @since s60
     */
    CMIDInteractiveGauge(CMIDUIManager* aUIManager);

    void ConstructL();
    void ConstructSliderL();
    void UpdateMemberVariables();

private:
    CAknSlider* iSlider;
    TAknLayoutRect iSliderGaugeRect; // form2_midp_gauge_slider_pane
    TInt iItemheightWithoutLabel; // Height of the GaugeItem without label
    // Slider position in relation to CCoeControl (CMIDControlItem Rect) area
    TPoint iSliderPositionTl; // Top left point of the slider control
    TInt iOriginalSliderValue;
    TBool iPhysicsScrollingTriggered;
};


#endif // CMIDGAUGEITEM_H

// End of File
