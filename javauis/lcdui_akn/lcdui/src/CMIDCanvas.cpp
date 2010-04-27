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
* Description:  CMIDCanvas - Implements the native part of the Canvas class.
*
*/

// Using auto_ptr
#include <memory>

// API used for getting the layout for the network indicator bitmap and for
// retrieving info if pen is enabled (in CMIDCanvas::HandlePointerEventL
// function).
#include <AknUtils.h>

// Using Layout_Meta_Data::IsLandscapeOrientation()
#include <layoutmetadata.cdl.h>


// Using CRepository class used in CMIDCanvas::IsNetworkIndicatorEnabledL
// function.
#include <centralrepository.h>
#ifdef RD_JAVA_NGA_ENABLED
#include <logger.h>
#include <e32cmn.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#endif // RD_JAVA_NGA_ENABLED


// Using debug macros.
#include <j2me/jdebug.h>

// OMJ storage
#include <securitystoragedatadefs.h>
#include <javastorageentry.h>
#include <javastorage.h>
#include <javastoragenames.h>
#include <javauid.h>
#include <javasymbianoslayer.h>

#include "javaoslayer.h"

// Using constants for S60 LCDUI Plugin internal keys.
#include "S60LCDUIInternalCRKeys.h"

#include "CMIDCanvas.h"

// Using CMIDUIManager for iKeyDecoder and iScaler initialization.
#include "CMIDUIManager.h"

// Using CMIDScaler as member iScaler.
#include "CMIDScaler.h"

// Using CMIDKeyDecoder as member iKeyDecoder.
#include "CMIDKeyDecoder.h"

// Using CMIDNetworkIndicator as member iNetworkIndicator.
#include "CMIDNetworkIndicator.h"

// Using CMIDCallIndicator as member iCallIndicator.
#include "CMIDCallIndicator.h"

// Using CMIDRemConObserver as memeber iRemConObserver.
#include "CMIDRemConObserver.h"


#ifdef RD_TACTILE_FEEDBACK
// Using CMIDTactileFeedbackExtension as member iTactileFeedback.
#include "CMIDTactileFeedbackExtension.h"
#endif // RD_TACTILE_FEEDBACK

#include "MMIDCustomComponent.h"

// Using CoreUiAvkonLcdui for the splash screen
#include "coreuiavkonlcdui.h"

#ifdef DEFER_BITBLT
const TInt KEstCyclesPerPixel = 32;
#endif // DEFER_BITBLT

const TInt KAllowedTimeBetweenDragEvents = 50000; // 50ms

// Used with component controls
const TInt KComponentMainControl = 0;
const TInt KComponentFocusedNone = -1;

// Using enumeration TFocusState.
#include "MMIDTextEditor.h"

#ifdef RD_JAVA_NGA_ENABLED
// Max dimension for openGL texture, must be power of 2
const TInt KMaxBlitSize = 256;
const TInt KTexturesCount = 1;
const TInt KBytesPerPixel = 4;
const TInt KPhoneScreen = 0;
// The color that is used to clear the framebuffer when
// mixing 3D & 2D rendering
#define KTransparentClearColor TRgb(0x000000, 0x0)
#define KOpaqueClearColor      TRgb(0xFFFFFF, 0xFF)

#ifdef _DEBUG
#define ASSERT_GL() AssertGL()
#else
#define ASSERT_GL()
#endif

#endif // RD_JAVA_NGA_ENABLED

#define KZeroSize TSize()


// ---------------------------------------------------------------------------
// TBitBltData
// BitBlt buffer data datatype.
// ---------------------------------------------------------------------------
//
struct TBitBltData
{
    TRect iRect;
};


// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// BitBltData
// Get BitBlt buffer data from buffered operation passed to buffer processor.
// Used in CMIDCanvas::ProcessL function.
// ---------------------------------------------------------------------------
//
inline const TBitBltData& BitBltData(const TMIDBufferOp* aOp)
{
    ASSERT(aOp->Size() >= TInt(1 + (
                                   sizeof(TBitBltData) / sizeof(TMIDBufferOp))));

    return *static_cast< const TBitBltData* >(aOp->Data());
}


