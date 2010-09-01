/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDCUSTOMITEM_H
#define CMIDCUSTOMITEM_H

#include <gdi.h>
#include <lcdui.h>
#include <reflcdui.h>
//
#include "CMIDForm.h"
// CMIDControlItem inheritance in class declaration
#include "CMIDControlItem.h"
// interface class for receiving media key events
#include "MMIDMediaKeysListener.h"
// PointerEventSuppressor
#include <aknpointereventsuppressor.h>

#ifdef RD_TACTILE_FEEDBACK
#include "CMIDTactileFeedbackExtension.h"
#endif //RD_TACTILE_FEEDBACK


// FORWARD DECLARATIONS
class CMIDRemConObserver;
class CMIDKeyDecoder;
class CEikLabel;

NONSHARABLE_CLASS(CMIDCustomItem) : public CMIDControlItem, public MMIDCustomItem,
#ifndef RD_JAVA_NGA_ENABLED
        public MDirectScreenAccess,
#endif
        public MMIDBufferProcessor, public MDirectContainer, public MMIDMediaKeysListener
#ifdef RD_TACTILE_FEEDBACK
        , public MMIDTactileFeedbackComponent
#endif // RD_TACTILE_FEEDBACK
        , public MMIDLcduiEventConsumer
{
public:
    static CMIDCustomItem* NewL(MMIDEnv& aEnv, const TDesC& aLabel, CMIDUIManager* aUIManager);

    TBool           SupportsInternalTraversal() const;

    //
    // From MMIDCustomItem
    //
    TInt            InteractionModes() const;
    void            SetFocusAndScroll(TBool aFocus,const TRect* aScroll,TInt aDirection);

    void            SetContainerWindowL(const CCoeControl& aWindow);
    void            Invalidate(const TSize& aMinSize, const TSize& aPrefSize);
    const TSize&    PreferredContentSize() const;
    const TSize&    MinimumContentSize() const;

    CFbsBitmap*     FrameBuffer() const;
    MDirectContainer& DirectContainer();
    //
    // From MMIDItem
    //
    void  SetLabelL(const TDesC& aLabel);
    void  SetLayoutL(TLayout aLayout);

    void  SetPreferredSizeL(const TSize& aSize);
    TSize PreferredSize() const;

    TSize MinimumSize() const;

    void  AddCommandL(MMIDCommand* aCommand);
    void  RemoveCommand(MMIDCommand* aCommand);
    void  SetDefaultCommand(MMIDCommand* aCommand);

    //
    // From MMIDComponent
    //
    void Dispose();
    MMIDBufferProcessor* Processor();

    //
    // From MMIDBufferProcessor
    //
#ifdef RD_JAVA_NGA_ENABLED
    virtual TBool ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, java::util::Monitor* aMonitor);
#else
    virtual TBool ProcessL(
        const TMIDBufferOp*& aRead, const TMIDBufferOp* aEnd,
        TInt& aCycles, TRequestStatus* aStatus = NULL);
#endif
    void  AbortAsync();

    //
    // From CCoeControl
    //
    TSize MinimumSize();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void Draw(const TRect& aRect) const;
    void SizeChanged();
    void PositionChanged();
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent &aPointerEvent);
#endif //RD_SCALABLE_UI_V2
    void HandleCurrentL(TBool aCurrent);

public:
    TSize ResetPreferredSize() const;
    TSize CheckRequestedSize(const TSize& aRequestedSize) const;
    void FocusChanged(TDrawNow aDrawNow);
    void HandleVisibilityChange();
    // overrides CMIDItem::SetForm(CMIDForm* aForm)
    void SetForm(CMIDForm* aForm);

    TInt ItemPreferredHeightWithoutLabel();

public:
    // from CMIDItem
    void ItemAddedToFormL();
    void ItemRemovedFromForm();

    // from CMIDControlItem
    void PostFocusTransferEvent(TBool aFocus,CMIDForm::TDirection aDirection);

    void AdjustToSizeL(const TSize& aSize);

    void ResolutionChange(TInt aType);

    void HandleWindowFade(TBool aFaded);

private:
    /**
     * PointerEventSuppressor.
     * Own.
     */
    CAknPointerEventSuppressor* iPointerEventSuppressor;
    TPoint iPointerPressedCoordinates;
    TBool iPointerEventSuppressionOngoing;
    TInt iPESPointerMovementInTwips;
    TInt iPESTimeInMilliseconds;

    /**
     * Ctor
     * @param aEnv Environment instance
     * @param aUIManager UI Manager
     */
    CMIDCustomItem(MMIDEnv& aEnv, CMIDUIManager* aUIManager);
    ~CMIDCustomItem();
    void ConstructL(const TDesC& aLabel);

    CMIDForm::TDirection TranslateKey(const TKeyEvent& aEvent);

    void ResetBuffers();
    void CreateBuffersL();

    void UpdateMargins();

#ifndef RD_JAVA_NGA_ENABLED
    /**
     * Pauses the direct screen access.
     * Sets iDirectPaused flag to true.
     */
    void PauseDirectAccess();

    /**
     * Sets iDirectPaused flag to true.
     * Resumes the direct screen access using StartDirectAccess().
     */
    void ResumeDirectAccess();

    /**
     * Stops direct screen access.
     */
    void StopDirectAccess();

    void StartDirectAccess();