// ---------------------------------------------------------------------------
// IsDownScaling
// Check if downscaling in use.
// @param aSourceSize Source rect size.
// @param aDestRect Destination rect.
// ---------------------------------------------------------------------------
//
inline TBool IsDownScaling(const TSize& aSourceSize, const TRect& aDestRect)
{
    return (aSourceSize.iWidth > aDestRect.Width() ||
            aSourceSize.iHeight > aDestRect.Height());
}


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMIDCanvas::NewL
// Two-phased constructor. Use this method to create a CMIDCanvas instance.
// ---------------------------------------------------------------------------
//
CMIDCanvas* CMIDCanvas::NewL(
    MMIDEnv& aEnv,
    CCoeControl& aWindow,
    MMIDComponent::TType aComponentType,
    TBool aUpdateRequired)
{
    CMIDCanvas* self = new(ELeave) CMIDCanvas(aEnv,
            aComponentType,
            aUpdateRequired);

    CleanupStack::PushL(self);
    self->ConstructL(aWindow);
    CleanupStack::Pop(self);

    return self;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::~CMIDCanvas
// Destructor.
// ---------------------------------------------------------------------------
//
CMIDCanvas::~CMIDCanvas()
{
    DEBUG("+ CMIDCanvas::~CMIDCanvas");

    // Notify custom components that this container is disposing.
    TInt customCount = iCustomComponents.Count();

    for (TInt i = 0; i < customCount; i++)
    {
        iCustomComponents[i]->CustomComponentContainerDisposing();
    }

    // Close the array. Objects are not owned.
    iCustomComponents.Close();

    // Inform all listeners
    TInt contentsCount(iDirectContents.Count());

    for (TInt j = 0; j < contentsCount; j++)
    {
        iDirectContents[j]->MdcContainerDestroyed();
    }

    iDirectContents.Reset();

    // Unregister the component from the observer
    // if it were previously registered from MdcAddContent.
    iEnv.ToLcduiObserver().UnregisterControl(*this);

#ifdef CANVAS_DIRECT_ACCESS
    StopDirectAccess();
    delete iDirectAccess;
#endif // CANVAS_DIRECT_ACCESS

    // Network indicator
    if (iNetworkIndicator)
    {
        iNetworkIndicator->Close(); // Use cancel here.
        delete iNetworkIndicator;
    }

    // Call indicator
    if (iCallIndicator)
    {
        iCallIndicator->Cancel();
        delete iCallIndicator;
    }

    // Canvas removes itself from the CMIDRemConObserver.
    ASSERT(iKeyDecoder);

    if (iRemConObserver && iKeyDecoder->MediaKeysEnabled())
    {
        iRemConObserver->RemoveMediaKeysListener(this);
    }

    if (iDisplayable)
    {
        if (iScaler)
        {
            iDisplayable->GetUIManager()->CloseScaler(iScaler);
        }

        // Displayable is notified about content control deletion.
        iDisplayable->NotifyContentDestroyed();
    }

#ifdef CANVAS_DOUBLE_BUFFER
    if (iFrameBuffer)
    {
        iEnv.ReleaseCanvasFrameBuffer((*this), iFrameBuffer);
        iFrameBuffer = NULL;
    }

    delete iFrameContext;
    iFrameContext = NULL;
    delete iFrameDevice;
    iFrameDevice = NULL;
#endif // CANVAS_DOUBLE_BUFFER

#ifdef RD_TACTILE_FEEDBACK
    delete iTactileFeedback;
#endif // RD_TACTILE_FEEDBACK

    if (iPointerEventSuppressor)
    {
        delete iPointerEventSuppressor;
        iPointerEventSuppressor = NULL;
    }

#ifdef RD_JAVA_NGA_ENABLED
    CloseEgl();
    DisposePixelSource();
    delete[] iTexturePixels;
    delete[] iVertexArray;
    delete[] iTriangleStrip;
#endif // RD_JAVA_NGA_ENABLED
    DEBUG("- CMIDCanvas::~CMIDCanvas");
}


// ---------------------------------------------------------------------------
// CMIDCanvas::FullScreen
// Set fullscreen mode on/off.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::FullScreen(TBool aFullScreen)
{
    DEBUG_INT("CMIDCanvas::FullScreen(%d) ++--", aFullScreen);
    if (iFullScreen != aFullScreen)
    {
        iFullScreen = aFullScreen;
        // We have to inform all components on this Canvas.
        if (iCustomComponents.Count() > 0)
        {
            for (int i = 0; i < iCustomComponents.Count(); i++)
            {
                iCustomComponents[i]->HandleFullscreenModeChange();
            }
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDCanvas::IsScalingOn
// Tells whether scaling is on or off.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::IsScalingOn()
{
    return iScalingOn;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::SelectionKeyCompatibility
// Tells whether application attribute
// Nokia-MIDlet-S60-Selection-Key-Compatibility is set true or not.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::SelectionKeyCompatibility()
{
    return iS60SelectionKeyCompatibility;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::CountComponentControls
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDCanvas::CountComponentControls() const
{
    return iCustomComponentsControlCount;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::ComponentControl
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
CCoeControl* CMIDCanvas::ComponentControl(TInt aIndex) const
{
    TInt index = 0;

    TInt count = iCustomComponents.Count();

    for (int i = 0; i < count; i++)
    {
        TInt compoundCount =
            iCustomComponents[i]->CustomComponentControlCount();

        for (int j = 0; j < compoundCount; j++)
        {
            if (index == aIndex)
            {
                return iCustomComponents[i]->CustomComponentControl(j);
            }

            index++;
        }
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::SetFocusedComponent
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SetFocusedComponent(MMIDCustomComponent* aComponent)
{
    if (aComponent == NULL)
    {
        iFocusedComponent = KComponentFocusedNone;
        iPressedComponent = NULL;
    }
    else
    {
        TInt count = iCustomComponents.Count();
        TInt index = iCustomComponents.Find(aComponent);
        if ((index != KErrNotFound) && (iFocusedComponent != index))
        {
            iFocusedComponent = index;
        }
    }
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::OfferKeyEventL
// Process a key event. We ask the java environment to translate the key event
// and we send it java side only if the translation allows it. The translation
// is controlled by CMIDKeyDecoder via its interface to the java environment.
// CBA keys are special. We only try and send them java side if we are full
// screen and have no commands nor CommandListener, as stated in the specs.
// Otherwise - if we are full screen with at least one command and
// CommandListener we display the options menu or if we are not full screen
// we simply return so that the CBA will process the key event and display the
// options menu if needed. Note that in this context MSK is considered to be a
// softkey, it must not be delivered as a key event to the Java side.
// ---------------------------------------------------------------------------
//
TKeyResponse CMIDCanvas::OfferKeyEventL(
    const TKeyEvent& aEvent,
    TEventCode aType)
{
    DEBUG("+ CMIDCanvas::OfferKeyEventL");

    DEBUG_INT(
        "+ CMIDCanvas::OfferKeyEventL - received TKeyEvent: iCode = %d",
        aEvent.iCode);

    DEBUG_INT(
        "+ CMIDCanvas::OfferKeyEventL - received TKeyEvent: iModifiers = %d",
        aEvent.iModifiers);

    DEBUG_INT(
        "+ CMIDCanvas::OfferKeyEventL - received TKeyEvent: iRepeats = %d",
        aEvent.iRepeats);

    DEBUG_INT(
        "+ CMIDCanvas::OfferKeyEventL - received TKeyEvent: iScanCode = %d",
        aEvent.iScanCode);

    TKeyResponse response = EKeyWasNotConsumed;

    if ((iFocusedComponent != KComponentFocusedNone) &&
            (iFocusedComponent < iCustomComponents.Count()) &&
            (iCustomComponents[iFocusedComponent]->
             CustomComponentControl(KComponentMainControl)->IsVisible()))
    {
        // Traversal check
        if ((aType == EEventKeyDown) &&
                (((aEvent.iCode == EKeyUpArrow) ||
                  (aEvent.iCode == EKeyDownArrow)) ||
                 ((aEvent.iScanCode == EStdKeyUpArrow) ||
                  (aEvent.iScanCode == EStdKeyDownArrow))))
        {
            // Traversing is not allowed when editing with VKB
            if (iDisplayable && !iDisplayable->IsVKBOnScreen())
            {
                iCustomComponents[iFocusedComponent]->TraverseL(aEvent);
                response = EKeyWasConsumed;
            }
        }

        if (response != EKeyWasConsumed)
        {
            // Passing the keyevent to focused component
            response = iCustomComponents[iFocusedComponent]->
                       CustomComponentControl(KComponentMainControl)->
                       OfferKeyEventL(aEvent, aType);
        }
    }

    // Check if the key event was consumed by the child control.
    if (response == EKeyWasConsumed)
    {
        DEBUG_INT(
            "CMIDCanvas::OfferKeyEventL - key consumed by custom control: %d",
            iFocusedComponent);

        // Return response.
        return response;
    }


    DEBUG("CMIDCanvas::OfferKeyEventL, \
event not consumed by any custom component");

    TInt keycode = aEvent.iScanCode;

    // If there is additional selection key mapping (e.g. to Enter key) and
    // scan code of delivered key is this additional key, flag is set to ETrue
    TBool additionalSelectionKey =
        (iKeyDecoder->AdditionalSelectionKeyMappingCode() != 0) &&
        (keycode == iKeyDecoder->AdditionalSelectionKeyMappingCode());

    if (keycode == EStdKeyDevice0 ||
            keycode == EStdKeyDevice1 ||
            keycode == EStdKeyDevice3 ||
            additionalSelectionKey)
    {
        // CBA keys

        // In S60SelectionKeyCompatibility mode turned on and MSK pressed
        // don't map any command, send key event to Java.
        if (iS60SelectionKeyCompatibility &&
                (keycode == EStdKeyDevice3 || additionalSelectionKey))
        {
            SendKeyEventToJavaSideL(aEvent, aType);
            DEBUG("- CMIDCanvas::OfferKeyEventL = key was not consumed");
            return EKeyWasNotConsumed;
        }

        if (iDisplayable && !iDisplayable->IsFullScreenMode())
        {
            // Pressing selection key is handled in CEikCba so that command
            // mapped to selection (MSK) key is invoked from there.
            // But CEikCba doesn't handle additional selection key
            // mapping, so it is done here.
            if (additionalSelectionKey && aType == EEventKey)
            {
                iDisplayable->ProcessMSKCommandL();
            }
            else if (aType == EEventKeyDown && keycode == EStdKeyDevice3)
            {
                CMIDMenuHandler* menuHandler = iDisplayable->MenuHandler();

                // If there is no additional selection key mapping
                // and scan code of delivered key is MSK then show menu,
                // used when ENavigationKeysFire or MSK were pressed
                if (iDisplayable->NumCommandsForOkOptionsMenu() == 0
                        && menuHandler)
                {
                    menuHandler->ProcessCommandL(EAknSoftkeyEmpty);
                }
                else if (iDisplayable->NumCommandsForOkOptionsMenu() > 0
                         && menuHandler)
                {
                    menuHandler->ProcessCommandL(EAknSoftkeyContextOptions);
                }
            }

            // Not a full screen mode, return so that the CBA gets the event.
            DEBUG("- CMIDCanvas::OfferKeyEventL = key was not consumed");
            return EKeyWasNotConsumed;
        }

        if (iDisplayable && iDisplayable->IsFullScreenMode()  &&
                iDisplayable->IsCommandListenerSet() &&
                iDisplayable->CommandCount() > 0)
        {
            CMIDMenuHandler* menuHandler = iDisplayable->MenuHandler();

            // If full screen and command listener is set and there are
            // Commands added to Canvas, then options menu is displayed.
            if (aType == EEventKey && menuHandler)
            {
                menuHandler->ShowMenuL(
                    CMIDMenuHandler::EOptionsMenu);
            }

            DEBUG("- CMIDCanvas::OfferKeyEventL = key was not consumed");
            return EKeyWasNotConsumed;
        }
    }

    SendKeyEventToJavaSideL(aEvent, aType);

    DEBUG("- CMIDCanvas::OfferKeyEventL = key was not consumed");
    return EKeyWasNotConsumed;
}


// ---------------------------------------------------------------------------
// From class MMIDComponent.
// CMIDCanvas::Dispose
// Invoked by the framework to clean up resources.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Dispose()
{
    delete this;
}


// ---------------------------------------------------------------------------
// From class MMIDComponent.
// CMIDCanvas::Type
// Always returns the type MMIDComponent::ECanvas as component type.
// ---------------------------------------------------------------------------
//
MMIDComponent::TType CMIDCanvas::Type() const
{
    return MMIDComponent::ECanvas;
}


// ---------------------------------------------------------------------------
// From class MMIDComponent.
// CMIDCanvas::Processor
// Always returns this-> as buffer processor associated with this component.
// ---------------------------------------------------------------------------
//
MMIDBufferProcessor* CMIDCanvas::Processor()
{
    return this;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::Control
// Returns a reference to the CCoeControl corresponding to this canvas.
// ---------------------------------------------------------------------------
//
CCoeControl& CMIDCanvas::Control()
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::FrameBuffer
// Returns a pointer to the frame buffer. If double buffering is supported
// this method must not return NULL. There is no ownership transfer.
// ---------------------------------------------------------------------------
//
CFbsBitmap* CMIDCanvas::FrameBuffer() const
{
#ifdef CANVAS_DOUBLE_BUFFER
    return iFrameBuffer;
#else
    return NULL;
#endif // CANVAS_DOUBLE_BUFFER
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::ContentSize
// Returns the size of the canvas content.
// ---------------------------------------------------------------------------
//
TSize CMIDCanvas::ContentSize() const
{
    return iContentSize;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::PauseDirectAccess
// Disables temporarily direct screen access.
// This method should be called to disable direct screen access when a popup
// menu is about to be displayed. This is a workaround for menu's failing to
// correctly draw their shadows on Techview and other UI's that have floating
// menubars.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::PauseDirectAccess()
{
#ifdef CANVAS_DIRECT_ACCESS
    StopDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

    iFlags |= EDirectPaused;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::ResumeDirectAccess
// Re-enables direct screen access.
// To be called when the menu has disappeared.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ResumeDirectAccess()
{
    iFlags &= ~EDirectPaused;

#ifdef CANVAS_DIRECT_ACCESS
    StartDirectAccess();
#endif // CANVAS_DIRECT_ACCESS
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::SuppressKeys
// Switch off key event posting. Game keys will still be tracked, but
// keyPressed()/keyReleased() callbacks will not be called.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SuppressKeys()
{
    iFlags &= ~EPostKeyEvents;
    iEnv.ResetKeys();
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::GameActions
// Returns the latched state of the game action keys.
// ---------------------------------------------------------------------------
//
TUint32 CMIDCanvas::GameActions()
{
    TUint32 latch = (iGameKeyLatch | iGameKeyState);
    iGameKeyLatch = 0;
    return latch;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::DirectContainer
// Returns a direct container on which video can be rendered.
// ---------------------------------------------------------------------------
//
MDirectContainer& CMIDCanvas::DirectContainer()
{
    return *this;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::DrawBackground
// Draw a background image.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DrawBackground(
    CBitmapContext& aGc, const TPoint& aPosition, const TSize& aSize)
{
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = iDisplayable->BackGroundControlContext();

    AknsDrawUtils::DrawBackground(
        skin, cc, NULL, aGc, aPosition, aSize, KAknsDrawParamDefault);
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// CMIDCanvas::IsGameCanvas
// Returns the type of Canvas object.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::IsGameCanvas() const
{
    return iIsGameCanvas;
}


// ---------------------------------------------------------------------------
// From class MMIDCanvas.
// Gets the Network indicator location for fullscreen
// If not in fullscreenmode EFalse is returned
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::NetworkIndicatorLocation(
    TPoint& aPosition,
    TSize& aSize) const
{
    if (iFullScreen)
    {
        if (iNetworkIndicator)
        {
            iNetworkIndicator->GetLocation(aPosition, aSize);
            return ETrue;
        }
    }
    return EFalse;
}


#ifdef RD_TACTILE_FEEDBACK
// ---------------------------------------------------------------------------
// From MMIDCancas.
// CMIDCanvas::TactileFeedbackComponent
// Retutrns this as tactile feedback component.
// ---------------------------------------------------------------------------
//
MMIDTactileFeedbackComponent* CMIDCanvas::TactileFeedbackComponent()
{
    return this;
}
#endif // RD_TACTILE_FEEDBACK

#ifdef RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// From class MMIDBufferProcessor.
// CMIDCanvas::ProcessL
// Process a block of commands, updating read pointer as you go.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::ProcessL(
    const TMIDBufferOp*& aRead,
    const TMIDBufferOp* aEnd,
    TInt& /* aCycles */,
    java::util::Monitor* aMonitor)
{
    if (!iForeground)
    {
        DEBUG("CMIDCanvas::ProcessL() - not foreground");
        ASSERT(!iAlfMonitor);
        aRead = aEnd;
        return EFalse;
    }

    switch (aRead->OpCode())
    {
    case EDrwOpcM3GContentStart:
    {
        if (!iM3GContent &&
                i3DAccelerated &&
                iDirectContents.Count() == 0)
        {
            DEBUG("CMIDCanvas::ProcessL - M3G content start");
            iM3GContent = ETrue;
            iM3GStart = ETrue;

            // First time when M3G content is drawn =>
            // switch to EGL surface drawing.
            // Pixel source must be disposed first.
            DisposePixelSource();
            OpenEglL();

            // if we are scaling with m3g and HW acceleration
            // create a pbuffer surface to be used with m3g rendering
            if (iScalingOn && iFullScreen)
            {
                CreatePBufferSurfaceL();
            }
            // Draw all framebuffer content to EGL surface.
            iUpperUpdateRect = TRect(Size());
            UpdateEglContent();
        }
    }
    break;
    case EDrwOpcBitBltRect:
    {
        const TBitBltData& data = BitBltData(aRead);
        UpdateL(data.iRect);
    }
    break;
    case EDrwOpcBitBlt:
    {
        UpdateL(iViewRect);
    }
    break;
    case EDrwOpcFirstPaint:
    {
        java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi()->stopStartScreen();
    }
    break;
    default:
        User::Leave(KErrNotSupported);
        break;
    }

    aRead += aRead->Size();
    ASSERT(aRead == aEnd);

    // iFrameReady tells if this is async operation.
    // In async case CMIDBuffer waits until canvas
    // gets callback from Alf.
    // See CMIDBuffer.cpp and ProduceNewFrameL()
    if (iFrameReady)
    {
        iAlfMonitor = aMonitor;
    }
    return iFrameReady;
}

#else // RD_JAVA_NGA_ENABLED 

// ---------------------------------------------------------------------------
// From class MMIDBufferProcessor.
// CMIDCanvas::ProcessL
// Process a block of commands, updating read pointer as you go.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::ProcessL(
    const TMIDBufferOp*& aRead,
    const TMIDBufferOp* aEnd,
    TInt& aCycles,
    TRequestStatus* /* aStatus */)
{
    DEBUG("+ CMIDCanvas::ProcessL");

#ifdef CANVAS_DOUBLE_BUFFER
    while (aRead < aEnd)
    {
        switch (aRead->OpCode())
        {
        case EDrwOpcBitBlt:
        {
            TSize size = iFrameBuffer->SizeInPixels();

#ifdef DEFER_BITBLT
            aCycles -= BitBltCycles(size);

            if (aCycles < 0)
            {
                return EFalse;
            }
#endif // DEFER_BITBLT

            Update(TRect(size));
        }
        break;

        case EDrwOpcBitBltRect:
        {
            const TBitBltData& data = BitBltData(aRead);
            TRect rect = data.iRect;

            // This is needed to avoid artifacting after orientation switch.
            // While changing orientation, wrong rect was not updated.
            if (iWndUpdate && (rect.Size() != iViewRect.Size())
                    && !iScalingOn)
            {
                break;
            }

            TRect frameRect(iFrameBuffer->SizeInPixels());

            rect.Intersection(frameRect);

#ifdef DEFER_BITBLT
            aCycles -= BitBltCycles(rect.Size());

            if (aCycles < 0)
            {
                return EFalse;
            }
#endif // DEFER_BITBLT

            Update(rect);
        }
        break;
        case EDrwOpcFirstPaint:
        {
            java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi()->stopStartScreen();
        }
        break;
        default:
            User::Leave(KErrNotSupported);
            break;
        }

        aRead += aRead->Size();
    }
#endif // CANVAS_DOUBLE_BUFFER

    DEBUG("- CMIDCanvas::ProcessL");

    return EFalse;
}
#endif // RD_JAVA_NGA_ENABLED


// ---------------------------------------------------------------------------
// From class MMIDBufferProcessor.
// CMIDCanvas::AbortAsync
// Does nothing.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::AbortAsync()
{
    // nop
}


#ifdef CANVAS_DIRECT_ACCESS
// ---------------------------------------------------------------------------
// From class MAbortDirectScreenAccess.
// CMIDCanvas::AbortNow
// This function is called by the window server when direct screen access
// must stop (for example because a dialogue is moved in front of the area
// where direct screen access is taking place).
// ---------------------------------------------------------------------------
//
void CMIDCanvas::AbortNow(
    RDirectScreenAccess::TTerminationReasons /* aReasons */)
{
    StopDirectAccess();
}


// ---------------------------------------------------------------------------
// From class MDirectScreenAccess.
// CMIDCanvas::Restart
// This function is called by the window server as soon as direct screen
// access can resume.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Restart(
    RDirectScreenAccess::TTerminationReasons /* aReasons */)
{
    StartDirectAccess();
}
#endif // CANVAS_DIRECT_ACCESS


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcAddContent
// Set the content of this direct container.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcAddContent(MDirectContent* aContent)
{
    TInt error = iDirectContents.Append(aContent);

    __ASSERT_DEBUG(error == KErrNone, User::Invariant());

    // Register this control to observer
    // in order to provide possibility  to call
    // methods from another than LCDUI ES thread.
    // Unregistration is done in destructor.
#ifdef RD_JAVA_NGA_ENABLED
    iEnv.ToLcduiObserver().RegisterControl(*this, this);
#else
    iEnv.ToLcduiObserver().RegisterControl(*this);
#endif
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcRemoveContent
// Remove the content from this direct container. This will called when
// the Player is closed.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcRemoveContent(MDirectContent* aContent)
{
    TInt index = iDirectContents.Find(aContent);

    if (index != KErrNotFound)
    {
        iDirectContents.Remove(index);
        if (iDirectContents.Count() == 0)
        {
            iRestoreContentWhenUnfaded = EFalse;
        }
    }
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcAddContentBounds
// Adds a rectangle to be excluded from redrawing.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcAddContentBounds(const TRect& aRect)
{
    if (iDisplayable)
    {
        iDisplayable->AddDirectContentArea(aRect);
    }
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcRemoveContentBounds
// Removes a rectangle to be excluded from redrawing.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcRemoveContentBounds(const TRect& aRect)
{
    if (iDisplayable)
    {
        iDisplayable->RemoveDirectContentArea(aRect);
    }
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcContainerWindowRect()
// ---------------------------------------------------------------------------
//
TRect CMIDCanvas::MdcContainerWindowRect() const
{
    // CMIDCanvas has the size of the window always
    return MdcContentBounds();
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcContainerVisibility
// Get the visiblity of this direct container.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::MdcContainerVisibility() const
{
    return iDisplayable->IsVisible();
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcContentBounds
// Get the bounds of the area that the content of this direct container can
// occupy.
// Return content rect in screen co-ordinates. Position is stored in
// member variable because CCoeControl::PositionRelativeToScreen cannot
// be called from another thread.
// ---------------------------------------------------------------------------
//
TRect CMIDCanvas::MdcContentBounds() const
{
    return TRect(iPositionRelativeToScreen, Size());
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcItemContentRect
// Get the area on the Item on which content can be displayed.
// This should only be called when the direct content is displayed on an Item,
// so always panic if it is called on Canvas.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcItemContentRect(
    TRect& /* aContentRect */,
    TRect& /* aScreenRect */) const
{
    __ASSERT_DEBUG(EFalse, User::Invariant());
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcHandlePointerEventL
// Allows a control created for direct container content display to pass on
// pointer events.
// Though this is the leaving function nothing can leave actually.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcHandlePointerEventL(TPointerEvent& /* aPointerEvent */)
{
    __ASSERT_DEBUG(EFalse, User::Invariant());
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcFlushContainer
// Flush the direct container content.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcFlushContainer(const TRect& aRect)
{
    // Flush the framebuffer on screen
    iEnv.ToLcduiObserver().FlushControl(*this, aRect);
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcGetDSAResources
// Invokes callback aConsumer->MdcDSAResourcesCallback in LCDUI thread.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcGetDSAResources(MUiEventConsumer& aConsumer)
{
    // May run in non-LCDUI ES thread
    // Invoke the callback running in LCDUI ES thread
    iEnv.ToLcduiObserver().InvokeDSAResourcesCallback(*this, aConsumer);
}


// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcGetUICallback
// Invokes callback aConsumer->MdcUICallback in LCDUI thread.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcGetUICallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    // May run in non-LCDUI ES thread
    // Invoke the callback running in LCDUI ES thread
    iEnv.ToLcduiObserver().InvokeUICallback(aConsumer, aCallbackId);
}

#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// From class MDirectContainer.
// CMIDCanvas::MdcNotifyContentAdded
// Notification about added direct content, called in LCDUI thread.
// Canvas needs to dispose pixel source before MMAPI adds video surface
// to canvas window. Pixel source cannot be disposed in MdcAddContent
// because it may be called in MMAPI thread.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MdcNotifyContentAdded()
{
    DisposePixelSource();
    CloseEgl();
}
#endif // RD_JAVA_NGA_ENABLED


// ---------------------------------------------------------------------------
// From class MMIDMediaKeysListener.
// CMIDCanvas::HandleMediaKeyEvent
// This method is called when a media key has been pressed.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::HandleMediaKeyEvent(TMIDKeyEvent& aKeyEvent)
{
    if (PostKeyEvents())
    {
        DEBUG_INT3(
            "CMIDCanvas::HandleMediaKeyEvent - PostKeyEvent() - \
JAVA code %d type %d repeats %d",
            aKeyEvent.iKeyCode,
            aKeyEvent.iEvents,
            aKeyEvent.iRepeats);

        iEnv.PostKeyEvent(*this, aKeyEvent);
    }
}


#ifdef RD_TACTILE_FEEDBACK
// ---------------------------------------------------------------------------
// From class MMIDTactileFeedbackComponent.
// CMIDCanvas::UpdateTactileFeedback
// Update tactile feedback areas.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UpdateTactileFeedback()
{
    TInt areaCount = iTactileFeedback->GetAreasCount();

    for (TInt i = 0; i < areaCount; i++)
    {
        CMIDTactileFeedbackExtension::FeedbackArea* area =
            iTactileFeedback->GetArea(i);

        TRect physicalRect = area->rect;

        iTactileFeedback->SetFeedbackArea(area->id,
                                          physicalRect,
                                          (TInt)area->style);
    }
}


// ---------------------------------------------------------------------------
// From class MMIDTactileFeedbackComponent.
// CMIDCanvas::RegisterFeedbackArea
// Register feedback area.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::RegisterFeedbackArea(TInt aId, TRect aRect, TInt aStyle)
{
    iTactileFeedback->RegisterFeedbackArea(aId, aRect, aStyle);
}


// ---------------------------------------------------------------------------
// From class MMIDTactileFeedbackComponent.
// CMIDCanvas::UnregisterFeedbackArea
// Unregister feedback area.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UnregisterFeedbackArea(TInt aId)
{
    iTactileFeedback->UnregisterFeedbackArea(aId);
}


// ---------------------------------------------------------------------------
// From class MMIDTactileFeedbackComponent.
// CMIDCanvas::UnregisterFeedbackForControl
// Unregister feedback for the control.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UnregisterFeedbackForControl()
{
    iTactileFeedback->UnregisterFeedbackForControl();
}


// ---------------------------------------------------------------------------
// From class MMIDTactileFeedbackComponent.
// CMIDCanvas::MoveAreaToFirstPriority
// Move area to first priority.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::MoveAreaToFirstPriority(TInt aId)
{
    iTactileFeedback->MoveAreaToFirstPriority(aId);
}
#endif // RD_TACTILE_FEEDBACK

// ---------------------------------------------------------------------------
// CMIDCanvas::RegisterComponentL
// Registers a new custom component to this container.
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvas::RegisterComponentL(MMIDCustomComponent* aComponent)
{
    DEBUG("CMIDCanvas::RegisterComponentL +");

    if (iCustomComponents.Find(aComponent) == KErrNotFound)
    {
        DEBUG("CMIDCanvas::RegisterComponentL, registering new");

        iCustomComponents.Append(aComponent);

        // Update custom components count in order to improve
        // the performance when counting components
        iCustomComponentsControlCount +=
            aComponent->CustomComponentControlCount();

    }

    DEBUG("CMIDCanvas::RegisterComponentL -");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::UnregisterComponent
// Unregisters an existing custom component from this container.
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UnregisterComponent(MMIDCustomComponent* aComponent)
{
    DEBUG("CMIDCanvas::UnregisterComponent +");

    TInt index = iCustomComponents.Find(aComponent);

    if (index != KErrNotFound)
    {
        // Update of iFocusedComponent index.
        if (index < iFocusedComponent)
        {
            iFocusedComponent--;
        }
        else if (index == iFocusedComponent)
        {
            iFocusedComponent = KComponentFocusedNone;
        }

        if ((iPressedComponent) && (iPressedComponent == aComponent))
        {
            iPressedComponent = NULL;
        }

        iCustomComponents.Remove(index);
        // Compress the array to keep the object indexes in order.
        iCustomComponents.Compress();
        // Update custom components count in order to improve
        // the performance when counting components
        iCustomComponentsControlCount -=
            aComponent->CustomComponentControlCount();

        // Redraw the whole canvas.
        DrawDeferred();
    }

    DEBUG("CMIDCanvas::UnregisterComponent -");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::IsFullScreen
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::IsFullScreen() const
{
    return iFullScreen;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::SetComponentIndexL
// Changes the index of the specified custom component.
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SetComponentIndexL(
    MMIDCustomComponent* aComponent,
    TInt aNewIndex)
{
    DEBUG("CMIDCanvas::SetComponentIndexL +");

    // Adjust the index if it is out of bounds.
    TInt count = iCustomComponents.Count();
    if (aNewIndex >= count)
    {
        aNewIndex = count - 1;
    }

    // The index must not be negative.
    __ASSERT_DEBUG(aNewIndex >= 0, User::Invariant());

    TInt index = iCustomComponents.Find(aComponent);

    // Do not adjust the position if there is no need for it
    // or if the component does not exist in the array.
    if (index != KErrNotFound && index != aNewIndex)
    {
        // Update index of focused component

        if (index == iFocusedComponent)
        {
            iFocusedComponent = aNewIndex;
        }
        else if ((iFocusedComponent < index) && (iFocusedComponent >= aNewIndex))
        {
            iFocusedComponent++;
        }
        else if ((iFocusedComponent > index) && (iFocusedComponent <= aNewIndex))
        {
            iFocusedComponent--;
        }

        // Remove the old index from the components array.
        iCustomComponents.Remove(index);

        // Add the component to the new index. Note that the array
        // should contain memory for all the components since
        // Remove should not decrease the memory used by the array
        // Therefore, the following operation is leave-safe and
        // return value is ignored intentionally.
        iCustomComponents.Insert(aComponent, aNewIndex);

        // Redraw the whole canvas.
        DrawDeferred();
    }

    DEBUG("CMIDCanvas::SetComponentIndexL -");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::ComponentIndex
// Returns the index of the given component in this container.
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
TInt CMIDCanvas::ComponentIndex(MMIDCustomComponent* aComponent) const
{
    DEBUG("CMIDCanvas::ComponentIndex");

    return iCustomComponents.Find(aComponent);
}

// ---------------------------------------------------------------------------
// CMIDCanvas::PostEvent
// Post an event to Java side.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::PostEvent(
    TEventType aType,
    TInt aData0,
    TInt aData1) const
{
    return iEnv.PostJavaEvent(const_cast< CMIDCanvas& >(*this),
                              EDisplayable,
                              aType,
                              aData0,
                              aData1,
                              0);
}

#ifdef RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// CMIDCanvas::Update
// Update screen from frame buffer.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UpdateL(const TRect& aRect)
{
    // Drawing Network indicator only when Update is called.
    if (iFullScreen && iNetworkIndicator)
    {
        iNetworkIndicator->SetDrawIndicator(ETrue);
    }

    // iRestoreContentWhenUnfaded is used when Canvas should be faded
    // DrawNow need to be called, otherwise  Canvas will be unfaded
    if (iDirectContents.Count() == 0 && !iRestoreContentWhenUnfaded && IsFocused())
    {
        // In case direct content content was removed
        // from canvas, recreate pixel source here
        if (!iAlfCompositionPixelSource &&
                !IsEglAvailable())
        {
            InitPixelSourceL();
        }
        DrawWindowNgaL(aRect);
    }
    else
    {
        // Must draw via CCoeControl framework
        DrawNow(aRect);
    }

    // This is needed to avoid artifacting after orientation switch.
    // It is called once after orientation change.
    if (iWndUpdate)
    {
        iWndUpdate = EFalse;
    }
}

#else // !RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// CMIDCanvas::Update
// Update screen from frame buffer.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Update(const TRect& aRect)
{
    DEBUG("- CMIDCanvas::Update");

#ifdef CANVAS_DOUBLE_BUFFER
    if (IsVisible())
    {
        // Drawing Network indicator only when Update is called.
        if (iFullScreen && iNetworkIndicator)
        {
            iNetworkIndicator->SetDrawIndicator(ETrue);
        }

#ifdef CANVAS_DIRECT_ACCESS
        // Check if some direct screen content is drawn.
        if (iDirectContents.Count() > 0)
        {
            // There is some direct screen content drawn, DSA is not free for
            // us at the moment.
            DrawNow(aRect);
        }
        else
        {
            // No direct screen content is drawn, so we can try to use DSA.
            StartDirectAccess();

            if (iDirectGc)
            {
                // DSA is ready to use, draw directly.
                DrawDirect(aRect);
            }
            else
#endif // CANVAS_DIRECT_ACCESS
            {
                ActivateGc();
                DrawWindow(aRect);
                DeactivateGc();
            }
#ifdef CANVAS_DIRECT_ACCESS
        }
#endif // CANVAS_DIRECT_ACCESS

        // This is needed to avoid artifacting after orientation switch.
        // It is called once after orientation change.
        if (iWndUpdate)
        {
            iWndUpdate = EFalse;
        }
    }
#endif // CANVAS_DOUBLE_BUFFER

    DEBUG("+ CMIDCanvas::Update");
}

#endif // RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// CMIDCanvas::PrepareDraw
// Prepare drawing. Used in DrawWindow and DrawDirect.
// aWindowRect is in bitmap coords.
// ---------------------------------------------------------------------------
//
inline void CMIDCanvas::PrepareDraw(
    CGraphicsContext& aGc, TRect& aWinRect) const
{
    const TRect rect = Rect();
#ifdef RD_JAVA_NGA_ENABLED
    if (iDirectContents.Count() > 0)
    {

        // Content may be centered in which case we need to clear the background.
        if (rect != iViewRect)
        {
            // Clear background with black if centered.
            aGc.SetBrushColor(KRgbBlack);
            aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);

            DrawUtils::ClearBetweenRects(aGc, rect, iViewRect);
        }
    }
#else
    // Content may be centered in which case we need to clear the background.
    if (rect != iViewRect)
    {
        // Clear background with black if centered.
        aGc.SetBrushColor(KRgbBlack);
        aGc.SetBrushStyle(CGraphicsContext::ESolidBrush);

        DrawUtils::ClearBetweenRects(aGc, rect, iViewRect);
    }
#endif // RD_JAVA_NGA_ENABLED

    if (iFullScreen && iNetworkIndicator)
    {
        // Network indicator or call indicator is drawn to the off-screen
        // buffer if connection is open.
        iNetworkIndicator->DrawNetworkIndicator(
            *iFrameContext, aWinRect, iViewRect);
    }

    if (iViewRect.Size() == iContentSize)
    {
        // Transform the source rect to window coords.
        aWinRect.Move(iViewRect.iTl);

        // Clip to viewport.
        aWinRect.Intersection(iViewRect);
    }
    else
    {
        aWinRect = iViewRect;
    }
}


#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// CMIDCanvas::DrawWindowNgaL
// Draw using pixel source of EGL surface depending on what canvas content
// has been drawn.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DrawWindowNgaL(const TRect& aRect)
{
    ASSERT(IsEglAvailable() || iAlfCompositionPixelSource);

    if (iFullScreen && iNetworkIndicator)
    {
        TRect windowRect(aRect);
        iNetworkIndicator->DrawNetworkIndicator(
            *iFrameContext, windowRect, iViewRect);
    }

    if (!IsEglAvailable())
    {
        // No M3G content, use pixel source
        ActivatePixelSourceL();
        DEBUG("CMIDCanvas::Draw - Pixel Source activated");
    }
    else // M3G content, use EGL surface
    {
        // Invalidates window so that wserv does not
        // draw window content on top our EGL surface.
        // This is needed only once when starting to use EGL surface.
        if (iM3GStart)
        {
            DEBUG("CMIDCanvas::DrawWindow - invalidate");
            Window().Invalidate();
            iM3GStart = EFalse;
        }

        if (iScalingOn && iFullScreen)
        {
            SetCurrentEglType(EEglPbuffer);
            BlitFrameBufferPixels();
            SetCurrentEglType(EEglWindow);
            if (iWndUpdate)
            {
                DEBUG("CMIDCanvas::DrawWindowNgaL() - clearing window surface");
                ClearEglSurface(EEglWindow);
            }
            BlitPBufferScaledToWindowSurface();
        }
        else
        {
            SetCurrentEglType(EEglWindow);
            BlitFrameBufferPixels();
        }

        if (eglSwapBuffers(iEglDisplay, iEglWindowSurface) == EGL_FALSE)
        {
            ELOG1(EJavaUI, "eglSwapBuffers() failed, eglError=%d", eglGetError());
            ASSERT(EFalse);
        }
        SetCurrentEglType(EEglNone);
    }
}
#endif // RD_JAVA_NGA_ENABLED


#ifdef CANVAS_DOUBLE_BUFFER
// ---------------------------------------------------------------------------
// CMIDCanvas::DrawWindow
// Draw through Window server using native frame buffer.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DrawWindow(const TRect& aRect) const
{
    DEBUG("CMIDCanvas::DrawWindow ++");
    ASSERT(iFrameBuffer);

    CWindowGc& gc = SystemGc();

    // If EColor16MA display mode is used, graphics context needs to set its
    // draw mode to EDrawModeWriteAlpha for just to copy an alpha channel and
    // not to do any blending.
    if (iFrameBuffer->DisplayMode() == EColor16MA)
    {
        gc.SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
    }

    TRect windowRect(aRect);
    PrepareDraw(gc, windowRect);

    // Check if Canvas scaling is used.
    if (iViewRect.Size() == iContentSize)
    {
        DEBUG("DrawWindow - Not scaled - BitBlt");
        gc.BitBlt(windowRect.iTl, iFrameBuffer, windowRect);
    }
    else if (IsDownScaling(iContentSize, iViewRect))
    {
        DEBUG("DrawWindow - Downscaling - BitBlt");
        gc.BitBlt(windowRect.iTl, iFrameBuffer, windowRect.Size());
    }
    // Upscaling
    else if (iScaler)
    {
        iFrameBuffer->LockHeap();
        TUint32* pixelData = iFrameBuffer->DataAddress();

        // Scale the framebuffer content.
        CFbsBitmap* map = iScaler->Process(
                              iFrameBuffer->DisplayMode(),
                              pixelData,
                              iContentSize.iWidth,
                              iContentSize.iHeight,
                              iFrameBuffer->SizeInPixels().iWidth - iContentSize.iWidth,
                              iViewRect.Width(),
                              iViewRect.Height());

        iFrameBuffer->UnlockHeap();

        if (map)
        {
            DEBUG("DrawWindow - Upscaling - BitBlt - map ok");
            gc.BitBlt(windowRect.iTl, map, windowRect.Size());
        }
        else
        {
            DEBUG("DrawWindow - Upscaling - DrawBitmap - no map");
            gc.DrawBitmap(windowRect, iFrameBuffer, iContentSize);
        }
    }

#ifdef RD_JAVA_NGA_ENABLED
    iCoeEnv->WsSession().Finish();
#endif

    DEBUG("CMIDCanvas::DrawWindow --");
}
#endif // CANVAS_DOUBLE_BUFFER


#ifdef CANVAS_DIRECT_ACCESS
// ---------------------------------------------------------------------------
// CMIDCanvas::StartDirectAccess
// Start Direct Screen Access.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::StartDirectAccess()
{
    // If DSA is not enabled or paused, or direct GC already exists just get
    // out of here.
    if (!DirectEnabled() || DirectPaused() || iDirectGc)
    {
        return;
    }

    CCoeAppUi& appUi = *static_cast< CCoeAppUi* >(ControlEnv()->AppUi());

    if (appUi.IsDisplayingMenuOrDialog())
    {
        return;
    }

    TRAPD(err, RestartL());

    if (KErrNone != err)
    {
        StopDirectAccess();
    }
    else
    {
        if (iDcDsaToStart && iDirectGc)
        {
            // Resume DSA for all direct contents
            iDcDsaToStart = EFalse;
            TInt contentsCount(iDirectContents.Count());

            for (TInt j = 0; j < contentsCount; j++)
            {
                iDirectContents[j]->MdcResumeDSA();
            }
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDCanvas::StopDirectAccess
// Stop Direct Screen Access.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::StopDirectAccess()
{
    iDirectGc = NULL;
    if (!iDcDsaToStart)
    {
        // Abort direct DSA for all direct contents

        TInt contentsCount(iDirectContents.Count());

        if (contentsCount > 0)
        {
            iDcDsaToStart = ETrue;
        }

        for (TInt j = 0; j < contentsCount; j++)
        {
            iDirectContents[j]->MdcAbortDSA();
        }
    }

    if (iDirectAccess)
    {
        iDirectAccess->Cancel();
    }
}


// ---------------------------------------------------------------------------
// CMIDCanvas::RestartL
// Restart Direct Screen Access.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::RestartL()
{
    ASSERT(DirectEnabled());
    ASSERT(iDirectAccess);

    if (!iDirectAccess->IsActive())
    {
        iDirectAccess->StartL();
    }

    iDirectGc = NULL;

    if (iDirectAccess->DrawingRegion()->Count() == 1)
    {
        // Check that the drawing region is fully visibile (not covered)
        TRect drawableRect((*iDirectAccess->DrawingRegion())[0]);
        TRect canvasRect(PositionRelativeToScreen(), Size());

        drawableRect.Intersection(canvasRect);

        // Check that the drawing region has the proper size
        if (drawableRect == canvasRect)
        {
            // Prepare context for actual drawing
            iDirectGc = iDirectAccess->Gc();

            // Direct draw is using only BitBlt and DrawBitmap so we can
            // always use EDrawModeWriteAlpha draw mode to optimize drawing
            // process. If EColor16MA display mode is used, graphics context
            // needs to set its draw mode to EDrawModeWriteAlpha to just copy
            // an alpha channel and not do any blending.
            if (iFrameBuffer->DisplayMode() == EColor16MA)
            {
                iDirectGc->SetDrawMode(
                    CGraphicsContext::EDrawModeWriteAlpha);
            }
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDCanvas::DrawDirect
// Draw using Direct Screen Access.
// aRect is in framebuffer coords.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DrawDirect(const TRect& aRect) const
{
    DEBUG("+ CMIDCanvas::DrawDirect");

    ASSERT(DirectEnabled());
    ASSERT(iDirectGc);
    ASSERT(iDirectAccess);
    ASSERT(iFrameBuffer);

    // Visible region in screen coordinates. Should only contain a single clip
    // rect otherwise direct access should have aborted.
    const RRegion& visibleRegion = *(iDirectAccess->DrawingRegion());

    ASSERT(visibleRegion.Count() == 1);

    TRect windowRect(aRect);
    PrepareDraw(*iDirectGc, windowRect);

    // Check if Canvas scaling is used.
    if (iViewRect == iContentSize)
    {
        // No scaling, plain draw.
        DEBUG("DrawDirect - NotScaled");
        iDirectGc->BitBlt(windowRect.iTl, iFrameBuffer, windowRect);
    }
    else if (IsDownScaling(iContentSize, iViewRect))
    {
        DEBUG("DrawDirect - Downscaling");
        iDirectGc->BitBlt(windowRect.iTl, iFrameBuffer, windowRect.Size());
    }
    // Upscaling
    else if (iScaler)
    {
        iFrameBuffer->LockHeap();
        TUint32* pixelData = iFrameBuffer->DataAddress();

        // Scale the framebuffer content.
        CFbsBitmap* map = iScaler->Process(
                              iFrameBuffer->DisplayMode(),
                              pixelData,
                              iContentSize.iWidth,
                              iContentSize.iHeight,
                              iFrameBuffer->SizeInPixels().iWidth - iContentSize.iWidth,
                              iViewRect.Width(),
                              iViewRect.Height());

        iFrameBuffer->UnlockHeap();

        if (map)
        {
            DEBUG("DrawDirect - Upscaling - BitBlt - map ok");
            iDirectGc->BitBlt(windowRect.iTl, map, windowRect.Size());
        }
        else
        {
            DEBUG("DrawDirect - Upscaling - DrawBitmap - no map");
            iDirectGc->DrawBitmap(windowRect, iFrameBuffer, iContentSize);
        }
    }

    if (UpdateRequired())
    {
        windowRect.Move(PositionRelativeToScreen());
        windowRect.Intersection(visibleRegion.RectangleList()[0]);

        // Direct screen drawing does not show up on some devices unless the
        // screen device Update() method is called.
        iUpdateRegion.Clear();
        iUpdateRegion.AddRect(windowRect);
        iDirectAccess->ScreenDevice()->Update(iUpdateRegion);
    }

    DEBUG("- CMIDCanvas::DrawDirect");
}
#endif //CANVAS_DIRECT_ACCESS


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::HandleResourceChange
// Resource change handling.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::HandleResourceChange(TInt aType)
{
    if (aType == KEikInputLanguageChange)
    {
        if ((iFocusedComponent != KComponentFocusedNone) &&
                (iFocusedComponent < iCustomComponents.Count()))
        {
            iCustomComponents[iFocusedComponent]->
            CustomComponentControl(KComponentMainControl)->
            HandleResourceChange(aType);
        }
    }
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        // If orientation change is done,
        // then we have to inform all components about it.
        if (iFocusedComponent != KComponentFocusedNone &&
                iFullScreen)
        {
            for (int i = 0; i < iCustomComponents.Count(); i++)
            {
                iCustomComponents[i]->HandleResolutionChange();
            }
        }
    }
    else if ((aType == KEikMessageUnfadeWindows) ||
             (aType == KEikMessageFadeAllWindows))
    {
        iLastFadeMessage = aType;
    }
    else if ((aType == KEikMessageWindowsFadeChange) &&
             ((iLastFadeMessage == KEikMessageUnfadeWindows) ||
              (iLastFadeMessage == KEikMessageFadeAllWindows)))
    {
        TInt contentsCount(iDirectContents.Count());

        switch (iLastFadeMessage)
        {
        case KEikMessageUnfadeWindows:
            iRestoreContentWhenUnfaded = EFalse;

#ifdef CANVAS_DIRECT_ACCESS
            ResumeDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

            for (TInt j = 0; j < contentsCount; j++)
            {
                iDirectContents[j]->MdcContainerVisibilityChanged(ETrue);
            }
            break;

        case KEikMessageFadeAllWindows:
            iRestoreContentWhenUnfaded = ETrue;

#ifdef CANVAS_DIRECT_ACCESS
            PauseDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

            for (TInt j = 0; j < contentsCount; j++)
            {
                iDirectContents[j]->MdcContainerVisibilityChanged(EFalse);
            }
            break;
        }
        iLastFadeMessage = 0;
    }
}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CMIDCanvas::HandlePointerEventInControlsL
// Pointer events handling in controls.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::HandlePointerEventInControlsL(
    const TPointerEvent& aPointerEvent)
{
    TBool inControl = EFalse;

    switch (aPointerEvent.iType)
    {
    case TPointerEvent::EButton1Down:
    case TPointerEvent::EButton2Down:
    case TPointerEvent::EButton3Down:
        // Pointer pressed
        if ((iFocusedComponent > KComponentFocusedNone)&&
                (iFocusedComponent < iCustomComponents.Count()) &&
                (PointerEventInControl(iCustomComponents[iFocusedComponent],
                                       aPointerEvent.iPosition)))
        {
            // Store the control for receiving pointer events until release
            iPressedComponent = iCustomComponents[iFocusedComponent];
            // Pass the pointer event
            iPressedComponent->CustomComponentControl
            (KComponentMainControl)->HandlePointerEventL(aPointerEvent);
            inControl = ETrue;
        }
        break;
    case TPointerEvent::EButton1Up:
    case TPointerEvent::EButton2Up:
    case TPointerEvent::EButton3Up:
        // Pointer released
        if ((iPressedComponent) && (PointerEventInControl(iPressedComponent,
                                    aPointerEvent.iPosition)))
        {
            // Pass the pointer event
            iPressedComponent->CustomComponentControl
            (KComponentMainControl)->HandlePointerEventL(aPointerEvent);
            iPressedComponent = NULL;
            inControl = ETrue;
        }
        break;
    case TPointerEvent::EDrag:
        if (iPressedComponent)
        {
            iPressedComponent->ProcessPointerEventL(aPointerEvent);
            inControl = ETrue;
        }
        break;
    default:
        break;
    }
    return inControl;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::PointerEventInControl
// Pointer events checking.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::PointerEventInControl(MMIDCustomComponent* aControl,
                                        TPoint aPoint)
{
    TBool inControl = EFalse;
    TInt compoundCount = aControl->CustomComponentControlCount();
    for (int j = 0; j < compoundCount; j++)
    {
        // Check if the pointer event position is in visible control's
        // area.
        if (aControl->CustomComponentControl(j)->IsVisible())
        {
            inControl |= aControl->CustomComponentControl(j)->
                         Rect().Contains(aPoint);
        }
    }
    return inControl;
}

// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::HandlePointerEventL
// Pointer events handling.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (AknLayoutUtils::PenEnabled())
    {
        TPoint point = aPointerEvent.iPosition;
        TInt count = iCustomComponents.Count();
        TBool consumed = EFalse;

        // Long tap not allowed in full screen mode.
        if (!iFullScreen)
        {
            consumed = iDisplayable->TryDetectLongTapL(aPointerEvent);
        }

        if (!consumed && (iFocusedComponent != KComponentFocusedNone) &&
                (iCustomComponents[iFocusedComponent]->IsTouchEnabled()))
        {
            consumed = HandlePointerEventInControlsL(aPointerEvent);
        }

        if (!consumed)
        {
            // Points are counted from 0 to width - 1, resp. height -1,
            // so we compute maxima for points coordinates based
            // on native view size and java canvas size.
            TSize nativeMaxCoords = iViewRect.Size() - TSize(1, 1);
            TSize javaMaxCoords = iContentSize - TSize(1, 1);

            // Transform point to content coordinates.
            //
            // iViewRect.iTl                -> (0, 0)
            // iViewRect.iBr - TSize(1, 1)  -> (iContentSize.iWidth - 1,
            //                                  iContentSize.iHeight - 1)
            point -= iViewRect.iTl;

            if (javaMaxCoords != nativeMaxCoords)
            {
                point.iX = (point.iX * javaMaxCoords.iWidth) /
                           nativeMaxCoords.iWidth;
                point.iY = (point.iY * javaMaxCoords.iHeight) /
                           nativeMaxCoords.iHeight;
            }

            TEventType type;
            TTime now;

            switch (aPointerEvent.iType)
            {
            case TPointerEvent::EButton1Down:
            case TPointerEvent::EButton2Down:
            case TPointerEvent::EButton3Down:
                type = EPointerPressed;
                iPointerPressedCoordinates = TPoint(point.iX, point.iY);
                if (point.iX >= 0 && point.iX <= javaMaxCoords.iWidth &&
                        point.iY >= 0 && point.iY <= javaMaxCoords.iHeight)
                {
                    // Translated point is inside of java canvas
                    iDragEventsStartedInside = ETrue;
                }
                break;
            case TPointerEvent::EButton1Up:
            case TPointerEvent::EButton2Up:
            case TPointerEvent::EButton3Up:
                type = EPointerReleased;
                if (iPointerEventSuppressionOngoing)
                {
                    point.iX = iPointerPressedCoordinates.iX;
                    point.iY = iPointerPressedCoordinates.iY;
                }
                break;
            case TPointerEvent::EDrag:
                type = EPointerDragged;
                // If pointerEventSuppressor time is set to 0 don't do any
                // drag event filtering. (default time here is 500ms and it
                // can be changed with JAD parameter
                // Nokia-MIDlet-Tap-Detection-Options)
                if (iPESTimeInMilliseconds != 0)
                {
                    now.HomeTime();
                    if (now.MicroSecondsFrom(iPreviousDragTimeStamp) <
                            KAllowedTimeBetweenDragEvents)
                    {
                        // Ignore drag event because time between drags is
                        // shorter than allowed interval.
                        return;
                    }
                    else
                    {
                        iPreviousDragTimeStamp = now;
                    }
                }
                break;
            default:
                type = ENoType;
            }

            // Pointer Event Suppressor for helping tap detection with finger
            // usage
            if (iPointerEventSuppressor->SuppressPointerEvent(aPointerEvent))
            {
                iPointerEventSuppressionOngoing = ETrue;
                return;
            }
            else
            {
                iPointerEventSuppressionOngoing = EFalse;
            }

            // To have the cursor on focused control
            if (iFocusedComponent != KComponentFocusedNone)
            {
                iCustomComponents[iFocusedComponent]->
                CustomComponentControl(KComponentMainControl)->
                SetFocus(ETrue);
            }

            // Ignore pointer events outside of Canvas content.
            if (!iDragEventsStartedInside)
            {
                return;
            }

            PostEvent(type, point.iX, point.iY);

            // Flag is reset, if release event happened. However reset
            // has to be done after event is sent to java, because of possible
            // dragging, which started inside of canvas. In this case
            // also release event should be delivered to java
            if (type == EPointerReleased)
            {
                iDragEventsStartedInside = EFalse;
            }
        }
    }
}
#endif // RD_SCALABLE_UI_V2


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::FocusChanged
// Change of focus.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::FocusChanged(TDrawNow /* aDrawNow */)
{
    DEBUG("+ CMIDCanvas::FocusChanged");

    iEnv.ResetKeys();
    iGameKeyState = 0;
    iGameKeyLatch = 0;

    TInt contentsCount(iDirectContents.Count());

    if (IsFocused())
    {
#ifdef CANVAS_DIRECT_ACCESS
        ResumeDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

        if (!iRestoreContentWhenUnfaded)
        {
            for (TInt j = 0; j < contentsCount; j++)
            {
                iDirectContents[j]->MdcContainerVisibilityChanged(ETrue);
            }
        }

        // To have cursor on focused control.
        if ((iFocusedComponent != KComponentFocusedNone) &&
                (iFocusedComponent < iCustomComponents.Count()))
        {
            iCustomComponents[iFocusedComponent]->
            CustomComponentControl(KComponentMainControl)->
            SetFocus(ETrue);
        }
        // Redraw the canvas after unfading
        DrawDeferred();

#ifdef RD_JAVA_NGA_ENABLED
        // To avoid situation when Canvas is redrawn but black area remains
        if (iAlfCompositionPixelSource)
        {
            TRAPD(err, ActivatePixelSourceL());
            if (err != KErrNone)
            {
                DEBUG_INT("CMIDCanvas::FocusChanged - ActivatePixelSourceL error %d", err);
            }
        }
#endif // RD_JAVA_NGA_ENABLED

    }
    else
    {
#ifdef CANVAS_DIRECT_ACCESS
        PauseDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

        if (!iRestoreContentWhenUnfaded)
        {
            for (TInt j = 0; j < contentsCount; j++)
            {
                iDirectContents[j]->MdcContainerVisibilityChanged(EFalse);
            }
        }

        // To  cursor on focused control.
        if ((iFocusedComponent != KComponentFocusedNone) &&
                (iFocusedComponent < iCustomComponents.Count()))
        {
            iCustomComponents[iFocusedComponent]->
            CustomComponentControl(KComponentMainControl)->
            SetFocus(EFalse);
        }
        // Repaint to ensure that fading will be displayed correctly for Alert
        // or PopupTextBox when DSA is paused.
        DrawDeferred();
    }

    DEBUG("- CMIDCanvas::FocusChanged");
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::SizeChanged
// Change of size.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SizeChanged()
{
    DEBUG("+ CMIDCanvas::SizeChanged");

    TSize controlSize = Size();

    // Position in screen co-ordinates changes when mode (full/normal) is
    // changed.
    iPositionRelativeToScreen = PositionRelativeToScreen();
    TRect contentBounds = TRect(iPositionRelativeToScreen, controlSize);

    // If JAD attribute Nokia-MIDlet-Original-Display-Size for graphics
    // scaling is defined, the iContentSize must follow it.
    TSize orientedOrgMIDletScrSize(OrientedOrgMIDletScrSize());

    if (contentBounds != iOldContentBounds)
    {
        iOldContentBounds = contentBounds;

        TSize contentSize;

        if (iFullScreen && (orientedOrgMIDletScrSize != controlSize))
        {
            contentSize = orientedOrgMIDletScrSize;
        }

        if (contentSize == KZeroSize)
        {
            contentSize = controlSize;
        }

        if ((contentSize != iContentSize) || iScalingOn)
        {
            iContentSize = contentSize;

#ifdef RD_JAVA_NGA_ENABLED
            HandleSizeChanged();
#endif // RD_JAVA_NGA_ENABLED
            PostEvent(ESizeChanged, iContentSize.iWidth, iContentSize.iHeight);
        }

#ifdef CANVAS_DIRECT_ACCESS
        // Stop direct screen access to avoid restarting
        // of direct content DSA during MdcContentBoundsChanged.
        // DSA is resumed by Layout method.
        StopDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

        // Inform all listeners.
        TInt contentsCount(iDirectContents.Count());

        for (TInt j = 0; j < contentsCount; j++)
        {
            iDirectContents[j]->MdcContentBoundsChanged(contentBounds);
            iDirectContents[j]->MdcContainerWindowRectChanged(MdcContainerWindowRect());
        }

        // We cannot determine whether the control size actually did change or
        // not, so we have to do the layout no matter what.
        Layout();
    }
    DEBUG("- CMIDCanvas::SizeChanged");
}


// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::PositionChanged
// Change of position.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::PositionChanged()
{
    Layout();
}

#ifdef RD_JAVA_NGA_ENABLED
// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::Draw
// Drawing.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Draw(const TRect& aRect) const
{
    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDCanvas::Draw starts", -1, -1);
    }
    DEBUG("CMIDCanvas::Draw ++");

    // While changing the orientation method DrawWindow() not called,
    // variable iWndUpdate is set to True. If iWndUpdate is True
    // DrawWindow() is called from method Update()
    // This is needed to avoid artifacting after orientation switch.
    TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();

    if (iLandscape != landscape)
    {
        iLandscape = landscape;
        iWndUpdate = ETrue;
    }
    else
    {
        // iRestoreContentWhenUnfaded is used when Canvas should be faded
        // DrawWindow need to be called, otherwise  Canvas will be unfaded
        // Sometimes iRestoreContentWhenUnfaded is not set yet and Canvas
        // already lost focus, then IsFocused is used
        if (iDirectContents.Count() > 0 || iRestoreContentWhenUnfaded || !IsFocused())
        {
            DrawWindow(aRect);
        }
        else if (iAlfCompositionPixelSource)
        {
            CMIDCanvas* myself = const_cast<CMIDCanvas*>(this);
            TRAP_IGNORE(myself->ActivatePixelSourceL());
        }
        iWndUpdate = EFalse;
    }

    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDCanvas::Draw ends", -1, -1);
        iStartUpTraceDone = ETrue;
    }
    DEBUG("CMIDCanvas::Draw --");
}
#else // !RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// From class CCoeControl.
// CMIDCanvas::Draw
// Drawing.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Draw(const TRect& aRect) const
{
    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDCanvas::Draw starts", -1, -1);
    }
    DEBUG("+ CMIDCanvas::Draw");

#ifdef CANVAS_DOUBLE_BUFFER
    ASSERT(iFrameBuffer);
#endif // CANVAS_DOUBLE_BUFFER

#ifdef CANVAS_ASYNC_PAINT
    TRect rect(aRect);

    rect.Intersection(iViewRect);
    rect.Move(-iViewRect.iTl);

    TSize size(rect.Size());

    TInt posPacked  = (rect.iTl.iX << 16) | (rect.iTl.iY);
    TInt sizePacked = (size.iWidth << 16) | (size.iHeight);

    PostEvent(EPaint, posPacked, sizePacked);
#else
#ifdef CANVAS_DOUBLE_BUFFER

// While changing the orientation method DrawWindow() not called,
// variable iWndUpdate is set to True. If iWndUpdate is True
// DrawWindow() is called from method Update()
// This is needed to avoid artifacting after orientation switch.
TBool landscape = Layout_Meta_Data::IsLandscapeOrientation();

if (iLandscape != landscape)
{
    iLandscape = landscape;
    iWndUpdate = ETrue;
}
else
{
#ifdef CANVAS_DIRECT_ACCESS
    // Check if DSA is ready and not used by some other client.
    if (iDirectGc && !iDirectContents.Count() && !DirectPaused())
    {
        DrawDirect(aRect);   // Draw directly to screen.
    }
    else
    {
#endif // CANVAS_DIRECT_ACCESS
        DrawWindow(aRect);   // Bitblt via window server.
#ifdef CANVAS_DIRECT_ACCESS
    }
#endif // CANVAS_DIRECT_ACCESS

    iWndUpdate = EFalse;
}
#endif // CANVAS_DOUBLE_BUFFER
#endif // CANVAS_ASYNC_PAINT

    if (!iStartUpTraceDone)
    {
        java::util::JavaOsLayer::startUpTrace("MIDP: CMIDCanvas::Draw ends", -1, -1);
        iStartUpTraceDone = ETrue;
    }
    DEBUG("- CMIDCanvas::Draw");
}
#endif // RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// CMIDCanvas::CMIDCanvas
// C++ constructor.
// ---------------------------------------------------------------------------
//
CMIDCanvas::CMIDCanvas(
    MMIDEnv& aEnv,
    MMIDComponent::TType aComponentType,
#ifdef CANVAS_DIRECT_ACCESS
    TBool /*aUpdateRequired*/
#else
    TBool aUpdateRequired
#endif // CANVAS_DIRECT_ACCESS
) :
        CCoeControl()
        ,iEnv(aEnv)
#ifdef CANVAS_DOUBLE_BUFFER
        ,iFrameBuffer(NULL)
#endif // CANVAS_DOUBLE_BUFFER
        ,iIsGameCanvas((
                           aComponentType == MMIDComponent::EGameCanvas ? ETrue : EFalse))
        ,iFlags(EPostKeyEvents)
        ,iFullScreen(EFalse)
        ,iScalingOn(EFalse)
        ,iS60SelectionKeyCompatibility(EFalse)
        ,iRestoreContentWhenUnfaded(EFalse)
        ,iLastFadeMessage(0)
#ifdef CANVAS_DIRECT_ACCESS
        ,iDcDsaToStart(EFalse)
#endif // CANVAS_DIRECT_ACCESS
        ,iDragEventsStartedInside(EFalse)
{
    DEBUG("+ CMIDCanvas::CMIDCanvas - EDirectEnabled");

#ifdef CANVAS_DIRECT_ACCESS
    iFlags |= (EDirectPaused | EDirectEnabled | EUpdateRequired);
#else

#if defined( __WINS__ ) || defined( __WINSCW__ )
    aUpdateRequired = ETrue;
#endif

    if (aUpdateRequired)
    {
        iFlags |= EUpdateRequired;
    }
#endif // CANVAS_DIRECT_ACCESS

    //Default values for PointerEventSuppressor
    iPESPointerMovementInTwips = CMIDUIManager::EPESPointerMovementInTwips;
    iPESTimeInMilliseconds = CMIDUIManager:: EPESTimeInMilliseconds;

    DEBUG("- CMIDCanvas::CMIDCanvas");
}


// ---------------------------------------------------------------------------
// CMIDCanvas::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ConstructL(CCoeControl& aWindow)
{
    SetContainerWindowL(aWindow);

    iFocusedComponent = KComponentFocusedNone;
    iPressedComponent = NULL;
    iOrgMIDletScrSize = TSize();
    iTargetMIDletScrSize = TSize();

#ifdef CANVAS_DOUBLE_BUFFER
    // Create a framebuffer large enough for full screen mode.
    // In normal mode we will waste the border pixels. This may be offset in
    // future by sharing the framebuffer between displayables.
    //
    // Allow dynamic changing of screen modes find the
    // largest available screen size to allocate a frame buffer big enough to
    // deal with all screen modes;
    CWsScreenDevice* device  = ControlEnv()->ScreenDevice();
    TInt noModes = device->NumScreenModes();
    TPixelsAndRotation pixelAndRot;
    TSize frameBufferSize(0, 0);

    for (TInt ii=0 ; ii < noModes; ++ii)
    {
        device->GetScreenModeSizeAndRotation(ii, pixelAndRot);

        if (frameBufferSize.iHeight < pixelAndRot.iPixelSize.iHeight)
        {
            frameBufferSize.iHeight = pixelAndRot.iPixelSize.iHeight;
        }

        if (frameBufferSize.iWidth < pixelAndRot.iPixelSize.iWidth)
        {
            frameBufferSize.iWidth = pixelAndRot.iPixelSize.iWidth;
        }
    }

    DEBUG_INT2("Canvas FrameBuffer Size: Width: %d,  Height %d",
               frameBufferSize.iWidth, frameBufferSize.iHeight);

    CreateFrameBufferL(frameBufferSize);

    if (iFrameBuffer)
    {
        iFrameDevice = CFbsBitmapDevice::NewL(iFrameBuffer);
        User::LeaveIfError(iFrameDevice->CreateContext(iFrameContext));
    }

    // Set iLandscape set as True according to landscape orientation mode
    // and initialize iWndUpdat. iWndUpdat variable notify us, when
    // orienation was changed
    iLandscape = Layout_Meta_Data::IsLandscapeOrientation();
    iWndUpdate = EFalse;

#endif // CANVAS_DOUBLE_BUFFER

    SetScalingFactors();

    if (iScalingOn)
    {
        // We need iScaler only when scaling is on.
        iScaler = iDisplayable->GetUIManager()->OpenScalerL();
    }

#ifdef CANVAS_DIRECT_ACCESS
    ResumeDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

    if (IsNetworkIndicatorEnabledL())
    {
        iNetworkIndicator = CMIDNetworkIndicator::NewL(this);

        // Initialize and set active CallIndicator to receive notification
        // when voiceline status changes.
        iCallIndicator = CMIDCallIndicator::NewL(iNetworkIndicator);
        iCallIndicator->SetActiveLocal();
    }

    iKeyDecoder = iDisplayable->GetUIManager()->OpenKeyDecoderL();
    ASSERT(iKeyDecoder);

    // Canvas listens Media key events if CMIDRemConObserver is initialized
    // and media keys keys are enabled.
    iRemConObserver = iKeyDecoder->GetRemConObserver();
    if (iRemConObserver && iKeyDecoder->MediaKeysEnabled())
    {
        iRemConObserver->AddMediaKeysListenerL(this);
    }

    // Set S60 Selection Key Compatibility to provide MSK key events to
    // canvas.
    iS60SelectionKeyCompatibility = iEnv.MidletAttributeIsSetToVal(
                                        LcduiMidletAttributes::KAttribS60SelectionKeyCompatibility,
                                        LcduiMidletAttributeValues::KTrueValue);

    if (iDisplayable)
    {
        iDisplayable->SetS60SelectionKeyCompatibility(
            iS60SelectionKeyCompatibility);
    }

#ifdef RD_TACTILE_FEEDBACK
    // Create a CMIDTactileFeedbackExtension in case tactile feedback is
    // supported.
    iTactileFeedback = new(ELeave) CMIDTactileFeedbackExtension(this, 1);
#endif // RD_TACTILE_FEEDBACK

    //Create PointerEventSuppressor with default values or JAD parameter defined values
    iPointerEventSuppressor = CAknPointerEventSuppressor::NewL();
    TInt pointerMovementInPixels =
        ControlEnv()->ScreenDevice()->HorizontalTwipsToPixels(iPESPointerMovementInTwips);
    TSize suppressorValues =
        iDisplayable->GetUIManager()->ReadPointerEventSuppressorValues();
    if (!(suppressorValues.iWidth == KPESErrorValue &&
            suppressorValues.iHeight == KPESErrorValue))
    {
        iPESTimeInMilliseconds = suppressorValues.iHeight;
        pointerMovementInPixels =
            ControlEnv()->ScreenDevice()->HorizontalTwipsToPixels(suppressorValues.iWidth);
    }
    iPointerEventSuppressor->SetMaxTapDuration(iPESTimeInMilliseconds * 1000);
    iPointerEventSuppressor->SetMaxTapMove(TSize(pointerMovementInPixels,
                                           pointerMovementInPixels));

#ifdef RD_JAVA_NGA_ENABLED
    iForeground = EFalse;
    iEglDisplay = EGL_NO_DISPLAY;
    iEglWindowSurface = EGL_NO_SURFACE;
    iEglWindowSurfaceContext = EGL_NO_CONTEXT;
    iEglPbufferSurface = EGL_NO_SURFACE;
    iEglPbufferSurfaceContext = EGL_NO_CONTEXT;
    iPbufferTexture = 0;

    i3DAccelerated =
        iEnv.IsHardwareAcceleratedL(MMIDEnv::EHardware3D) > 0;

    InitPixelSourceL();
#endif // RD_JAVA_NGA_ENABLED        
}


// ---------------------------------------------------------------------------
// CMIDCanvas::SetContainerWindowL
// Set the container window.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SetContainerWindowL(const CCoeControl& aWindow)
{
#ifdef CANVAS_DIRECT_ACCESS
    StopDirectAccess();

    if (iDirectAccess)
    {
        delete iDirectAccess;
        iDirectAccess = NULL;
    }
#endif // CANVAS_DIRECT_ACCESS

    iDisplayable = (CMIDDisplayable*) &aWindow;
    CCoeControl::SetContainerWindowL(aWindow);
    Window().SetBackgroundColor();

#ifdef RD_SCALABLE_UI_V2
    Window().SetPointerGrab(ETrue);
#endif // RD_SCALABLE_UI_V2

#ifdef CANVAS_DIRECT_ACCESS
    if (DirectEnabled())
    {
        RWsSession& session = ControlEnv()->WsSession();
        CWsScreenDevice* device = ControlEnv()->ScreenDevice();

        iDirectAccess = CDirectScreenAccess::NewL(
                            session, *device, *DrawableWindow(), *this);
    }
#endif // CANVAS_DIRECT_ACCESS

    // Position in screen co-ordinates is first time available when container
    // window is set.
    iPositionRelativeToScreen = PositionRelativeToScreen();
}


// ---------------------------------------------------------------------------
// CMIDCanvas::SendKeyEventToJavaSideL
// Send key event to Java side.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SendKeyEventToJavaSideL(
    const TKeyEvent& aEvent, TEventCode aType)
{
    DEBUG("+ CMIDCanvas::SendKeyEventToJavaSideL");
    TMIDKeyEvent event;

    // See if the key can be sent java side.
    DEBUG_INT2("CMIDCanvas::PostKeyEvent - SOS code %d SOS type %d",
               aEvent.iCode,
               aType);

    // map key event data
    TKeyEvent wsEvent = aEvent;

    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - received TKeyEvent: iCode = %d", aEvent.iCode);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - received TKeyEvent: iModifiers = %d", aEvent.iModifiers);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - received TKeyEvent: iRepeats = %d", aEvent.iRepeats);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - received TKeyEvent: iScanCode = %d", aEvent.iScanCode);

    iEnv.MappingDataForKey(wsEvent, aType);

    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - mapped TKeyEvent: iCode = %d", aEvent.iCode);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - mapped TKeyEvent: iModifiers = %d", aEvent.iModifiers);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - mapped TKeyEvent: iRepeats = %d", aEvent.iRepeats);
    DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - mapped TKeyEvent: iScanCode = %d", aEvent.iScanCode);

    //See if the key can be sent java side.
    if (iEnv.TranslateKeyL(event, wsEvent, aType))
    {
        DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - translated TMIDKeyEvent: iEvents = %d", event.iEvents);
        DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - translated TMIDKeyEvent: iKeyCode = %d", event.iKeyCode);
        DEBUG_INT("+ CMIDCanvas::SendKeyEventToJavaSideL - translated TMIDKeyEvent: iRepeats = %d", event.iRepeats);

        ASSERT(iKeyDecoder);

        TInt gameAction = iKeyDecoder->GameAction(event.iKeyCode);

        if (0 != gameAction)
        {
            if (event.iEvents & TMIDKeyEvent::EPressed)
            {
                GamePress(gameAction);
            }

            if (event.iEvents & TMIDKeyEvent::EReleased)
            {
                GameRelease(gameAction);
            }
        }

        if (PostKeyEvents() || (0 == gameAction))
        {
            DEBUG_INT2(
                "CMIDCanvas::PostKeyEvent - JAVA code %d JAVA type %d",
                event.iKeyCode,
                event.iEvents);

            // 9-way Navigation Support
            // Send two arrow events generated when press diagonal direction
            switch (event.iKeyCode)
            {
                //KeyLeftUpArrow
            case EKeyLeftUpArrow:
            {
                TMIDKeyEvent pevent;
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyUpArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyLeftArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                break;
            }
            //KeyRightUpArrow
            case EKeyRightUpArrow:
            {
                TMIDKeyEvent pevent;
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyUpArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyRightArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                break;
            }
            //KeyRightDownArrow
            case EKeyRightDownArrow:
            {
                TMIDKeyEvent pevent;
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyDownArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyRightArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                break;
            }
            //KeyLeftDownArrow
            case EKeyLeftDownArrow:
            {
                TMIDKeyEvent pevent;
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyDownArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                pevent.iKeyCode = iKeyDecoder->MapNonUnicodeKey(EStdKeyLeftArrow);
                pevent.iEvents = event.iEvents;
                pevent.iRepeats = event.iRepeats;
                iEnv.PostKeyEvent(*this, pevent);
                break;
            }
            default:
                iEnv.PostKeyEvent(*this, event);
            }
        }
    }
    DEBUG("- CMIDCanvas::SendKeyEventToJavaSideL");
}


#ifdef CANVAS_DOUBLE_BUFFER
// ---------------------------------------------------------------------------
// CMIDCanvas::CreateFrameBufferL
// Create frame buffer.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::CreateFrameBufferL(const TSize& aSize)
{
    DEBUG_INT2("+ CMIDCanvas::CreateFrameBufferL( %D, %D)",
               aSize.iWidth,
               aSize.iHeight);

    iFrameBuffer = iEnv.ReserveCanvasFrameBufferL((*this), aSize);

    DEBUG("- CMIDCanvas::CreateFrameBufferL");
}
#endif // CANVAS_DOUBLE_BUFFER


#ifdef DEFER_BITBLT
// ---------------------------------------------------------------------------
// CMIDCanvas::BitBltCycles
// Get BitBlt cycles count.
// ---------------------------------------------------------------------------
//
TInt CMIDCanvas::BitBltCycles(const TSize& aSize) const
{
    TInt cycles = (aSize.iWidth * aSize.iHeight * KEstCyclesPerPixel);

    if (cycles < KMinOpCycles) cycles = KMinOpCycles;
    if (cycles > KMaxOpCycles) cycles = KMaxOpCycles;

    return cycles;
}
#endif // DEFER_BITBLT


// ---------------------------------------------------------------------------
// CMIDCanvas::Layout
// Layout Canvas control.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::Layout()
{
    DEBUG("+ CMIDCanvas::Layout");
#ifdef CANVAS_DIRECT_ACCESS
    StopDirectAccess();
#endif // CANVAS_DIRECT_ACCESS

    TRect rect(Rect());
    TSize size(rect.Size());
    TSize viewSize;

    if (iFullScreen)
    {
#ifdef RD_JAVA_NGA_ENABLED
        if (iScalingOn)
        {
            // Translate to screen coordinates.
            rect.Move(iPositionRelativeToScreen);
        }
#endif // RD_JAVA_NGA_ENABLED

        if (iNetworkIndicator)
        {
            // Refresh the layout data for network indicator.
            iNetworkIndicator->GetLayoutData();
        }

        // Get original MIDlet size which eventually reflects display
        // orientation.
        TSize orientedOrgMIDletScrSize(OrientedOrgMIDletScrSize());

        // If Nokia-MIDlet-Target-Display-Size is defined, Canvas will be
        // scaled to that size.
        if (iTargetMIDletScrSize != KZeroSize)
        {
            viewSize = iTargetMIDletScrSize;
        }

        // If optional JAD parameter Nokia-MIDlet-Target-Display-Size is NOT
        // defined and Nokia-MIDlet-Original-Display-Size is defined to
        // smaller size than the device's screen size, we will scale the
        // Canvas to fit the screen without changing aspect ratio.
        // If the aspect ratio of Nokia-MIDlet-Original-Display-Size is not
        // same as the display has, we need to leave black borders to both
        // sides or up and down.
        else if (
            (orientedOrgMIDletScrSize.iWidth != 0) &&
            (orientedOrgMIDletScrSize.iHeight != 0))
        {
            TReal widthScalingFactor =
                (TReal)size.iWidth /
                (TReal)orientedOrgMIDletScrSize.iWidth;

            TReal heightScalingFactor =
                (TReal)size.iHeight /
                (TReal)orientedOrgMIDletScrSize.iHeight;

            // Aspect ratio of MIDlet is same as display.
            if (widthScalingFactor == heightScalingFactor)
            {
                viewSize.SetSize(size.iWidth, size.iHeight);
            }
            // Aspect ratio of MIDlet is "wider" than display.
            else if (widthScalingFactor < heightScalingFactor)
            {
                viewSize.SetSize(size.iWidth,
                                 widthScalingFactor * iContentSize.iHeight);
            }
            //Aspect ratio of MIDlet is "narrower" than display.
            else
            {
                viewSize.SetSize(heightScalingFactor * iContentSize.iWidth,
                                 size.iHeight);
            }
        }
        // If JAD parameters for graphics scaling are not defined, Canvas
        // won't be scaled.
        else
        {
            viewSize = iContentSize;
        }
    }
    // If Canvas is not in full screen mode, it won't be scaled.
    else
    {
        viewSize = iContentSize;
    }

    TSize border = (size - viewSize);
    TBool center = ((border.iWidth != 0) || (border.iHeight != 0));
    TPoint viewPos = rect.iTl;

    // Compute position of view rect in window.
    //
    // If content does not fill the control, center it, but try
    // to keep the left hand pixel of content aligned to a word
    // boundary of screen memory to enable faster blts.
    if (center)
    {
        viewPos.iX += border.iWidth  / 2;
        viewPos.iY += border.iHeight / 2;

        const TPoint screenPos = PositionRelativeToScreen();

        // Align to 4-pixel boundary - this is overkill but covers both
        // EColor256 and EColor4K. Should query graphics plugin for the
        // alignment.
        const TInt KAlignBoundary = 4;
        TInt alignment = KAlignBoundary
                         - ((viewPos.iX + screenPos.iX) & 0x3);

        // This alignment change is made to keep canvas as more center
        // as it is possible. Alignment can have values between 0 and 4
        // so if it have value more than 2, there is a -4 subtraction
        // to prevent moving canvas too much to right side of display.
        if (alignment > 2)
        {
            alignment -= KAlignBoundary;
        }

        viewPos.iX += alignment;

        // Fallback to original position - we'll just have to take the
        // unaligned bitblt performance hit.
        if (viewPos.iX < rect.iTl.iX ||
                (viewPos.iX + viewSize.iWidth) > rect.iBr.iX)
        {
            viewPos.iX -= alignment;
        }
    }

    iViewRect = TRect(viewPos, viewSize);

#ifdef CANVAS_DIRECT_ACCESS
    StartDirectAccess();
#endif // CANVAS_DIRECT_ACCESS
#ifdef RD_JAVA_NGA_ENABLED
    // To avoid situation when Orientation was changed and black screen is shown
    TRAPD(err, UpdateL(iViewRect));
    if (err != KErrNone)
    {
        DEBUG_INT("CMIDCanvas::Layout - update error %d", err);
    }
#endif // RD_JAVA_NGA_ENABLED

    TInt contentsCount(iDirectContents.Count());

    for (TInt j = 0; j < contentsCount; j++)
    {
        iDirectContents[j]->MdcContentBoundsChanged(
            TRect(PositionRelativeToScreen(), Size()));
    }

    DEBUG_PROFILE("CMIDCanvas::Layout Canvas up and running \n");
    DEBUG("- CMIDCanvas::Layout");
}


#ifdef CANVAS_DIRECT_ACCESS
// ---------------------------------------------------------------------------
// CMIDCanvas::DirectEnabled
// Tells whether Direct Screen Access is enabled or disabled.
// ---------------------------------------------------------------------------
//
inline TBool CMIDCanvas::DirectEnabled() const
{
    return iFlags & EDirectEnabled;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::DirectPaused
// Tells whether Direct Screen Access is paused or not.
// ---------------------------------------------------------------------------
//
inline TBool CMIDCanvas::DirectPaused() const
{
    return iFlags & EDirectPaused;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::UpdateRequired
// Tells whether update is required or not.
// ---------------------------------------------------------------------------
//
inline TBool CMIDCanvas::UpdateRequired() const
{
    return iFlags & EUpdateRequired;
}
#endif //CANVAS_DIRECT_ACCESS


// ---------------------------------------------------------------------------
// CMIDCanvas::PostKeyEvents
// Tells whether posting of key events is enabled or disabled.
// ---------------------------------------------------------------------------
//
inline TBool CMIDCanvas::PostKeyEvents() const
{
    return iFlags & EPostKeyEvents;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::GamePress
// Process game press action.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::GamePress(TInt aGameAction)
{
    const TInt gameBit = 1 << aGameAction;
    iGameKeyState |= gameBit;
    iGameKeyLatch |= iGameKeyState;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::GameRelease
// Process game release action.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::GameRelease(TInt aGameAction)
{
    const TInt gameBit = 1 << aGameAction;
    iGameKeyState &= ~ gameBit;
    iGameKeyLatch |= iGameKeyState;
}


// ---------------------------------------------------------------------------
// CMIDCanvas::SetScalingFactors
// Sets MIDlet's original size and target size resolutions if those are
// defined in JAD or manifest. If attribute is missing or it's not defined
// correctly, the value will be 0,0. This value is later used as an "error
// value".
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SetScalingFactors()
{
    CMIDMenuHandler* menuHandler = iDisplayable->MenuHandler();

    ASSERT(menuHandler);

    iScalingOn = menuHandler->IsScalingEnabled();

    iOrgMIDletScrSize = menuHandler->GetScalingParameterOrgMIDletScrSize();

    iTargetMIDletScrSize =
        menuHandler->GetScalingParameterTargetMIDletScrSize();

    iScaleMIDletOnOrientSwitch =
        menuHandler->GetScalingParameterScaleMIDletOnOrientSwitch();
}


// ---------------------------------------------------------------------------
// CMIDCanvas::OrientedOrgMIDletScrSize
// Returns original MIDlet screen size with regards to display orientation.
// ---------------------------------------------------------------------------
//
TSize CMIDCanvas::OrientedOrgMIDletScrSize() const
{
    // Ensure that scaling and scaling-on-orientation-switch is enabled.
    if (iScalingOn && iScaleMIDletOnOrientSwitch)
    {
        TSize orientedSize;

        // Ensure that original size is defined as portrait.
        if (iOrgMIDletScrSize.iWidth < iOrgMIDletScrSize.iHeight)
        {
            orientedSize = iOrgMIDletScrSize;
        }
        else
        {
            orientedSize.iWidth = iOrgMIDletScrSize.iHeight;
            orientedSize.iHeight = iOrgMIDletScrSize.iWidth;
        }

        // Change original size orientation if actuall display orientation
        // is landscape.
        if (Layout_Meta_Data::IsLandscapeOrientation())
        {
            TInt swap = orientedSize.iWidth;
            orientedSize.iWidth = orientedSize.iHeight;
            orientedSize.iHeight = swap;
        }

        return orientedSize;
    }
    else
    {
        return iOrgMIDletScrSize;
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::IsNetworkIndicatorEnabledL
// Check if the network indicator should be shown in Canvas.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::IsNetworkIndicatorEnabledL() const
{
    if (RProcess().SecureId().iId != 0x102033E6)
    {
        // For standalone type apps we don't show indicator.
        return EFalse;
    }
    TBool enabled = ETrue; // Return ETrue by default.

    std::auto_ptr<java::storage::JavaStorage> js(java::storage::JavaStorage::createInstance());
    java::storage::JavaStorageApplicationEntry_t entries;
    try
    {
        js->open();
        java::util::Uid uid;
        TUidToUid(iEnv.MidletSuiteUid(), uid);
        js->read(java::storage::MIDP_PACKAGE_TABLE, uid, entries);
        js->close();
    }
    catch (java::storage::JavaStorageException& ex)
    {
        DEBUG_INT("CMIDCanvas::IsNetworkIndicatorEnabledL: JavaStorage error: \
            reading MIDP_PACKAGE_TABLE failed, error code = %D", ex.mStatus);
    }
    java::storage::JavaStorageEntry attribute;
    attribute.setEntry(java::storage::SECURITY_DOMAIN_CATEGORY, L"");
    java::storage::JavaStorageApplicationEntry_t::const_iterator findIterator = entries.find(attribute);
    std::wstring res = L"";
    if (findIterator != entries.end())
    {
        res = (*findIterator).entryValue();
    }
    entries.clear();

    if (res == MANUFACTURER_DOMAIN_CATEGORY)
    {
        enabled = EFalse;
    }
    else if (res == OPERATOR_DOMAIN_CATEGORY)
    {
        // Read the central repository key to find out if the
        // network indicator should be shown for midlets in
        // operator domain.
        CRepository* repository = NULL;
        TRAPD(err, repository = CRepository::NewL(KCRUidMidpLcdui));

        if (err == KErrNone)
        {
            err = repository->Get(KShowCanvasNetIndicatorInOperatorDomain, enabled);
            if (err != KErrNone)
            {
                DEBUG_INT("CMIDCanvas::IsNetworkIndicatorEnabledL: \
                    reading CenRep key failed, error code = %D", err);
            }
        }

        delete repository;
        repository = NULL;
    }

    return enabled;
}

#ifdef RD_JAVA_NGA_ENABLED

// ---------------------------------------------------------------------------
// CMIDCanvas::HandleForeground
// Relases resources in graphics HW (=pixel source or EGL resources)
// when going to background.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::HandleForeground(TBool aForeground)
{
    DEBUG_INT("CMIDCanvas::HandleForeground(%d) ++", aForeground);
    iForeground = aForeground;

    if (!iForeground)
    {
        if (IsEglAvailable())
        {
            if (iEglOccupied)
            {
                DEBUG("CMIDCanvas::HandleForeground() - egl - pending dispose");
                iEglPendingDispose = ETrue;
            }
            else
            {
                CloseEgl();
            }
        }

        SuspendPixelSource();
    }
    DEBUG("CMIDCanvas::HandleForeground --");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::InitPixelSourceL()
// ---------------------------------------------------------------------------
//
void CMIDCanvas::InitPixelSourceL()
{
    if (iAlfCompositionPixelSource)
    {
        return;
    }
    iPixelSourceSuspended = ETrue;
    TBool modeSupported = EFalse;
    switch (iFrameBuffer->DisplayMode())
    {
    case EColor16MU:
        modeSupported = ETrue;
        iAlfBufferAttributes.iFormat =
            MAlfBufferProvider::ESourceFormatXRGB_8888;
        break;
    case EColor16MA:
        modeSupported = ETrue;
        iAlfBufferAttributes.iFormat =
            MAlfBufferProvider::ESourceFormatARGB_8888;
        break;
    case EColor16MAP:
        modeSupported = ETrue;
        iAlfBufferAttributes.iFormat =
            MAlfBufferProvider::ESourceFormatARGB_8888_PRE;
        break;
    default:
        break;
    }

    if (modeSupported)
    {
        iAlfCompositionPixelSource = CAlfCompositionPixelSource::NewL(*this, &Window());

        iAlfBufferAttributes.iWidth  = iContentSize.iWidth;
        iAlfBufferAttributes.iHeight = iContentSize.iHeight;
        iAlfBufferAttributes.iStride =  iAlfBufferAttributes.iWidth * KBytesPerPixel;
        iAlfBufferAttributes.iAlignment = 32;
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::DisposePixelSource
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DisposePixelSource()
{
    DEBUG("CMIDCanvas::DisposePixelSource ++");
    NotifyMonitor();
    if (iAlfCompositionPixelSource)
    {
        DEBUG("CMIDCanvas::DisposePixelSource - delete alf");
        iAlfCompositionPixelSource->Suspend();
        delete iAlfCompositionPixelSource;
        iAlfCompositionPixelSource = NULL;
    }
    DEBUG("CMIDCanvas::DisposePixelSource --");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::ActivatePixelSourceL
// In scaling case need to call SetExtent() again if pixel source was suspended.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ActivatePixelSourceL()
{
    ASSERT(iAlfCompositionPixelSource);

    if (iDirectContents.Count() > 0)
    {
        return;
    }

    iAlfCompositionPixelSource->ActivateSyncL();
    iFrameReady = ETrue;

    if (iPixelSourceSuspended)
    {
        iPixelSourceSuspended = EFalse;
        if (iFullScreen && iScalingOn)
        {
            iAlfCompositionPixelSource->SetExtent(iViewRect, KPhoneScreen);
        }
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::SuspendPixelSource
// ---------------------------------------------------------------------------
//
void CMIDCanvas::SuspendPixelSource()
{
    NotifyMonitor();
    if (iAlfCompositionPixelSource)
    {
        iAlfCompositionPixelSource->Suspend();
        iPixelSourceSuspended = ETrue;
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::NotifyMonitor
// CMIDCanvas::ProcessL() is asyncronous when CAlfCompositionPixelSource is used.
// This notiCMIDBuffer's monitor so that waiting Java thread is resumed.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::NotifyMonitor()
{
    if (iAlfMonitor)
    {
        iAlfMonitor->notify();
        iAlfMonitor = NULL;
    }
    iFrameReady = EFalse;
}

// ---------------------------------------------------------------------------
// From MAlfBufferProvider
// CMIDCanvas::ProduceNewFrameL
// Callback from CAlfCompositionPixelSource. Copy RGB data from framebuffer
// to pixel source's buffer. Return ETrue if we have a frame ready,
// EFalse otherwise.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::ProduceNewFrameL(const TRegion& aVisibleRegion, TUint8*& aBuffer)
{
    DEBUG_INT("CMIDCanvas::ProduceNewFrameL (thread=%d) ++", RThread().Id().Id());
    TRect bbox(aVisibleRegion.BoundingRect());
    DEBUG_INT4("CMIDCanvas::ProduceNewFrameL - visible region(x=%d, y=%d, w=%d, h=%d) ++",
               bbox.iTl.iX, bbox.iTl.iY, bbox.Width(), bbox.Height());

    // If iDirectContents.Count() > 0, canvas hasn't received
    // MdcNotifyContentAdded in LCDUI thread yet.
    if (!iFrameReady || iDirectContents.Count() > 0)
    {
        DEBUG("CMIDCanvas::ProduceNewFrameL - FRAME IS NOT READY --");
        NotifyMonitor();
        return EFalse;
    }

    NotifyMonitor();

    TUint8* from = (TUint8*)iFrameBuffer->DataAddress();

    TBool downScaling = IsDownScaling(iContentSize, iViewRect);
    TInt width =  downScaling ? iViewRect.Width()  : iContentSize.iWidth;
    TInt height = downScaling ? iViewRect.Height() : iContentSize.iHeight;

    TUint bytes = width * KBytesPerPixel;
    TInt scanLength = CFbsBitmap::ScanLineLength(
                          iFrameBuffer->SizeInPixels().iWidth, iFrameBuffer->DisplayMode());

    for (TInt y = 0; y < height; ++y)
    {
        Mem::Copy(aBuffer, from, bytes);
        aBuffer += iAlfBufferAttributes.iStride;
        from += scanLength;
    }

    DEBUG("CMIDCanvas::ProduceNewFrameL --");
    return ETrue;
}

// ---------------------------------------------------------------------------
// From MAlfBufferProvider
// CMIDCanvas::BufferAttributes
// Called by CAlfCompositionPixelSource to get the size of RGB data.
// ---------------------------------------------------------------------------
//
MAlfBufferProvider::TBufferCreationAttributes& CMIDCanvas::BufferAttributes()
{
    DEBUG_INT2("CMIDCanvas::BufferAttributes - iContentSize(w=%d, h=%d) ++",
               iContentSize.iWidth, iContentSize.iHeight);

    TBool downScaling = IsDownScaling(iContentSize, iViewRect);
    iAlfBufferAttributes.iWidth  = downScaling ? iViewRect.Width() : iContentSize.iWidth;
    iAlfBufferAttributes.iHeight = downScaling ? iViewRect.Height() : iContentSize.iHeight;

    iAlfBufferAttributes.iStride = iAlfBufferAttributes.iWidth * KBytesPerPixel;
    return iAlfBufferAttributes;
}

// ---------------------------------------------------------------------------
// From MAlfBufferProvider
// CMIDCanvas::ContextAboutToSuspend
// Notification from Alf.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ContextAboutToSuspend()
{
    DEBUG("CMIDCanvas::ContextAboutToSuspend() ++");
    NotifyMonitor();
    iPixelSourceSuspended = ETrue;
    DEBUG("CMIDCanvas::ContextAboutToSuspend() --");
}

// ---------------------------------------------------------------------------
// From MAlfBufferProvider
// CMIDCanvas::OnActivation
// Notification from Alf.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::OnActivation()
{
    DEBUG("CMIDCanvas::OnActivation()++--");
}


// ---------------------------------------------------------------------------
// CMIDCanvas::OpenEglL
// Creates EGL window surface and context. Only 32 bit display modes are
// supported.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::OpenEglL()
{
    if (IsEglAvailable())
    {
        return;
    }

    DEBUG("CMIDCanvas::OpenEglL() ++");

    if (iEglDisplay == EGL_NO_DISPLAY)
    {
        iEglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        // Initializing an already initialized display has no effect.
        // eglTerminate is not called in LCDUI side ever, only by M3G
        if (eglInitialize(iEglDisplay, NULL, NULL) == EGL_FALSE)
        {
            ELOG1(EJavaUI, "eglInitialize() failed, eglError=%d", eglGetError());
            User::Leave(KErrNotSupported);
        }
    }

    RWindow& window = Window();
    // Choose the buffer size based on the Window's display mode.
    TDisplayMode displayMode = window.DisplayMode();
    TInt bufferSize = 0;
    switch (displayMode)
    {
    case(EColor16MU):
    case(EColor16MA):
    case(EColor16MAP):
        bufferSize = 32;
        break;
    default:
        ELOG1(EJavaUI, "CMIDCanvas::OpenEglL(): unsupported window display mode %d",
              displayMode);
        User::Leave(KErrNotSupported);
        break;
    }

    EGLint surfaceType = EGL_WINDOW_BIT | EGL_SWAP_BEHAVIOR_PRESERVED_BIT;

    const EGLint attributeList[] =
    {
        EGL_SURFACE_TYPE, surfaceType,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_BUFFER_SIZE, bufferSize,
        EGL_TRANSPARENT_TYPE, EGL_NONE,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLint numConfigs = 0;

    // Choose the best EGLConfig that matches the desired properties.
    if ((eglChooseConfig(iEglDisplay, attributeList, &iEglConfig, 1, &numConfigs) == EGL_FALSE) ||
            numConfigs == 0)
    {
        ELOG1(EJavaUI, "eglChooseConfig() failed, eglError=%d", eglGetError());
        User::Leave(KErrNotSupported);
    }

    // Check that surface content is preserved in swap, otherwise the
    // game canvas content cannot be preserved.
    eglGetConfigAttrib(iEglDisplay, iEglConfig, EGL_SURFACE_TYPE, &surfaceType);
    if (surfaceType & EGL_SWAP_BEHAVIOR_PRESERVED_BIT == 0)
    {
        ELOG(EJavaUI, "Egl config does not support EGL_SWAP_BEHAVIOR_PRESERVED_BIT");
        User::Leave(KErrNotSupported);
    }

    // Create a window surface where the graphics are blitted.
    // Pass the Native Window handle to egl.
    iEglWindowSurface = eglCreateWindowSurface(iEglDisplay, iEglConfig, (void*)&window, NULL);
    if (iEglWindowSurface == EGL_NO_SURFACE)
    {
        ELOG1(EJavaUI, "eglCreateWindowSurface() failed, eglError=%d", eglGetError());
        User::Leave(KErrNoMemory);
    }
    // Need to preserve surface content after swapping
    eglSurfaceAttrib(iEglDisplay, iEglWindowSurface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED);

    CreateEglContext();
    ClearEglSurface(EEglWindow);

    // Allocate memory for ARGB->RGBA conversion needed by
    // openGL texture
    if (!iTexturePixels)
    {
        iTexturePixels = new(ELeave) TUint8[KMaxBlitSize * KMaxBlitSize * KBytesPerPixel];
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    if (!iVertexArray)
    {
        iVertexArray = new(ELeave) GLshort[8];
    }
    if (!iTriangleStrip)
    {
        iTriangleStrip = new(ELeave) GLubyte[4];
        iTriangleStrip[0] = 0;
        iTriangleStrip[1] = 1;
        iTriangleStrip[2] = 3;
        iTriangleStrip[3] = 2;
    }
    DEBUG("CMIDCanvas::OpenEglL() --");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::CloseEgl
// Destroys EGL contexts and surfaces.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::CloseEgl()
{
    DEBUG("CMIDCanvas::CloseEglL() ++");

    iM3GContent = EFalse;
    iM3GStart = EFalse;
    iEglPendingResize = EFalse;
    iEglPendingDispose = EFalse;

    if (iEglDisplay == EGL_NO_DISPLAY)
    {
        return;
    }

    // MIDlet might draw only 2D after this =>
    // need to set frame buffer opaque to be compatible with
    // blending methods in Lcdgd. UpdateOffScreenBitmapL() does this
    // for GameCanvas.
    if (!IsGameCanvas() && iFrameContext)
    {
        iFrameContext->SetBrushColor(KOpaqueClearColor);
        iFrameContext->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iFrameContext->Clear();
    }

    // Take a snapshot from window surface to the frame buffer.
    TRAPD(err, UpdateOffScreenBitmapL(EFalse));
    if (err != KErrNone)
    {
        DEBUG("CMIDCanvas::CloseEgl() - UpdateOffScreenBitmapL() failed");
    }

    // make sure the we have no current target
    // so that surfaces and contexts are destoyed immediately
    SetCurrentEglType(EEglNone);

    // If pbuffer has been created dispose that here
    if (iEglPbufferSurfaceContext != EGL_NO_CONTEXT ||
            iEglPbufferSurface != EGL_NO_SURFACE)
    {
        DisposePBufferSurface();
    }

    if (iEglWindowSurfaceContext != EGL_NO_CONTEXT)
    {
        eglDestroyContext(iEglDisplay, iEglWindowSurfaceContext);
        iEglWindowSurfaceContext = EGL_NO_CONTEXT;
    }
    if (iEglWindowSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(iEglDisplay, iEglWindowSurface);
        iEglWindowSurface = EGL_NO_SURFACE;
    }

    DEBUG("CMIDCanvas::CloseEglL() --");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::CreateEglContext
// (Re)create EGL context
// ---------------------------------------------------------------------------
//
void CMIDCanvas::CreateEglContext()
{
    if (iEglWindowSurfaceContext != EGL_NO_CONTEXT)
    {
        SetCurrentEglType(EEglNone);
        eglDestroyContext(iEglDisplay, iEglWindowSurfaceContext);
        iEglWindowSurfaceContext = EGL_NO_CONTEXT;
    }
    // Create a rendering context
    iEglWindowSurfaceContext = eglCreateContext(iEglDisplay, iEglConfig, EGL_NO_CONTEXT, NULL);
    if (iEglWindowSurfaceContext == EGL_NO_CONTEXT)
    {
        ELOG1(EJavaUI, "eglCreateContext() failed, eglError=%d", eglGetError());
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::SetCurrentEglType
// Sets the current EGL context and surface.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::SetCurrentEglType(TEglType aType)
{
    EGLBoolean res = EGL_TRUE;
    if (aType == EEglPbuffer)
    {
        res = eglMakeCurrent(
                  iEglDisplay, iEglPbufferSurface, iEglPbufferSurface, iEglPbufferSurfaceContext);
    }
    else if (aType == EEglWindow)
    {
        res = eglMakeCurrent(
                  iEglDisplay, iEglWindowSurface, iEglWindowSurface, iEglWindowSurfaceContext);
    }
    else
    {
        res = eglMakeCurrent(
                  iEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
    if (res == EGL_FALSE)
    {
        ELOG1(EJavaUI, "eglMakeCurrent() failed, eglError=%s", GetEglError(eglGetError()).Ptr());
    }
    return (res == EGL_FALSE ? EFalse : ETrue);
}

// ---------------------------------------------------------------------------
// CMIDCanvas::GetCurrentEgl
// Gets the current EGL type
// ---------------------------------------------------------------------------
//
CMIDCanvas::TEglType CMIDCanvas::GetCurrentEglType()
{
    EGLSurface surface = eglGetCurrentSurface(EGL_READ);
    if (surface == EGL_NO_SURFACE)
    {
        return EEglNone;
    }
    else if (surface == iEglPbufferSurface)
    {
        return EEglPbuffer;
    }
    else
    {
        return EEglWindow;
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::GetEglSurfaceSize
// Returns size of the EGL surface.
// ---------------------------------------------------------------------------
//
TSize CMIDCanvas::GetEglSurfaceSize(EGLSurface aSurface)
{
    TSize sz;
    if (aSurface == EGL_NO_SURFACE)
    {
        aSurface = iEglWindowSurface;
        if (iScalingOn && iFullScreen)
        {
            aSurface = iEglPbufferSurface;
        }
    }
    if (!(eglQuerySurface(iEglDisplay, aSurface, EGL_WIDTH, &sz.iWidth) &&
            eglQuerySurface(iEglDisplay, aSurface, EGL_HEIGHT, &sz.iHeight)))
    {
        sz.SetSize(0, 0);
    }
    DEBUG_INT2("CMIDCanvas::GetEglSurfaceSizeL() - w=%d, h=%d", sz.iWidth, sz.iHeight);
    return sz;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::HandleSizeChanged()
// EGL cannot handle window resize upwards and in this case we need to
// recreate the surface. If recreation is needed (and M3G is not using our
// surface) EGL context and surface are destoryed here and recreated in
// next ProcessL() when M3G_CONTENT_START is recieved from Java side.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::HandleSizeChanged()
{
    DEBUG("CMIDCanvas::HandleSizeChanged ++");

    SuspendPixelSource();

    if (IsEglAvailable())
    {
        TSize surfaceSize = GetEglSurfaceSize(iEglWindowSurface);
        TSize controlSize = Size();
        if (surfaceSize.iHeight < controlSize.iHeight ||
                surfaceSize.iWidth < controlSize.iWidth)
        {
            // Check if egl surface is currently occupied.
            if (iEglOccupied)
            {
                // Delayed resizing. It is done when the ReleaseEglSurface method
                // is called.
                DEBUG("CMIDCanvas::HandleSizeChanged - egl - resize pending");
                iEglPendingResize = ETrue;
            }
            else
            {
                DEBUG("CMIDCanvas::SizeChanged - close egl");
                // Surface recreation is done in next ProcessL() call
                CloseEgl();
            }
        }
    }
    DEBUG("CMIDCanvas::HandleSizeChanged --");
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::UpdateEglContent
// Renders current 2D content in frame buffer as an OpenGL texture to
// EGL surface. If scaling is on, renders to pbuffer, otherwise to window surface.
// Called from M3G (via CMIDGraphics) when EGL surface needs to be updated with
// 2D content. This will not be called if M3G Background is used for clearing
// entire canvas area.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UpdateEglContent()
{
    if (!IsEglAvailable())
    {
        return;
    }

    DEBUG("CMIDCanvas::UpdateEglContent() ++");

    // If scaling update framebuffer to pbuffer, otherwise to windowsurface
    if (iScalingOn && iFullScreen)
    {
        SetCurrentEglType(EEglPbuffer);
    }
    else
    {
        SetCurrentEglType(EEglWindow);
    }
    BlitFrameBufferPixels();
    SetCurrentEglType(EEglNone);

    DEBUG("CMIDCanvas::UpdateEglContent() --");
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::IsEglAvailable
// Return ETrue, if EGL based drawing is in use.
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::IsEglAvailable()
{
    return (iEglWindowSurface != EGL_NO_SURFACE);
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::BindEglSurface
// Called from M3G (via CMIDGraphics) when M3G binds to canvas
// graphics rendering target.
// ---------------------------------------------------------------------------
//
EGLSurface CMIDCanvas::BindEglSurface()
{
    if (IsEglAvailable())
    {
        iEglOccupied = ETrue;
    }

    if (iScalingOn && iFullScreen)
    {
        return iEglPbufferSurface;
    }
    return iEglWindowSurface;
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::ReleaseEglSurface
// Called from M3G (via CMIDGraphics) when M3G releases canvas
// graphics rendering target.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ReleaseEglSurface()
{
    iEglOccupied = EFalse;
    if (IsEglAvailable())
    {
        // Pending dispose. Egl context must be released.
        if (iEglPendingDispose)
        {
            DEBUG("CMIDCanvas::ReleaseEglSurface() - dispose egl");
            CloseEgl();
        }
        else if (iEglPendingResize)
        {
            DEBUG("CMIDCanvas::ReleaseEglSurface() - pending resize");
            HandleSizeChanged();
        }
        ClearFrameBuffer();
    }
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::UpdateOffScreenBitmap()
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UpdateOffScreenBitmapL(TBool aForced)
{
    if (IsEglAvailable() && iFrameContext && (IsGameCanvas() || aForced))
    {
        DEBUG("CMIDCanvas::UpdateOffScreenBitmapL() ++");

        UpdateEglContent();

        EGLSurface surface = EGL_NO_SURFACE;
        TEglType current = GetCurrentEglType();

        if (iScalingOn && iFullScreen)
        {
            SetCurrentEglType(EEglPbuffer);
            surface = iEglPbufferSurface;
        }
        else
        {
            SetCurrentEglType(EEglWindow);
            surface = iEglWindowSurface;
        }

        CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);
        User::LeaveIfError(bitmap->Create(
                               GetEglSurfaceSize(surface), iEnv.DisplayMode()));

        if (EGL_FALSE == eglCopyBuffers(iEglDisplay, surface, bitmap))
        {
            ELOG1(EJavaUI, "eglCopyBuffers() failed, eglError=%s",
                  GetEglError(eglGetError()).Ptr());
            User::Leave(KErrUnknown);
        }

        // Make framebuffer opaque
        iFrameContext->SetBrushColor(KOpaqueClearColor);
        iFrameContext->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        iFrameContext->Clear();

        iFrameContext->BitBlt(TPoint(), bitmap);
        CleanupStack::PopAndDestroy();
        SetCurrentEglType(current);

        // Make sure that full Canvas area
        // is made transparent in next ClearFrameBuffer() call.
        // This is needed in case UpdateOffScreenBitmapL is called
        // from Nokia UI API
        iUpperUpdateRect = TRect(Size());

        DEBUG("CMIDCanvas::UpdateOffScreenBitmapL() --");
    }
}

// ---------------------------------------------------------------------------
// From MMIDComponentNgaExtension
// CMIDCanvas::UpdateOffScreenBitmap()
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::FillEglSurface(const TRect& aRect, const TRgb& aColor)
{
    if (!IsEglAvailable() ||
            !iUpperUpdateRect.IsEmpty() ||
            !iLowerUpdateRect.IsEmpty())
    {
        return EFalse;
    }
    TSize sz(this->GetEglSurfaceSize());
    if (aRect.Width() >= sz.iWidth && aRect.Height() >= sz.iHeight)
    {
        return ClearEglSurface(EEglNone, &aColor);
    }
    TEglType current = GetCurrentEglType();
    if (iScalingOn && iFullScreen)
    {
        if (current != EEglPbuffer)
        {
            SetCurrentEglType(EEglPbuffer);
        }
    }
    else if (current != EEglWindow)
    {
        SetCurrentEglType(EEglWindow);
    }
    InitOpenGl(ETrue);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glVertexPointer(2, GL_SHORT, 0, iVertexArray);
    iVertexArray[0] = aRect.iTl.iX;
    iVertexArray[1] = aRect.iTl.iY;

    iVertexArray[2] = aRect.iBr.iX;
    iVertexArray[3] = aRect.iTl.iY;

    iVertexArray[4] = iVertexArray[2];
    iVertexArray[5] = aRect.iBr.iY;

    iVertexArray[6] = iVertexArray[0];
    iVertexArray[7] = iVertexArray[5];

    glColor4f(
        aColor.Red() / 255.0f, aColor.Green() / 255.0f,
        aColor.Blue() / 255.0f, aColor.Alpha() / 255.0f);

    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, iTriangleStrip);
    SetCurrentEglType(EEglNone);
    return ETrue;
}

// ---------------------------------------------------------------------------
// From CMIDCanvas
// CMIDCanvas::ClearEglSurface
// ---------------------------------------------------------------------------
//
TBool CMIDCanvas::ClearEglSurface(
    CMIDCanvas::TEglType aSurfaceType, const TRgb* aRgba)
{
    if (!IsEglAvailable())
    {
        return EFalse;
    }
    TEglType current = GetCurrentEglType();
    if (aSurfaceType == EEglNone || aRgba == NULL)
    {
        if (iScalingOn && iFullScreen)
        {
            if (aSurfaceType == EEglNone)
            {
                aSurfaceType = EEglPbuffer;
            }
            if (aRgba == NULL)
            {
                glClearColor(0.f, 0.f, 0.f, 1.f);
            }
        }
        else
        {
            if (aSurfaceType == EEglNone)
            {
                aSurfaceType = EEglWindow;
            }
            if (aRgba == NULL)
            {
                glClearColor(1.f, 1.f, 1.f, 1.f);
            }
        }
    }
    SetCurrentEglType(aSurfaceType);
    if (aRgba != NULL)
    {
        glClearColor(
            aRgba->Red() / 255.f, aRgba->Green() / 255.0f,
            aRgba->Blue() / 255.0f, aRgba->Alpha() / 255.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SetCurrentEglType(current);
    return ETrue;
}

// ---------------------------------------------------------------------------
// From MMIDCanvas
// CMIDCanvas::UpdateRect
// Notifies canvas about which areas have been updated with 2D content.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::UpdateRect(const TRect& aRect)
{
    if (aRect.IsEmpty())
    {
        return;
    }

    TRect rect(aRect);
    TRect canvasRect = IsDownScaling(iContentSize, iViewRect) ?
                       TRect(iViewRect.Size()) : TRect(iContentSize);
    rect.Intersection(canvasRect);

    // Update the member rects
    if (iUpperUpdateRect.Intersects(rect))
    {
        iUpperUpdateRect.BoundingRect(rect);
    }
    else if (iLowerUpdateRect.Intersects(rect))
    {
        iLowerUpdateRect.BoundingRect(rect);
    }
    // If rect is fully in the lower half of canvas,
    // merge it with iLowerUpdateRect, otherwise with iUpperUpdateRect
    else if (rect.iTl.iY > canvasRect.Height() / 2)
    {
        if (iLowerUpdateRect.IsEmpty())
        {
            iLowerUpdateRect = rect;
        }
        else
        {
            iLowerUpdateRect.BoundingRect(rect);
        }
    }
    else
    {
        if (iUpperUpdateRect.IsEmpty())
        {
            iUpperUpdateRect = rect;
        }
        else
        {
            iUpperUpdateRect.BoundingRect(rect);
        }
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::BlitFrameBufferPixels
// Sets up OpenGL state for 2D texture rendering and renders the textures for
// updated frame buffer areas by calling BlitSubRect().
// ---------------------------------------------------------------------------
//
void CMIDCanvas::BlitFrameBufferPixels()
{
    if (iUpperUpdateRect.IsEmpty() && iLowerUpdateRect.IsEmpty())
    {
        return;
    }
    InitOpenGl();
    // Must use this blending function because frame buffer
    // content is in pre-multiplied ARGB format.
    // See alpha blending in Lcdgd, lcdc16ma.cpp.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    // Disable any stray state we don't want
    glDisable(GL_CULL_FACE);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisable(GL_LIGHTING);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);

    ASSERT_GL();

    if (iUpperUpdateRect.Intersects(iLowerUpdateRect))
    {
        iUpperUpdateRect.BoundingRect(iLowerUpdateRect);
        iLowerUpdateRect = TRect();
        BlitSubRect(iUpperUpdateRect);
    }
    else
    {
        BlitSubRect(iUpperUpdateRect);
        BlitSubRect(iLowerUpdateRect);
    }

    ClearFrameBuffer();
}

// ---------------------------------------------------------------------------
// CMIDCanvas::InitOpenGl
// Initiazes OpenGL before 2d rendering
// ---------------------------------------------------------------------------
//
TSize CMIDCanvas::InitOpenGl(TBool aTopLeftOrigo, const TRect* aRect)
{
    // scaling is on the framebuffer is blitted to pbuffer instead of
    // window surface so get the size of appropriate surface for
    // setting correct values for viewport, projection etc.
    TSize size = GetEglSurfaceSize();
    glViewport(0, 0, size.iWidth, size.iHeight);
    if (aRect)
    {
        glScissor(aRect->iTl.iX, aRect->iTl.iY, aRect->Width(), aRect->Height());
    }
    else
    {
        glScissor(0, 0, size.iWidth, size.iHeight);
    }
    glEnable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Checks where the origo should be
    if (aTopLeftOrigo)
    {
        // Top-left
        glOrthof(0, TReal32(size.iWidth), TReal32(size.iHeight), 0.f, -1.f, 1.f);
    }
    else
    {
        // Bottom-left
        glOrthox(0, size.iWidth << 16, 0, size.iHeight << 16, -1 << 16, 1 << 16);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);
    return size;
}

// ---------------------------------------------------------------------------
// CMIDCanvas::BlitSubRect
// OpenGL texture dimensions must be powers of 2. Devides aRect into smaller
// areas for texture rendering so that texture dimensions won't exceed
// KMaxBlitSize.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::BlitSubRect(const TRect& aRect)
{
    if (aRect.IsEmpty())
    {
        return;
    }

    // OpenGL coordinate origin is in bottom left corner of screen
    TInt yOffset = iContentSize.iHeight - aRect.iBr.iY;
    TInt xOffset = aRect.iTl.iX;
    TInt width   = aRect.Width();
    TInt height  = aRect.Height();

    TInt canvasHeight = iContentSize.iHeight;

    TInt xBlits = (width == KMaxBlitSize) ? 1 : (width / KMaxBlitSize) + 1;
    TInt yBlits = (height == KMaxBlitSize) ? 1 : (height / KMaxBlitSize) + 1;

    TInt stride = iFrameBuffer->ScanLineLength(
                      iFrameBuffer->SizeInPixels().iWidth, iFrameBuffer->DisplayMode());

    for (TInt yBlit = 0; yBlit < yBlits; ++yBlit)
    {
        for (TInt xBlit = 0; xBlit < xBlits; ++xBlit)
        {

            TInt xStart = xOffset + xBlit * KMaxBlitSize;
            TInt yStart = yOffset + yBlit * KMaxBlitSize;
            TInt xSize = Min(KMaxBlitSize, width - (xStart - xOffset));
            TInt ySize = Min(KMaxBlitSize, height - (yStart - yOffset));

            // The first rectangle will be in bottom left corner,
            // then rectangle is moved right and upwards.
            // OpenGL y-coordinate increases upwards.
            TInt srcOffset = (canvasHeight - (yStart + ySize)) * stride +
                             xStart * KBytesPerPixel;

            BlitSubRectTexture(xStart, yStart, xSize, ySize, stride,
                               (TUint8*)iFrameBuffer->DataAddress() + srcOffset);
        }
    }
}


// ---------------------------------------------------------------------------
// CMIDCanvas::BlitSubRectTexture
// Creates OpenGL texture from the pixels passed as a parameter and draws
// texture to the current EGL surface.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::BlitSubRectTexture(TInt aXOffset, TInt aYOffset,
                                    TInt aWidth, TInt aHeight,
                                    TInt aStride, const TUint8* aPixels)
{
    DEBUG_INT4("BlitSubRectTextureL: aXOffset=%d, aYOffset=%d, aWidth=%d, aHeight=%d",
               aXOffset, aYOffset, aWidth, aHeight);
    DEBUG_INT("BlitSubRectTextureL: aStride=%d", aStride);

    ASSERT((aWidth > 0) && (aWidth <= 256));
    ASSERT((aHeight > 0) && (aHeight <= 256));
    ASSERT(iTexturePixels);
    ASSERT_GL();

    TInt tileWidth = KMaxBlitSize;
    TInt tileHeight = KMaxBlitSize;

    // Tweak tile size to avoid using excessive amounts of memory for
    // portions outside the blit area
    while (tileWidth >= aWidth * 2)
    {
        tileWidth >>= 1;
        tileHeight <<= 1;
    }

    while (tileHeight >= aHeight * 2)
    {
        tileHeight >>= 1;
    }

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    TUint tempTexObj[KTexturesCount];
    glGenTextures(KTexturesCount, tempTexObj);

    glBindTexture(GL_TEXTURE_2D, tempTexObj[0]);

    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 tileWidth, tileHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Raise out-of-memory if OpenGL ran out of resources
    GLint err = glGetError();
    if (err == GL_OUT_OF_MEMORY)
    {
        glDeleteTextures(KTexturesCount, tempTexObj);
        DEBUG("CMIDCanvas::BlitSubRectTexture(): Out of memory when creating OpenGL texture");
        return;
    }
    else if (err != GL_NO_ERROR)
    {
        ELOG1(EJavaUI, "CMIDCanvas::BlitSubRectTexture(): GL error after glTexImage2D(): %d", err);
        ASSERT(EFalse);
    }

    // Set up texture and vertex coordinate arrays for the image tiles
    const TUint8 tc[8] = { 0, 0, 0, 1, 1, 0, 1, 1 };
    GLshort pos[8];

    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_BYTE, 0, tc);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_SHORT, 0, pos);
    glEnableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    ASSERT_GL();

    glBindTexture(GL_TEXTURE_2D, tempTexObj[0]);
    TUint8* dst =  iTexturePixels;
    const TUint8* src = aPixels;

    TInt h = aHeight;

    if (tileWidth > aWidth ||
            tileHeight > aHeight)
    {
        // Clear the pixel data with transparent for case where
        // actual texture data does not cover the full tile.
        // This should be actually done with glClear().
        Mem::FillZ(dst, tileWidth * tileHeight * KBytesPerPixel);
        dst += tileWidth * (tileHeight - aHeight) * KBytesPerPixel;
    }

    while (h-- > 0)
    {
        ConvertPixels((TUint32*)src, dst, aWidth);
        src += aStride;
        dst += tileWidth * KBytesPerPixel;
    }

    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,
                    tileWidth, tileHeight,
                    GL_RGBA, GL_UNSIGNED_BYTE, iTexturePixels);

    // If scaling is on and pbuffer is used, the framebuffer contents
    // must be positioned to the top part of the puffer as m3g renders
    // there as well, thus 2D and 3D content are correctly on top of each other
    if (iScalingOn && iFullScreen)
    {
        TSize pbufferSize = GetEglSurfaceSize(iEglPbufferSurface);
        TInt offset = pbufferSize.iHeight - iContentSize.iHeight;

        pos[0] = (GLshort)aXOffset;
        pos[1] = (GLshort)(tileHeight + aYOffset + offset);
        pos[2] = pos[0];
        pos[3] = (GLshort)aYOffset + offset;
        pos[4] = (GLshort)(tileWidth + aXOffset);
        pos[5] = pos[1];
        pos[6] = pos[4];
        pos[7] = pos[3];
    }
    else
    {
        pos[0] = (GLshort)aXOffset;
        pos[1] = (GLshort)(tileHeight + aYOffset);
        pos[2] = pos[0];
        pos[3] = (GLshort)aYOffset;
        pos[4] = (GLshort)(tileWidth + aXOffset);
        pos[5] = pos[1];
        pos[6] = pos[4];
        pos[7] = pos[3];
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // release resources
    glDeleteTextures(KTexturesCount, tempTexObj);
    ASSERT_GL();
}

// ---------------------------------------------------------------------------
// CMIDCanvas::CreatePBufferSurfaceL
// Create EGL pbuffer to be used in scaling case.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::CreatePBufferSurfaceL()
{
    DEBUG("CMIDCanvas::CreatePBufferSurface() ++");
    ASSERT(iEglDisplay != EGL_NO_DISPLAY);

    RWindow& window = Window();

    // Choose the buffer size based on the Window's display mode.
    TDisplayMode displayMode = window.DisplayMode();
    TInt bufferSize = 0;
    switch (displayMode)
    {
    case(EColor16MU):
    case(EColor16MA):
    case(EColor16MAP):
        bufferSize = 32;
        break;
    default:
        ELOG1(EJavaUI, "CMIDCanvas::OpenEglL(): unsupported window display mode %d",
              displayMode);
        User::Leave(KErrNotSupported);
        break;
    }

    // Set the desired properties for the EGLSurface
    const EGLint attributeList[] =
    {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_BUFFER_SIZE, bufferSize,
        EGL_BIND_TO_TEXTURE_RGBA, EGL_TRUE,
        EGL_TRANSPARENT_TYPE, EGL_NONE,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };

    EGLConfig bestConfig;
    EGLint numConfigs = 0;

    // Choose the best EGLConfig that matches the desired properties.
    if (!eglChooseConfig(iEglDisplay, attributeList, &bestConfig, 1, &numConfigs) ||
            numConfigs == 0)
    {
        ELOG1(EJavaUI, "eglChooseConfig() for pbuffer failed, eglError=%d", eglGetError());
        User::Leave(KErrNotSupported);
    }

    // find buffer size based on the framebuffer size
    // pbuffer size must be power of two so that it can be bound
    // as texture image
    TInt width = 2;
    TInt height = 2;

    while (width < iContentSize.iWidth)
    {
        width <<= 1;
    }
    while (height < iContentSize.iHeight)
    {
        height <<= 1;
    }

    DEBUG_INT4("CMIDCanvas::CreatePBufferSurface - pbuffer size resolved as (width=%d, height=%d), contentSize(%d, %d)",
               width, height, iContentSize.iWidth, iContentSize.iHeight);

    // Special attributes in order for binding this surface as texture image
    EGLint attrib[] =
    {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
        EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
        EGL_MIPMAP_TEXTURE, EGL_TRUE,
        EGL_NONE
    };

    iEglPbufferSurface = eglCreatePbufferSurface(iEglDisplay, bestConfig, attrib);
    if (iEglPbufferSurface == EGL_NO_SURFACE)
    {
        ELOG1(EJavaUI, "eglCreatePbufferSurface() failed, eglError=%d", eglGetError());
        User::Leave(KErrNoMemory);
    }

    // Create a rendering context
    iEglPbufferSurfaceContext = eglCreateContext(iEglDisplay, bestConfig, NULL, NULL);
    if (iEglPbufferSurfaceContext == EGL_NO_CONTEXT)
    {
        ELOG1(EJavaUI, "eglCreateContext() for pbuffer failed, eglError=%d", eglGetError());
        User::Leave(KErrNoMemory);
    }

    DEBUG("CMIDCanvas::CreatePBufferSurface() --");
}

// ---------------------------------------------------------------------------
// CMIDCanvas::DisposePBufferSurface
// Destroys EGL pbuffer surface and context.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::DisposePBufferSurface()
{
    if (iEglPbufferSurfaceContext != EGL_NO_CONTEXT)
    {
        eglDestroyContext(iEglDisplay, iEglPbufferSurfaceContext);
        iEglPbufferSurfaceContext = EGL_NO_CONTEXT;

        // textures are destoyed along with the context so it's safe
        // set texture name to zero
        iPbufferTexture = 0;
    }
    if (iEglPbufferSurface != EGL_NO_SURFACE)
    {
        eglDestroySurface(iEglDisplay, iEglPbufferSurface);
        iEglPbufferSurface = EGL_NO_SURFACE;
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::BlitPBufferScaledToWindowSurface
// In scaling case renders and scales pbuffer content (=canvas 2D + 3D
// content in orig size) to EGL window surface.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::BlitPBufferScaledToWindowSurface()
{
    DEBUG("CMIDCanvas::BlitPBufferScaledToWindowSurface() ++");

    // Get screen size for setting clips and
    // parallel projection correctly
    TRect screenRect = Rect();
    TInt width = screenRect.Width();
    TInt height = screenRect.Height();

    glScissor(0, 0, width, height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrthox(0, width << 16,
             0, height << 16,
             -1 << 16, 1 << 16);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    // generate texture name if not already generated
    if (glIsTexture(iPbufferTexture) == GL_FALSE)
    {
        glGenTextures(1, &iPbufferTexture);
    }

    glBindTexture(GL_TEXTURE_2D, iPbufferTexture);
    if (eglBindTexImage(iEglDisplay, iEglPbufferSurface, EGL_BACK_BUFFER) == EGL_FALSE)
    {
        ELOG1(EJavaUI, "eglBindTexImage() failed, eglError=%d", eglGetError());
        ASSERT(EFalse);
    }

    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // calculate texture coordinates, i.e. the area in pbuffer
    // that has the actual frame content
    GLfloat contentWidth = iContentSize.iWidth;
    GLfloat contentHeight = iContentSize.iHeight;
    TSize bufferSize = GetEglSurfaceSize(iEglPbufferSurface);
    GLfloat horRatio = contentWidth / bufferSize.iWidth;
    GLfloat yoffset = (bufferSize.iHeight - contentHeight) / bufferSize.iHeight;
    GLfloat tc[8] = { 0.0f, 1.0f, 0.0f, yoffset, horRatio, 1.0f, horRatio, yoffset};

    GLshort pos[8];

    ASSERT_GL();

    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(2, GL_FLOAT, 0, tc);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_SHORT, 0, pos);
    glEnableClientState(GL_VERTEX_ARRAY);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    // position texture screen coordinates
    pos[0] = (GLshort)iViewRect.iTl.iX;
    pos[1] = (GLshort)iViewRect.Height() + (height - iViewRect.iBr.iY);
    pos[2] = pos[0];
    pos[3] = (GLshort)height - iViewRect.iBr.iY;
    pos[4] = (GLshort)iViewRect.iBr.iX;
    pos[5] = pos[1];
    pos[6] = pos[4];
    pos[7] = pos[3];

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // release pbuffer from texture
    eglReleaseTexImage(iEglDisplay, iEglPbufferSurface, EGL_BACK_BUFFER);

    DEBUG("CMIDCanvas::BlitPBufferScaledToWindowSurface() --");
}


// ---------------------------------------------------------------------------
// CMIDCanvas::ClearFrameBuffer
// Clears the frame buffer with fully transparent color. Rendering canvas 2D
// content as an OpenGL texture requires that texture is transparent in
// the areas where no 2D content has been drawn.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::ClearFrameBuffer()
{
    if (!iM3GStart && iUpperUpdateRect.IsEmpty() && iLowerUpdateRect.IsEmpty())
    {
        return;
    }
    // Clear the frame buffer
    iFrameContext->SetBrushColor(KTransparentClearColor);
    iFrameContext->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);

    iFrameContext->Clear(iUpperUpdateRect);
    iFrameContext->Clear(iLowerUpdateRect);

    // Reset members
    iUpperUpdateRect.SetRect(0,0,0,0);
    iLowerUpdateRect.SetRect(0,0,0,0);
}


// ---------------------------------------------------------------------------
// CMIDCanvas::AssertGL
// Check for OpenGL ES errors. Only for debug builds.
// ---------------------------------------------------------------------------
//
void CMIDCanvas::AssertGL()
{
    GLint err = glGetError();
    if (err == GL_NO_ERROR)
    {
        return;
    }
    else
    {
        ELOG1(EJavaUI, "OpenGL ES error=%d", err);
        ASSERT(EFalse);
    }
}

// ---------------------------------------------------------------------------
// CMIDCanvas::AssertGL
// Check for OpenGL ES errors. Only for debug builds.
// ---------------------------------------------------------------------------
//
const TDesC8& CMIDCanvas::GetEglError(EGLint aErr)
{
    _LIT8(KEglSuccess, "EGL_SUCCESS");
    _LIT8(KEglNotInitialized, "EGL_NOT_INITIALIZED");
    _LIT8(KEglBadAccess, "EGL_BAD_ACCESS");
    _LIT8(KEglBadAlloc, "EGL_BAD_ALLOC");
    _LIT8(KEglBadAttribute, "EGL_BAD_ATTRIBUTE");
    _LIT8(KEglBadContext, "EGL_BAD_CONTEXT");
    _LIT8(KEglBadConfig, "EGL_BAD_CONFIG");
    _LIT8(KEglBadCurrentSurface, "EGL_BAD_CURRENT_SURFACE");
    _LIT8(KEglBadDisplay, "EGL_BAD_DISPLAY");
    _LIT8(KEglBadSurface, "EGL_BAD_SURFACE");
    _LIT8(KEglBadMatch, "EGL_BAD_MATCH");
    _LIT8(KEglBadParameter, "EGL_BAD_PARAMETER");
    _LIT8(KEglBadNativePixmap, "EGL_BAD_NATIVE_PIXMAP");
    _LIT8(KEglBadNativeWindow, "EGL_BAD_NATIVE_WINDOW");
    switch (aErr)
    {
    case EGL_NOT_INITIALIZED:
        return KEglNotInitialized;
    case EGL_BAD_ACCESS:
        return KEglBadAccess;
    case EGL_BAD_ALLOC:
        return KEglBadAlloc;
    case EGL_BAD_ATTRIBUTE:
        return KEglBadAttribute;
    case EGL_BAD_CONTEXT:
        return KEglBadContext;
    case EGL_BAD_CONFIG:
        return KEglBadConfig;
    case EGL_BAD_CURRENT_SURFACE:
        return KEglBadCurrentSurface;
    case EGL_BAD_DISPLAY:
        return KEglBadDisplay;
    case EGL_BAD_SURFACE:
        return KEglBadSurface;
    case EGL_BAD_MATCH:
        return KEglBadMatch;
    case EGL_BAD_PARAMETER:
        return KEglBadParameter;
    case EGL_BAD_NATIVE_PIXMAP:
        return KEglBadNativePixmap;
    case EGL_BAD_NATIVE_WINDOW:
        return KEglBadNativeWindow;
    }
    return KEglSuccess;
}


#endif // RD_JAVA_NGA_ENABLED        
// End of File.