#endif

    /**
     * Checks if there are hw keyboards available in the device and returns the
     * interaction modes based on keyboard availability.
     *
     * @return  TInt containing the supported keyboard interaction modes.
     * @since S60 5.0
     */
    TInt KeyboardInteractionModesL() const;


    /**
     * Notifies the current displayable
     * about the direct content position update
     *
     * @since S60 5.0
     * @return  ETrue if the update was done
                EFalse if the update could be done
     */
    TBool UpdateDirectContentBounds();

#ifndef RD_JAVA_NGA_ENABLED
public:
// From MDirectScreenAccess
    virtual void AbortNow(RDirectScreenAccess::TTerminationReasons aReasons);
    virtual void Restart(RDirectScreenAccess::TTerminationReasons aReasons);
#endif

// From MDirectContainer
public:
    void MdcAddContent(MDirectContent* aContent);
    void MdcRemoveContent(MDirectContent* aContent);
    TRect MdcContainerWindowRect() const;
    TBool MdcContainerVisibility() const;
    TRect MdcContentBounds() const;
    void MdcItemContentRect(TRect& aContentRect, TRect& aScreenRect) const;
    void MdcHandlePointerEventL(TPointerEvent& aPointerEvent);
    void MdcFlushContainer(const TRect& aRect);
    void MdcAddContentBounds(const TRect& aRect);
    void MdcRemoveContentBounds(const TRect& aRect);
    void MdcGetDSAResources(MUiEventConsumer& aConsumer);
    void MdcGetUICallback(MUiEventConsumer& aConsumer, TInt aCallbackId);
    
    /**
     * From MDirectContainer.     
     *
     * @since  S60 v9.2
     * @param TBool aEnableFix
     */    
    void MdcFixUIOrientation(TBool aEnableFix);
     
// From MMIDLcduiEventConsumer
public: 
    void HandleLcduiEvent(int aType);

// From MMIDMediaKeysListener
public:
    /**
     * This method is called when a media key has been pressed.
     * Posts the key event to Java layer.
     *
     * @param TMIDKeyEvent      Media key event that has occured.
     * @since S60 3.2
     */
    void HandleMediaKeyEvent(TMIDKeyEvent& aKeyEvent);

    void ChangeDirectContainerVisibility(TBool aVisible);

#ifdef RD_TACTILE_FEEDBACK

    CMIDTactileFeedbackExtension* iTactileFeedback;

public:
    MMIDTactileFeedbackComponent* TactileFeedbackComponent();

    void UpdateTactileFeedback();

    void RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle);
    void UnregisterFeedbackArea(TInt aId);
    void UnregisterFeedbackForControl();
    void MoveAreaToFirstPriority(TInt aId);
#endif


private:
    /** The content buffer, used by the
    Graphics object associated to this custom item */
    CFbsBitmap* iFrameBuffer;
    CFbsBitGc* iFrameContext;
    CFbsBitmapDevice* iFrameDevice;

    /** The offscreen buffer, implements double buffering
    to assure content is kept between drawn. This allows
    having skin underneath the content if nothing is drawn on it.
    */
    CFbsBitmap* iOffScreenBuffer;
    CFbsBitGc* iOffScreenContext;
    CFbsBitmapDevice* iOffScreenDevice;

    TSize       iMinimumContentSize; // This doesn't include margin
    TSize       iPreferredContentSize;  // This doesn't include margin
    TRect       iContentRect;
    TBool       iEntered;
    TBool       iSupportsInternalTraversal;
    MMIDEnv&    iEnv;

    /**
     * Indicates that the iFrameBuffer may be reset (with background skin).
     * It prevents the situation where iFrameBuffer is reset
     * before it has been copied to iOffScreenBuffer.
     */
    TBool iResetFrameBuffer;

    /**
     * Item can only have one DirectContent.
     * Content is null if content is not added
     * with MdcAddContent method.
     */
    MDirectContent* iDirectContent;
    TBool iDirectAreaAddedToDisplayable;
    TRect iDirectArea;

    TMargins iContentMargins; // Margins for the content in relation to controlItem area
    CMIDRemConObserver* iRemConObserver;
    CMIDKeyDecoder* iKeyDecoder;
    // Nokia-MIDlet-S60-Selection-Key-Compatibility
    TBool iS60SelectionKeyCompatibility;

#ifndef RD_JAVA_NGA_ENABLED
    CDirectScreenAccess* iDirectAccess;

    /**
     * Indicates that direct screen access is paused
     * using PauseDirectAccess().
     */
    TBool iDirectPaused;
#endif

    TBool iRestoreDirectContentWhenUnfaded;

    // Stores information if CustomItem changed it visibility
    // from invisible (e.g. outside Form, to background) to visible and vice versa
    TBool iPreviousVisibility;

    /**
     * A consumer which is waiting for the callback.
     * Set in case that Container Window have not been set
     * before MdcGetDSAResources call.
     * Not owned.
     */
    MUiEventConsumer* iConsumerWaitingForDSAResourcesCallback;

    /**
     * Stores the last window position as returned
     * from DrawableWindow()->Position() to be useful
     * from another than MMAPI ES thread.
     */
    TPoint iLastWindowPosition;
 
    /**
     * using for Items on Form
     * checking if Form was created
     */    
    TBool iUiFixed;
    TBool iUiToBeFixedLater;

};

#endif // CMIDCUSTOMITEM_H



